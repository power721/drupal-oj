<?php

function import_user($p1=0,$p2=100,$redirect=TRUE)
{
	//debug($p1,'p1');debug($p2,'p2');
	if($p1<0||$p2<0)
	{
		drupal_set_message("user import:Invalid Arguments");
		if($redirect)
			drupal_goto('user/list');
		return;
	}
	$start_time = time();
	$last_user = variable_get('last_import_user',0);
	
	set_time_limit(0);
	$lim_off=db_escape_string($p1);
	$lim_row=db_escape_string($p2);
	if($lim_off==NULL)
		$lim_off = 0;
	
	db_set_active('JudgeOnline');
	db_query("SET NAMES 'latin1'");
	$encode = "PWDforJO2005";
	$query="SELECT user_id,DECODE(password,'$encode') AS pass FROM users u ORDER BY reg_time LIMIT $lim_off,$lim_row";
	//debug($query);
	//$query="SELECT *,UNIX_TIMESTAMP(u.reg_time) AS reg_time FROM users u LEFT JOIN privilege p ON u.user_id=p.user_id ORDER BY reg_time LIMIT $lim_off,$lim_row";
	$result=db_query($query,$p1,$p2);
	//debug($result);
	db_set_active('default');
	$num = 0;
	while($user = db_fetch_object($result))
	{
		//debug($problem,'problem id:'.$problem->problem_id);
		$last_user++;
		$items=db_query("SELECT uid FROM {users} WHERE name='%s'",$user->user_id);
		if($item = db_fetch_object($items))
		{
			if($item->uid!=NULL)
				continue;
		}
		$name = $user->user_id;
		$pass = $user->pass;
		oj_user_external_register($name, $pass, 'oj_user');
		$num++;
	}
	db_set_active('default');
	
	variable_set('last_import_user',$last_user);
	if(!$redirect)
		return;
	
	$msg = "You had import $num user";
	if($num>1)
		$msg.="s";
	$msg.=", execution time: ".(time()-$start_time)."s.";
	drupal_set_message($msg);
	drupal_set_message('Then you should import problems data.');
	drupal_goto('user/list');
}

function reset_user()
{
	db_query("DELETE FROM {users} WHERE uid>1");
	db_query("ALTER TABLE {users} AUTO_INCREMENT=2");
	
	db_query("DELETE FROM {users_roles} WHERE uid>1");
	
	db_query("DELETE FROM {profile_values} WHERE uid>1");
	
	db_query("DELETE FROM {authmap}");
	db_query("ALTER TABLE {authmap} AUTO_INCREMENT=1");
	
	db_query("DELETE FROM {url_alias} WHERE src LIKE 'user%%'");
	
	variable_set('last_import_user',0);
	
	drupal_set_message("You had delete all users.");
	drupal_goto('user/list');
}

function oj_user_external_register($name, $pass, $module) 
{
	global $user;

	$existing_user = user_load(array('name' => $name));
	if (isset($existing_user->uid)) 
	{
		return;
	}
	else 
	{
		// Register this new user.
		$userinfo = array
		(
			'name' => $name, 
			'pass' => $pass, 
			'init' => $name, 
			'status' => 1, 
			"authname_$module" => $name, 
			'access' => time(),
		);
		$account = user_save('', $userinfo);
		// Terminate if an error occured during user_save().
		if (!$account)
		{
			drupal_set_message(t("Error saving user account."), 'error');
			return;
		}
		watchdog('user', 'New external user: %name using module %module.', array('%name' => $name, '%module' => $module), WATCHDOG_NOTICE, l(t('edit'), 'user/' . $user->uid . '/edit'));
  }
}
