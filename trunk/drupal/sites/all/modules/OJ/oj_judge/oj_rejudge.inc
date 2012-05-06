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

function oj_rejudge($type = 'form',$id = NULL)
{
	global $php_path,$judge_php;
	$para = "$php_path $judge_php rejudge ";
	drupal_add_js('misc/collapse.js');
	drupal_add_js(drupal_get_path('module','oj_judge').'/oj_judge.js');
	switch($type)
	{
		case 'form':
			drupal_set_title('Rejudge Settings');
			return drupal_get_form('oj_rejudge_form');
		case 'problem':return drupal_get_form('oj_rejudge_form',1,$id);
		case 'sid':
				$tid = start_proc($para."sid $id");
				drupal_set_message("The rejudge has started,waiting and refresh to view the result.");
				drupal_goto("code/$id");
				break;
	}
}

function oj_rejudge_range($sid1,$sid2)
{
	for($sid = $sid1;$sid <= $sid2;$sid++)
		oj_rejudge_one($sid,FALSE);
}

function oj_rejudge_problem_callback($pid,$results = array())
{
	$time = time();
	//debug($pid,'$pid');
	//debug($results,'$results');
	if(empty($results))
	{
		$nid = db_result(db_query("SELECT nid FROM {content_type_problem} WHERE field_pid_value = %d",$pid));
		$node = node_load($nid,NULL,TRUE);
		$node->field_accepted[0]['value'] = 0;
		$node->field_solved[0]['value'] = 0;
		$node->field_error[0]['value'] = 0;
		node_save($node);
		
		$items = db_query("SELECT sid FROM {solution} WHERE pid = %d",$pid);
		while($item = db_fetch_object($items))
		{
			oj_rejudge_one($item->sid,FALSE,FALSE);
		}
	}
	else
	{
		$result = array_shift($results);
		$sql = "SELECT sid FROM {solution} WHERE pid = %d AND (result = $result";
		if(!empty($results))
			foreach($results as $result)
			{
				$sql .= " OR result = $result";
			}
		$sql .= ")";
		$items = db_query($sql,$pid);
		//debug($sql,'$sql');
		//debug($items,'$items');
		while($item = db_fetch_object($items))
		{
			//debug($item->sid);
			oj_rejudge_one($item->sid,TRUE,FALSE);
		}
	}
	//$time = time()-$time;
	//debug($time,'time');
}

function oj_rejudge_problem($pid,$results = array(),$redirect = TRUE)
{
	global $php_path,$judge_php;
	$para = "$php_path $judge_php rejudge pid $pid";
	if(is_array($results) && !empty($results))
		foreach($results as $result)
			$para .= " $result";
	$tid = start_proc($para);
	//debug($tid,'$tid');
	if($redirect)
	{
		drupal_set_message("The rejudge has started,waiting and refresh to view the result.");
		drupal_goto("problem/$pid");
	}
}

function oj_rejudge_contest($cid,$pid = 0,$result = array())
{
	
}

function oj_rejudge_one($sid,$reset = TRUE,$redirect = TRUE)
{
	$items = db_query("SELECT uid,pid,result FROM {solution} WHERE sid=%d",$sid);
	if(!($item = db_fetch_object($items)))
	{
		drupal_set_message("can't find this solution!");
		return;
	}
	$result = db_fetch_object(db_query("SELECT sid FROM {solution} WHERE uid=%d AND pid=%d AND sid<%d LIMIT 1",$item->uid,$item->pid,$sid));
	if($reset)//清除problem相关数据信息
	{
		$nid = db_result(db_query("SELECT nid FROM {solution} s LEFT JOIN {content_type_problem} p ON p.field_pid_value=s.pid WHERE sid=%d",$sid));
		$node = node_load($nid,NULL,TRUE);
		if($item->result == 0)
		{
			//problem的ac数量更新
			--$node->field_accepted[0]['value'];
			//problem的solved数量更新
			if(!$result->sid)
				--$node->field_solved[0]['value'];
		}
		else if(!$result->sid)
		{
			--$node->field_error[0]['value'];
		}
		node_save($node);
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
	//oj_judge($sid);//不能再次调用php进程
	$judge_list = new Judge_thread($sid);
	delete_last($sid);
	if($redirect)
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
	if($user->roles['5']!=NULL)
		return true;
}

function oj_rejudge_form(&$form_state,$type = NULL, $id = NULL)
{
	if($type)
	{
		$form['type'] = array
		(
			'#type' => 'value',
			'#value' => $type
		);
		$form['pid'] = array
		(
			'#type' => 'value',
			'#value' => $id
		);
	}
	else
	{
		$form['type'] = array
		(
			'#type' => 'radios',
			'#title' => 'type',
			'#options' => array(t('Solution'),t('Problem'),t('Contest')),
			//'#description' => t('select rejudge type.'),
			'#default_value' => 0,
		);
		//$type = (int)$form_state['post']['type'];
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
			'#default_value' => $id
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
	}
	
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
	$value = $form_state['values']['pid'];
	if(!empty($value))
		$pid[$value] = $value;
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
	$value = $form_state['values']['pid'];
	if(!empty($value))
		$pids[$value] = $value;
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
			oj_rejudge_problem($pid,$result);
			//oj_rejudge('pid',$pid,0,$result);
		}
	if($form_state['values']['type'] == 2)
		foreach($cids as $cid)
		{
			oj_rejudge_contest($cid);
		}
	drupal_set_message("You rejudge request will start.");
	//drupal_goto(url('status'));
}
