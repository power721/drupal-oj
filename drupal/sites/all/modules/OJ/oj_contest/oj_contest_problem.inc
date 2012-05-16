<?php

function oj_contest_problem_submit($cid,$num)
{
	$status = get_contest_status($cid);
	if($status == 0)
	{
		drupal_set_message("Contest $cid not start.",'info');
		drupal_goto("contest/$cid");
	}
	if($status == 2)
	{
		drupal_set_message("Contest $cid is ended.",'info');
		$pid = db_result(db_query("SELECT pid FROM {contest_problem} WHERE cid=%d AND num=%d",$cid,ord($num)-ord('A')+1));
		drupal_goto("problem/$pid/submit");
	}
	$title = db_result(db_query("SELECT title FROM {contests} WHERE cid=%d",$cid));
	drupal_set_title(check_plain("contest $cid -- Submit"));
	$breadcrumb = array();
	$breadcrumb[] = l('Home', '<front>');
	$breadcrumb[] = l('Contests', 'contests');
	$breadcrumb[] = l($cid, 'contest/'.$cid);
	$breadcrumb[] = l('problem', 'contest/'.$cid.'/problem');
	drupal_set_breadcrumb($breadcrumb);
	
	$output = "<center>";
	$output .= drupal_get_form('oj_contest_problem_submit_form',$cid,$num);
	$output .= "</center>";
	return $output;
}

function oj_contest_problem_submit_form(&$form_state=NULL,$cid,$num)
{
	global $user;
	$user_account = user_load($user->uid);
	$pid = db_result(db_query("SELECT pid FROM {contest_problem} WHERE cid=%d AND num=%d",$cid,ord($num)-ord('A')+1));
	$node = problem_load($pid);
	
	$form['cid']=array
	(
		'#type' => 'value',
		'#value' => $cid,
	);
	$form['pid']=array
	(
		'#type' => 'value',
		'#value' => $pid,
	);
	$form['nid']=array
	(
		'#type' => 'value',
		'#value' => $node->nid,
	);
	$form['num']=array
	(
		'#type' => 'value',
		'#value' => ord($num)-ord('A')+1,
	);
	$form['contest_id']=array
	(
		'#type' => 'item',
		'#title' => t('Contest ID'),
		'#value' => "<a href=../../>".$cid."</a>",
		'#prefix' => '<div class=container-inline>',
		'#suffix' => '</div>'
	);
	/*$form['problem_id']=array
	(
		'#type' => 'item',
		'#title' => t('Problem ID'),
		'#value' => $num,
		'#prefix' => '<div class=container-inline>',
		'#suffix' => '</div>'
	);*/
	$form['problem']=array
	(
		'#type' => 'item',
		'#title' => t("Problem")." $num",
		'#value' => "<a href=./>".$node->title."</a>",
		'#prefix' => '<div class=container-inline>',
		'#suffix' => '</div>'
	);
	$languages = oj_contest_language($cid);
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
		'#submit' => array('contest_problem_submit'),
		'#validate' => array('problem_submit_validate'),
	);

	$form['problem_reset'] = array 
	(
		'#value' => '<input type="reset" value="Reset!">'
	);
	
	return $form;
}

function contest_problem_submit_validate($form, &$form_state)
{
	$len = strlen($form_state['values']['source_code']);
	if($len<10)
		form_set_error(source_code, "Source code too short,submit FAILED;if you really need submit this source please contact administrator");
}

function contest_problem_submit(&$form, &$form_state)
{
	global $user;
	$cid = $form_state['values']['cid'];
	$num = $form_state['values']['num'];
	$array = oj_languages();
	$id = 0;
	foreach($array as $value)
	{
		$language[$value] = $id++;
	}
	
	$item = db_fetch_object(db_query("SELECT sid FROM {solution} WHERE uid=%d AND pid=%d LIMIT 1"
			,$user->uid,$form_state['values']['pid']));
	if($item->sid==NULL)
		db_query("UPDATE {content_type_problem} SET field_submit_user_value=field_submit_user_value+1 
				WHERE field_pid_value=%d",$form_state['values']['pid']);
	db_query("UPDATE {content_type_problem} SET field_submit_value=field_submit_value+1 
			WHERE field_pid_value=%d",$form_state['values']['pid']);
	$len = strlen($form_state['values']['source_code']);
	$time = time();
	db_query("INSERT INTO {solution} (pid,uid,cid,in_date,result,language,ip,num,code_length,source)
			VALUES (%d,%d,%d,%d,%d,%d,'%s',%d,%d,compress('%s'))",
		$form_state['values']['pid'],
		$user->uid,
		$cid,
		$time,
		10000,
		$language[$form_state['values']['language']],
		$_SERVER['REMOTE_ADDR'],
		$num,
		$len,
		$form_state['values']['source_code']
		);
	//$sid = db_result(db_query("SELECT sid FROM {solution} WHERE pid=%d AND uid=%d AND in_date=%d ORDER BY sid DESC LIMIT 1",$form_state['values']['pid'],$user->uid,$time));
	$sid = db_result(db_query("SELECT LAST_INSERT_ID()"));
	
	$account = user_load($user->uid);
	user_save($account,array('profile_submissions'=>$account->profile_submissions+1));
	
	db_query("UPDATE {node} SET changed=%d WHERE nid=%d",time(),$form_state['values']['nid']);
	//$cid = "content:".$form_state['values']['nid'];
	//cache_clear_all($cid,'cache_content',TRUE);
	
	oj_judge($sid);
	
	drupal_set_message("Source code submitted sucessfully.");
	drupal_goto("contest/$cid/status");
	
}
