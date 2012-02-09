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
		'#type' => 'hidden',
		'#value' => $node->nid,
	);
	$form['pid']=array
	(
		'#type' => 'hidden',
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
		'#type' => 'hidden',
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
	$form['language']=array
	(
		'#type'=>'select',
		'#title'=>t('Language'),
		'#options' => array('G++'=>'G++','GCC'=>'GCC','JAVA'=>'JAVA','Pascal'=>'Pascal','Masm32'=>'Masm32'),
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
	$item = db_fetch_object(db_query("SELECT sid FROM {solution} WHERE uid=%d AND pid=%d LIMIT 1",$user->uid,$form_state['values']['pid']));
	if($item->sid==null)
		db_query("UPDATE {content_type_problem} SET field_submit_user_value=field_submit_user_value+1 WHERE field_pid_value=%d",$form_state['values']['pid']);
	db_query("UPDATE {content_type_problem} SET field_submit_value=field_submit_value+1 WHERE field_pid_value=%d",$form_state['values']['pid']);
	$len = strlen($form_state['values']['source_code']);
	db_query("INSERT INTO {solution} (pid,uid,in_date,result,language,ip,num,code_length,source) VALUES (%d,%d,%d,%d,%d,'%s',%d,%d,compress('%s'))",
		$form_state['values']['pid'],
		$user->uid,
		time(),
		10000,
		$form_state['values']['language'],
		$_SERVER['REMOTE_ADDR'],
		-1,
		$len,
		$form_state['values']['source_code']
		);
	
	$item = db_fetch_object(db_query("SELECT CAST(value AS SIGNED) AS submissions FROM {profile_values} WHERE fid=5 AND uid=%d",$user->uid));
	$submissions = ($item->submissions+1);
	if($item)
		db_query("UPDATE {profile_values} SET value=%s WHERE fid=5 AND uid=%d",$submissions,$user->uid);
	else
	{
		db_query("INSERT INTO {profile_values} VALUES (%d,%d,%s)",5,$user->uid,$submissions);
		db_query("INSERT INTO {profile_values} VALUES (%d,%d,'0')",4,$user->uid);
	}
	
	db_query("UPDATE {node} SET changed=%d WHERE nid=%d",time(),$form_state['values']['nid']);
	$cid = "content:".$form_state['values']['nid'];
	cache_clear_all($cid,'cache_content',TRUE);
	drupal_set_message("Source code submitted sucessfully.");
	drupal_goto('status');
}

function reset_problem_submit(&$form, &$form_state)
{
	$form_state['values']['source_code'] = NULL;
}

function show_code($sid)
{
	if($sid==null)
	{
		drupal_set_message("No such solution.");
		drupal_goto('status');
	}
	$query = "SELECT s.*,u.name,uncompress(source) AS source FROM {solution} s LEFT JOIN {users} u ON s.uid=u.uid WHERE sid=%d";
	$items = db_query($query,$sid);
	$language = array('G++','GCC','JAVA','Pascal','Masm32');
	if($item = db_fetch_object($items))
	{
		//debug($item,'$item');
		$result = getResultDescript($item->result);
$script=<<<SCR
    function getCode(name,css)
    {
        document.getElementById(name).className = css;
        dp.SyntaxHighlighter.ClipboardSwf = 'dp.SyntaxHighlighter/Scripts/clipboard.swf';   
        dp.SyntaxHighlighter.HighlightAll(name);        
    }
SCR;
	$path = drupal_get_path('module','oj');
	drupal_add_js($script,'inline');
	drupal_add_css($path.'/misc/js/dp.SyntaxHighlighter/Styles/SyntaxHighlighter.css');
	drupal_add_js($path.'/misc/js/dp.SyntaxHighlighter/Scripts/shCore.js');
	drupal_add_js($path.'/misc/js/dp.SyntaxHighlighter/Scripts/shBrushCpp.js');
	drupal_add_js($path.'/misc/js/dp.SyntaxHighlighter/Scripts/shBrushJava.js');
	drupal_add_js($path.'/misc/js/dp.SyntaxHighlighter/Scripts/shBrushDelphi.js');
	drupal_add_js($path.'/misc/js/dp.SyntaxHighlighter/Scripts/shBrushXml.js');
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
		<font color=#006600>Language</font>:{$language[$item->language]}  <font color=#006600>Result</font>:{$result} <br></p>
SCR;
	if($item->result == 7)
	{
		drupal_add_js('misc/collapse.js');
		$item->error = preg_replace('![A-Z]:\\\\.+\\\\(\d+)!','${1}',$item->error);
		$item->error = oj_check_markup($item->error);
		$output .= "
		<fieldset class='fieldgroup field-compile-error collapsible'>
		<legend>Compile Error</legend>
		<div class='field-items'>$item->error</div>
		</fieldset>";
	}
$output .=<<<SCR
		<ul><li><font color=#333399 size=5>Source</font></li></ul>
		<textarea align='left' id='code' name='code' class='cpp' style='width: 600px;'>
{$item->source}
		</textarea>
		<script language='javascript' type='text/javascript'>dp.SyntaxHighlighter.HighlightAll('code');</script>
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
	if($sid==null)
		return false;
	global $user;
	if($user->uid==0)
		return false;
	if($user->uid==1)
		return true;
	if($user->roles['4']!=null||$user->roles['5']!=null)
		return true;
	//关闭了view code
	//题目处于比赛中，以前提交的代码
	
	//Source Browser
	if($user->roles['3']!=null)
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
		if($result->result==null||$result->result!=0)
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
	if($sid==null)
	{
		drupal_set_message("No such solution.");
		drupal_goto('status');
	}
	$query = "SELECT s.*,u.name,uncompress(source) AS source FROM {solution} s LEFT JOIN {users} u ON s.uid=u.uid WHERE sid=%d AND result=7";
	$items = db_query($query,$sid);
	$language = array('G++','GCC','JAVA','Pascal','Masm32');
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
	if($sid==null)
		return false;
	$item = db_fetch_object(db_query("SELECT error FROM {solution} WHERE sid=%d AND result=7",$sid));
	if($item->error==null)
		return false;
	global $user;
	if($user->uid==0)
		return false;
	if($user->uid==1)
		return true;
	if($user->roles['3']!=null||$user->roles['4']!=null||$user->roles['5']!=null)
		return true;
}
