<?php

function oj_user_cmp($uid1,$uid2)
{
	if(empty($uid1) || empty($uid2))
	{
		$user1 = $_GET['uid1'];
		$user2 = $_GET['uid2'];
		if(empty($user1) || empty($user2))
		{
			return "Invalid parameter.";
		}
		$uid1 = db_result(db_query("SELECT uid FROM {users} WHERE name='%s'",$user1));
		$uid2 = db_result(db_query("SELECT uid FROM {users} WHERE name='%s'",$user2));
	}
	return drupal_get_form('oj_user_cmp_form',$uid1,$uid2);
}

function oj_user_cmp_form($form_state,$uid1,$uid2)
{
	$user1 = user_load($uid1);
	$user2 = user_load($uid2);
	$result = get_user_cmp_result($uid1,$uid2);
	//drupal_set_title($user1->name.' VS '.$user2->name);
	
	$form['header'] = array
	(
		'#value' => "<table><tr><td colspan=3 align=center><a href={$uid1}>{$user1->name}</a> VS <a href={$uid2}>{$user2->name}</a></td></tr><tr><td>",
	);
	
	$form['uid1'] = array
	(
		'#type' => 'textfield',
		'#title' => 'user1',
		'#default_value' => $user1->name,
		'#size' => 20,
		'#autocomplete_path' => 'user/autocomplete',
	);
	
	$form['uid2'] = array
	(
		'#type' => 'textfield',
		'#title' => 'user2',
		'#default_value' => $user2->name,
		'#size' => 20,
		'#autocomplete_path' => 'user/autocomplete',
	);
	
	$form['submit'] = array
	(
		'#type' => 'submit',
		'#value' => 'compare',
	);
	$form['footer'] = array
	(
		'#value' => '</td></tr></table>',
	);
	
	$form['user1-ac'] = array
	(
		'#type' => 'fieldset',
		'#title' => 'Problems only '.$user1->name.' accepted',
		'#collapsible' => TRUE,
	);
	$form['user1-ac']['pid'] = array
	(
		'#type' => 'item',
		'#value' => $result[0],
	);
	
	$form['user2-ac'] = array
	(
		'#type' => 'fieldset',
		'#title' => 'Problems only '.$user2->name.' accepted',
		'#collapsible' => TRUE,
	);
	$form['user2-ac']['pid'] = array
	(
		'#type' => 'item',
		'#value' => $result[1],
	);
	
	$form['both-ac'] = array
	(
		'#type' => 'fieldset',
		'#title' => 'Problems both '.$user1->name.' and '.$user2->name.' accepted',
		'#collapsible' => TRUE,
	);
	$form['both-ac']['pid'] = array
	(
		'#type' => 'item',
		'#value' => $result[2],
	);
	
	$form['user1-wa'] = array
	(
		'#type' => 'fieldset',
		'#title' => 'Problems only '.$user1->name.' tried but failed',
		'#collapsible' => TRUE,
	);
	$form['user1-wa']['pid'] = array
	(
		'#type' => 'item',
		'#value' => $result[3],
	);
	
	$form['user2-wa'] = array
	(
		'#type' => 'fieldset',
		'#title' => 'Problems only '.$user2->name.' tried but failed',
		'#collapsible' => TRUE,
	);
	$form['user2-wa']['pid'] = array
	(
		'#type' => 'item',
		'#value' => $result[4],
	);
	
	$form['both-wa'] = array
	(
		'#type' => 'fieldset',
		'#title' => 'Problems both '.$user1->name.' and '.$user2->name.' tried but failed',
		'#collapsible' => TRUE,
	);
	$form['both-wa']['pid'] = array
	(
		'#type' => 'item',
		'#value' => $result[5],
	);
	
	return $form;
}

function oj_user_cmp_form_validate($form,$form_state)
{
	$user1 = $form_state['values']['uid1'];
	$user2 = $form_state['values']['uid2'];
	$uid1 = db_result(db_query("SELECT uid FROM {users} WHERE name='%s'",$user1));
	if(!$uid1)
		form_set_error('uid1','user1 is not exist.');
	
	$uid2 = db_result(db_query("SELECT uid FROM {users} WHERE name='%s'",$user2));
	if(!$uid2)
		form_set_error('uid2','user2 is not exist.');
}

function oj_user_cmp_form_submit($form,$form_state)
{
	drupal_goto(url('user/cmp',array('query'=>'uid1='.$form_state['values']['uid1'].'&uid2='.$form_state['values']['uid2'],'absolute' => TRUE)));
}

function get_user_cmp_result($uid1,$uid2)
{
	$user1_ac = array();
	$user2_ac = array();
	$user1_wa = array();
	$user2_wa = array();
	
	$items = db_query("SELECT pid,MIN(result) AS res FROM {solution} WHERE uid=%d GROUP BY pid",$uid1);
	while($item = db_fetch_object($items))
	{
		if($item->res)
			$user1_wa[] = $item->pid;
		else
			$user1_ac[] = $item->pid;
	}
	
	$items = db_query("SELECT pid,MIN(result) AS res FROM {solution} WHERE uid=%d GROUP BY pid",$uid2);
	while($item = db_fetch_object($items))
	{
		if($item->res)
			$user2_wa[] = $item->pid;
		else
			$user2_ac[] = $item->pid;
	}
	
	$user1_ac_only = array_diff($user1_ac,$user2_ac);
	$user1_wa_only = array_diff($user1_wa,$user2_wa);
	
	$user2_ac_only = array_diff($user2_ac,$user1_ac);
	$user2_wa_only = array_diff($user2_wa,$user1_wa);
	
	$both_ac = array_intersect($user1_ac,$user2_ac);
	$both_wa = array_intersect($user1_wa,$user2_wa);
	
	foreach(array('user1_ac_only','user1_wa_only','user2_ac_only','user2_wa_only','both_ac','both_wa') as $value)
	{
		$i = 0;
		foreach(${$value} as $pid)
		{
			${$value.'_str'} .= "<a class=user-cmp href=../problem/$pid>$pid</a> ";
			$i++;
			if($i == 10)
			{
				${$value.'_str'} .= "<br>\n";
				$i = 0;
			}
		}
	}
	return array($user1_ac_only_str,$user2_ac_only_str,$both_ac_str,$user1_wa_only_str,$user2_wa_only_str,$both_wa_str);
}
