<?php

/*
* rejudge a single solution,a problem with  specified status,a contest
* $sid an array of solutions
*/

function oj_rejudge_callback($type,$id1,$id2 = 0,$results = array())
{
//debug($type,'type');
//debug($id1,'id1');
//debug($results,'$results');
	if(empty($type))
	{
		drupal_set_title('Rejudge Settings');
		drupal_add_js('misc/collapse.js');
		drupal_add_js(drupal_get_path('module','oj_judge').'/oj_judge.js');
		return drupal_get_form('oj_rejudge_form');
	}
	switch($type)
	{
		case 'sid':
			if(empty($id2))
				oj_rejudge_one($id1);
			else
				oj_rejudge_range($id1,$id2);
			break;
		case 'pid':oj_rejudge_problem($id1,$results);
			break;
		case 'cid':oj_rejudge_contest($id1,$id2);
			break;
		default:
			drupal_set_message("There are some errors.");
	};
}

function oj_rejudge($type,$id1,$id2 = 0,$results = array())
{
	if(empty($type))
	{
		drupal_set_title('Rejudge Settings');
		drupal_add_js('misc/collapse.js');
		drupal_add_js(drupal_get_path('module','oj_judge').'/oj_judge.js');
		return drupal_get_form('oj_rejudge_form');
	}
	global $php_path,$judge_php;
	
	switch($type)
	{
		case 'sid':
			/*if(empty($id2))
				oj_rejudge_one($id1);
			else
				oj_rejudge_range($id1,$id2);*/
			
			break;
		case 'pid'://oj_rejudge_problem($id1);
			break;
		case 'cid'://oj_rejudge_contest($id1,$id2);
			break;
		default:
			drupal_set_message("There are some errors.");
			drupal_goto('');
	};
	
	$para = "$php_path $judge_php rejudge $type $id1 $id2";
	if(is_array($results) && !empty($results))
		foreach($results as $result)
			$para .= " $result";
	$pid = start_proc($para);
	drupal_set_message("Your request has been accepted,waiting for rejudge complete.");
	drupal_goto('status');
}

function oj_rejudge_range($sid1,$sid2)
{
	for($sid = $sid1;$sid <= $sid2;$sid++)
		oj_rejudge_one($sid,FALSE);
}

function oj_rejudge_problem_callback($pid,$results = array())
{
	if(empty($results))
	{
		$nid = db_result(db_query("SELECT nid FROM {content_type_problem} WHERE field_pid_value = %d",$pid));
		$node = node_load($nid);
		$node->field_accepted[0]['value'] = 0;
		$node->field_solved[0]['value'] = 0;
		node_save($node);
		$items = db_query("SELECT sid FROM {solution} WHERE pid = %d",$pid);
		while($item = db_fetch_object($items))
		{
			oj_rejudge_one($item->sid,FALSE);
		}
	}
	else
	{
		$sql = "SELECT sid FROM {solution} WHERE pid = %d AND (result = %d";
		$para = array($pid,array_shift($results));
		if(!empty($results))
			foreach($results as $result)
			{
				$para[] = $result;
				$sql .= " OR result = %d";
			}
		$sql .= ")";
		$items = db_query($sql,$para);
		while($item = db_fetch_object($items))
		{
			oj_rejudge_one($item->sid);
		}
	}
}

function oj_rejudge_problem($pid,$results = array(),$redirect = TRUE)
{
	global $php_path,$judge_php;
	$para = "$php_path $judge_php rejudge pid $pid";
	if(is_array($results) && !empty($results))
		foreach($results as $result)
			$para .= " $result";
	start_proc($para);
	
	if($redirect)
	{
		drupal_set_message("The rejudge has started,waiting and refresh to view the result.");
		drupal_goto("problem/$pid");
	}
}

function oj_rejudge_contest($cid,$pid = 0,$result = array())
{
	
}

function oj_rejudge_one($sid,$type = TRUE)
{
	$items = db_query("SELECT uid,pid,result FROM {solution} WHERE sid=%d",$sid);
	if(!($item = db_fetch_object($items)))
	{
		drupal_set_message("can't find this solution!");
		return;
	}
	$result = db_fetch_object(db_query("SELECT sid FROM {solution} WHERE uid=%d AND pid=%d AND sid!=%d LIMIT 1",$item->uid,$item->pid,$sid));
	if($type)//清除problem相关数据信息
	{
		if($item->result == 0)
		{
			//problem的ac数量更新
			db_query("UPDATE {content_type_problem} SET field_accepted_value=field_accepted_value-1 WHERE field_pid_value=%d",$item->pid);
			//problem的solved数量更新
			if(!$reault->sid)
				db_query("UPDATE {content_type_problem} SET field_solved_value=field_solved_value-1 WHERE field_pid_value=%d",$item->pid);
		}
		else if(!$reault->sid)
		{
			db_query("UPDATE {content_type_problem} SET field_error_value=field_error_value-1 WHERE field_pid_value=%d",$item->pid);
		}
	}
	//清除solution的judge状态
	db_query("UPDATE {solution} SET time=NULL,memory=NULL,result=10000,error=NULL WHERE sid=%d",$sid);
	if($item->result == 0)
	{
	//user的solved数量更新
		$res = db_fetch_object(db_query("SELECT CAST(value AS SIGNED) AS solved FROM {profile_values} WHERE fid=4 AND uid=%d",$item->uid));
		$solved = $res->solved-1;
		if(!empty($solved))
			db_query("UPDATE {profile_values} SET value='%s' WHERE fid=4 AND uid=%d",$solved,$item->uid);
	}
	//oj_judge($sid);
	$judge_list = new Judge_thread($sid);
	if($type)
	{
		drupal_set_message("The rejudge has started,waiting and refresh to view the result.");
		drupal_goto("code/$sid");
	}
}

function oj_rejudge_access()
{
	global $user;
	if($user->uid==0)
		return false;
	if($user->uid==1)
		return true;
	if($user->roles['5']!=null)
		return true;
}

function oj_rejudge_form(&$form_state)
{
	//debug($form_state,'$form_state');
	$form['type'] = array
	(
		'#type' => 'radios',
		'#title' => 'type',
		'#options' => array(t('Solution'),t('Problem'),t('Contest')),
		//'#description' => t('select rejudge type.'),
		'#default_value' => 0,
	);
	$type = (int)$form_state['post']['type'];
	$form['sid'] = array
	(
		'#type' => 'fieldset',
		'#title' => t('Solution ID'),
		'#attributes' => array('class' => 'fieldgroup field-sid collapsible'),
		'#description' => t('The Solution to rejudge.'),
	);
	$form['sid']['sid-0'] = array
	(
		'#type' => 'textfield',
		'#size' => 20,		
	);
	$form['sid']['add'] = array
	(
		'#value' => '<a class=add_sid href=javascript:void(0)>Add Solution</a>',
	);
	
	$form['pid'] = array
	(
		'#type' => 'fieldset',
		'#title' => t('Problem ID'),
		'#attributes' => array('class' => 'fieldgroup field-pid collapsible'),
		'#description' => t('The problem to rejudge.'),
	);
	$form['pid']['pid-0'] = array
	(
		'#type' => 'textfield',
		'#size' => '20',
	);
	$form['pid']['add'] = array
	(
		'#value' => '<a class=add_pid href=javascript:void(0)>Add Problem</a>',
	);
	$form['cid'] = array
	(
		'#type' => 'textfield',
		'#title' => t('Contest ID'),
		'#description' => t('Just rejudge contest solution.'),
		'#attributes' => array('class' => 'field-cid'),
	);
	
	$status_result[0] = 'All';
	$results = oj_get_status_result(TRUE);
	foreach($results as $key => $value)
	{
		$status_result[$key+1] = $value;
	}
	$form['result'] = array
	(
		'#type' => 'checkboxes',
		'#title' => t('Status result'),
		'#description' => t('The status result to rejudge.'),
		'#options' => $status_result,
		'#default_value' => array(10001),
		'#attributes' => array('class' => 'field-result'),
	);
	$form['submit'] = array
	(
		'#type' => 'submit',
		'#value' => t('Submit'),
		//'#executes_submit_callback' => TRUE,
		//'#submit' => array('oj_rejudge_form_submit')
	);
	return $form;
}

function oj_rejudge_form_validate(&$form, &$form_state)
{
	//debug($form);
	//debug($form_state);
	foreach($form_state['clicked_button']['#post'] as $key => $value)
	{
		$form_state['values'][$key] = $value;
		if(empty($value))
			continue;
		if(preg_match('/pid-(\d+)/',$key,$matched))
		{
			$pid[$value] = $value;
			/*$form['pid'][$key] = $form['pid']['pid-0'];
			$form['pid'][$key]['#value'] = $value;
			$form['pid'][$key]['#name'] = $key;
			$form['pid'][$key]['#parents'] = $key;
			$form['pid'][$key]['#array_parents'][1] = $key;
			$form['pid'][$key]['#id'] = "edit-$key";*/
		}
		if(preg_match('/sid-(\d+)/',$key,$matched))
		{
			$sid[$value] = $value;
			/*$form['sid'][$key] = $form['sid']['sid-0'];
			$form['sid'][$key]['#value'] = $value;
			$form['sid'][$key]['#name'] = $key;
			$form['sid'][$key]['#parents'] = $key;
			$form['sid'][$key]['#array_parents'][1] = $key;
			$form['sid'][$key]['#id'] = "edit-$key";*/
		}
		if(preg_match('/cid-(\d+)/',$key,$matched))
		{
			$cid[$value] = $value;
		}
	}
	$form_id = $form_state['values']['form_id'];
	//$args = $form['#parameters'];
	//$form_id = array_shift($args);
	//drupal_retrieve_form($form_id,$form,$form_state);
	//form_set_cache($form_state['values']['form_build_id'],$form,$form_state);
	//debug($form);
	//debug($form_state);
	if($form_state['values']['type'] == 0 && empty($sid))
	{
		form_error($form, t('You must select some solutions to rejudge.'));
		return $form;
	}
	if($form_state['values']['type'] == 1 && empty($pid))
	{
		form_error($form, t('You must select some problems to rejudge.'));
		return $form;
	}
	if($form_state['values']['type'] == 2 && empty($cid))
	{
		form_error($form, t('You must select a contest to rejudge.'));
		return $form;
	}
	foreach($form_state['values']['result'] as $key => $value)
	{
		if(empty($value))
			continue;
		$result[$key-1] = $key-1;
	}
	if($form_state['values']['type'] && empty($result))
	{
		form_error($form, t('You must select some status results to rejudge.'));
		return $form;
	}
	//debug($result);
	return $form;
}

function oj_rejudge_form_submit($form, &$form_state)
{
	foreach($form_state['clicked_button']['#post'] as $key => $value)
	{
		if(empty($value))
			continue;
		if($form_state['values']['type'] == 0 && preg_match('/sid-(\d+)/',$key,$matched))
		{
			$sids[$value] = $value;
		}
		if($form_state['values']['type'] == 1 && preg_match('/pid-(\d+)/',$key,$matched))
		{
			$pids[$value] = $value;
		}
		if($form_state['values']['type'] == 2 && preg_match('/cid-(\d+)/',$key,$matched))
		{
			$cids[$value] = $value;
		}
	}
	//debug($pids,'pids');
	//debug($sids,'sids');
	//debug($cids,'cids');
	
	$result = array();
	if($form_state['values']['type'] != 0)
		foreach($form_state['values']['result'] as $key => $value)
		{
			if(empty($value))
				continue;
			$result[$key-1] = $key-1;
		}
	//debug($result,'result');
	if($form_state['values']['type'] == 0)
		foreach($sids as $sid)
		{
			oj_rejudge_one($sid,FALSE);
		}
	if($form_state['values']['type'] == 1)
		foreach($pids as $pid)
		{
			//oj_rejudge_problem($pid,$result);
			oj_rejudge('pid',$pid,0,$result);
		}
	if($form_state['values']['type'] == 2)
		foreach($cids as $cid)
		{
			oj_rejudge_contest($cid);
		}
	
	//drupal_goto(url('status'));
}
