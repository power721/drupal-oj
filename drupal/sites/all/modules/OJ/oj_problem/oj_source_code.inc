<?php

function oj_problem_submit_access($node)
{
	global $user;
	if($user->uid==0)
		return false;
	if($node->type=='problem')
		return true;
	return false;
}

function oj_problem_submit_callback($node)
{
	global $user;
	
	$output = "<center>";
	//$output .= "Current Authenticated Author : <a href=../../user/$user->uid><font color=blue>" . $user->name . "</font></a><br><br>";
	$output .= drupal_get_form('oj_problem_submit_form',$node);
	$output .= "</center>";
	return $output;
}

function oj_problem_submit_form(&$form_state=NULL,$node)
{
	global $user;
	$user_account = user_load($user->uid);
	//debug($node);
	//debug($form_state,'$form_state');
	//debug($user_account->profile_language);
	$form['nid']=array
	(
		'#type' => 'value',
		'#value' => $node->nid,
	);
	$form['pid']=array
	(
		'#type' => 'value',
		'#value' => $node->field_pid['0']['value'],
	);
	$form['problem_id']=array
	(
		'#type' => 'item',
		'#title' => t('Problem ID'),
		'#value' => $node->field_pid['0']['value'],
		'#prefix' => '<div class=container-inline>',
		'#suffix' => '</div>'
	);
	/*
	$form['problem_title_value']=array
	(
		'#type' => 'value',
		'#value' => $node->title,
	);
	$form['problem_title']=array
	(
		'#type' => 'item',
		'#title' => t('Problem Title'),
		'#value' => '<a href=.>'.$node->title.'</a>',
		'#prefix' => '<div class=container-inline>',
		'#suffix' => '</div>'
	);
	*/
	$languages = oj_languages();
	$form['language']=array
	(
		'#type'=>'select',
		'#title'=>t('Language'),
		'#options' => $languages,
		'#default_value' => $user_account->profile_language,
		'#required' => TRUE,
		'#prefix' => '<div class=container-inline>',
		'#suffix' => '</div>'
	);
	
	$form['source_code']=array
	(
		'#type'=>'textarea',
		'#title'=>t('Source Code'),
		'#required' => TRUE,
		'#rows' => 30
	);
	
	$form['problem_submit'] = array
	(
		'#type' => 'submit',
		'#value' => 'Submit',
		'#executes_submit_callback' => TRUE,
		'#submit' => array('problem_submit'),
		'#validate' => array('problem_submit_validate'),
	);

	/*$form['problem_reset'] = array
	(
		'#type' => 'submit',
		'#value' => 'Reset',
		'#executes_submit_callback' => false,
		'#submit' => array('reset_problem_submit')
	);
	*/
	$form['problem_reset'] = array 
	(
		'#value' => '<input type="reset" value="Reset!">'
	);
	return $form;
}

function problem_submit_validate($form, &$form_state)
{
	$len = strlen($form_state['values']['source_code']);
	if($len<10)
		form_set_error(source_code, "Source code too short,submit FAILED;if you really need submit this source please contact administrator");
}

function problem_submit(&$form, &$form_state)
{
	global $user;
	$array = oj_languages();
	$id = 0;
	foreach($array as $value)
	{
		$language[$value] = $id++;
	}
	//$language = array('G++'=>0,'GCC'=>1,'JAVA'=>2,'Pascal'=>3,'Masm32'=>4);
	$item = db_fetch_object(db_query("SELECT sid FROM {solution} WHERE uid=%d AND pid=%d LIMIT 1"
			,$user->uid,$form_state['values']['pid']));
	if($item->sid==NULL)
		db_query("UPDATE {content_type_problem} SET field_submit_user_value=field_submit_user_value+1 
				WHERE field_pid_value=%d",$form_state['values']['pid']);
	db_query("UPDATE {content_type_problem} SET field_submit_value=field_submit_value+1 
			WHERE field_pid_value=%d",$form_state['values']['pid']);
	$len = strlen($form_state['values']['source_code']);
	$time = time();
	db_query("INSERT INTO {solution} (pid,uid,in_date,result,language,ip,num,code_length,source)
			VALUES (%d,%d,%d,%d,%d,'%s',%d,%d,compress('%s'))",
		$form_state['values']['pid'],
		$user->uid,
		$time,
		10000,
		$language[$form_state['values']['language']],
		$_SERVER['REMOTE_ADDR'],
		-1,
		$len,
		$form_state['values']['source_code']
		);
	//$sid = db_result(db_query("SELECT sid FROM {solution} WHERE pid=%d AND uid=%d AND in_date=%d ORDER BY sid DESC LIMIT 1",$form_state['values']['pid'],$user->uid,$time));
	$sid = db_result(db_query("SELECT LAST_INSERT_ID()"));
	
	$account = user_load($user->uid);
	user_save($account,array('profile_submissions'=>$account->profile_submissions+1));
	
	db_query("UPDATE {node} SET changed=%d WHERE nid=%d",time(),$form_state['values']['nid']);
	$cid = "content:".$form_state['values']['nid'];
	cache_clear_all($cid,'cache_content',TRUE);
	
	oj_judge($sid);
	
	drupal_set_message("Source code submitted sucessfully.");
	drupal_goto('status');
	
}

function reset_problem_submit(&$form, &$form_state)
{
	$form_state['values']['source_code'] = NULL;
}

function show_code($sid)
{
	if($sid==NULL)
	{
		drupal_set_message("No such solution.");
		drupal_goto('status');
	}
	$query = "SELECT s.*,u.name,uncompress(source) AS source FROM {solution} s LEFT JOIN {users} u ON s.uid=u.uid WHERE sid=%d";
	$items = db_query($query,$sid);
	$language = oj_languages(TRUE);
	//$language = array('G++','GCC','JAVA','Pascal','Masm32');
	$type = array('cpp','c','java','pascal','xml','c','cpp');
	if($item = db_fetch_object($items))
	{
		$result = getResultDescript($item->result);

		$path = drupal_get_path('module','oj').'/misc/js/dp.SyntaxHighlighter/';
		drupal_add_css($path.'Styles/shCore.css');
		drupal_add_css($path.'Styles/shThemeDefault.css');
		//drupal_add_css($path.'Styles/shThemeEclipse.css');
		drupal_add_js($path.'Scripts/shCore.js');
		switch($item->language)
		{
			case 0:
			case 1:drupal_add_js($path.'Scripts/shBrushCpp.js');break;
			case 2:drupal_add_js($path.'Scripts/shBrushJava.js');break;
			case 3:drupal_add_js($path.'Scripts/shBrushDelphi.js');break;
			case 4:drupal_add_js($path.'Scripts/shBrushXml.js');break;
			case 5:
			case 6:drupal_add_js($path.'Scripts/shBrushCpp.js');break;
		}
		$problem_url = url('problem/'.$item->pid);
		$user_url = url('user/'.$item->uid);
		$item->memory = (int)$item->memory;
		$item->time = (int)$item->time;
		$output =<<<SCR
			<p align=center><font size=5 color=#333399>View Code</font></p>
			<p align=center>
			<font color=#006600>Problem ID</font>:<a href={$problem_url}>{$item->pid}</a>  
			<font color=#006600>User ID</font>:<a href={$user_url}>{$item->name}</a> <br>
			<font color=#006600>Memory</font>:{$item->memory}K  <font color=#006600>Time</font>:{$item->time}MS <br>
			<font color=#006600>Language</font>:{$language[$item->language]}  <font color=#006600>Result</font>:{$result} <br>
SCR;
		if(oj_rejudge_access())
		{
			$output .= "<a href=".url("rejudge/sid/$sid").">Rejudge</a>";
		}
		$output .= "</p>";
		if($item->result == 7)
		{
			drupal_add_js('misc/collapse.js');
			$item->error = preg_replace('![a-zA-Z]:\\\\.+\\\\(\d+)!','${1}',$item->error);
			$item->error = oj_check_markup($item->error);
			$output .= "
			<fieldset class='fieldgroup field-compile-error collapsible'>
			<legend>Compile Error</legend>
			<div class='field-items'>$item->error</div>
			</fieldset>";
		}
		$output .= <<<SCR
			<ul><li><font color=#333399 size=5>Source</font></li></ul>
			
			<script type="syntaxhighlighter" class="brush: {$type[$item->language]};toolbar: false;"><![CDATA[ {$item->source}]]></script> 
			<script type="text/javascript">SyntaxHighlighter.all();</script>
			
SCR;
		print theme('page', $output);
	}
	else
	{
		drupal_set_message("No such solution.");
		drupal_goto('status');
	}
}

function show_code_access($sid)
{
	if($sid==NULL)
		return false;
	global $user;
	if($user->uid==0)
		return false;
	if($user->uid==1)
		return true;
	if($user->roles['4']!=NULL||$user->roles['5']!=NULL)
		return true;
	//关闭了view code
	//题目处于比赛中，以前提交的代码
	
	//Source Browser
	if($user->roles['3']!=NULL)
		return true;
	
	$items = db_query("SELECT s.uid,s.pid,p.value FROM {solution} s LEFT JOIN {profile_values} p ON s.uid=p.uid AND p.fid=8 WHERE sid=%d",$sid);
	if($item = db_fetch_object($items))
	{
		if($item->uid==$user->uid)
			return true;
		//AC共享计划
		//debug($item,'item');
		if($item->value!=1)
			return false;
		$result=db_fetch_object(db_query("SELECT p.*,min(result) AS result FROM {solution} s LEFT JOIN {profile_values} p ON s.uid=p.uid AND p.fid=8 WHERE s.uid=%d AND s.pid=%d",$user->uid,$item->pid));
		if($result->result==NULL||$result->result!=0)
			return false;
		//debug($result);
		return true;
	}
	else
		return false;
	//debug($sid);
	//debug($user,'$user');
	//debug($user_account,'$user_account');
	return false;
}

function show_compile_info($sid)
{
	if($sid==NULL)
	{
		drupal_set_message("No such solution.");
		drupal_goto('status');
	}
	$query = "SELECT s.*,u.name,uncompress(source) AS source FROM {solution} s LEFT JOIN {users} u ON s.uid=u.uid WHERE sid=%d AND result=7";
	$items = db_query($query,$sid);
	$language = oj_languages(TRUE);
	//$language = array('G++','GCC','JAVA','Pascal','Masm32');
	if($item = db_fetch_object($items))
	{
		$problem_url = url('problem/'.$item->pid);
		$user_url = url('user/'.$item->uid);
		$code_url = url('code/'.$item->sid);
		$item->error = oj_check_markup($item->error);
		$output=<<<SCR
		<p align=center><font size=5 color=#333399>Compile Error</font></p>
		<p align=center>
		<font color=#006600>Problem ID</font>:<a href={$problem_url}>{$item->pid}</a>  
		<font color=#006600>User ID</font>:<a href={$user_url}>{$item->name}</a> <br>
		<font color=#006600>Language</font>:<a href={$code_url}>{$language[$item->language]}</a> <br>
		</p>
		<hr>
		<div id=compile_info-info>{$item->error}</div>
SCR;
		print theme('page', $output);
	}
	else
	{
		drupal_set_message("No such solution.");
		drupal_goto('status');
	}
}

function show_compile_info_access($sid)
{
	if($sid==NULL)
		return false;
	$item = db_fetch_object(db_query("SELECT error FROM {solution} WHERE sid=%d AND result=7",$sid));
	if($item->error==NULL)
		return false;
	global $user;
	if($user->uid==0)
		return false;
	if($user->uid==1)
		return true;
	if($user->roles['3']!=NULL||$user->roles['4']!=NULL||$user->roles['5']!=NULL)
		return true;
}
