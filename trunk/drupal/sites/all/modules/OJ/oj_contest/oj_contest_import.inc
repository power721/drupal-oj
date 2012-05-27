<?php

function oj_contest_import($c1=1000,$c2=1099,$redirect=TRUE)
{
	
	if($c1<0 || $c2<0 || $c1>$c2)
	{
		drupal_set_message("contest import:Invalid Arguments.");
		if($redirect)
			drupal_goto('contests');
		return;
	}
	$start_time = time();
	
	set_time_limit(0);
	if($c1 == NULL)
	{
		$c1 = 1000;
		$c2 = 1099;
	}
	if($c2 == NULL)
		$c2 = 1099;
	global $user;
	$last_cid = variable_get('last_import_contest',1000);
	
	$num = 0;
	db_set_active('JudgeOnline');
	db_query("SET NAMES 'latin1'");
	$items = db_query("
					SELECT 
						contest_id,
						title,
						description,
						private,
						defunct,
						UNIX_TIMESTAMP(start_time) AS start_time,
						UNIX_TIMESTAMP(end_time) AS end_time
					FROM
						contest
					WHERE
						contest_id>=%d
						AND contest_id<=%d
					 ORDER BY contest_id
					",$c1,$c2);
	
	while($item = db_fetch_object($items))
	{
		$last_cid = max($last_cid,$item->contest_id);
		$contest = new stdClass;
		$contest->uid = $user->uid;
		$contest->title = problem_text($item->title);
		$contest->description = problem_text($item->description);
		$contest->start_time = $item->start_time;
		$contest->end_time = $item->end_time;
		$contest->access = $item->private?2:0;
		$contest->onsite = 0;
		$contest->defunct = $item->defunct!='N';
		$contest->contest_id = $item->contest_id;
		
		/***** problems *****/
		db_set_active('JudgeOnline');
		$problems = db_query("SELECT c.problem_id AS pid,num+1 AS num,c.title AS alias,p.title FROM contest_problem c LEFT JOIN problem p ON c.problem_id=p.problem_id WHERE c.contest_id=%d ORDER BY num",$item->contest_id);
		db_set_active('default');
		while($problem = db_fetch_array($problems))
		{
			$problem['pid'] = db_result(db_query("SELECT field_pid_value FROM {content_type_problem} WHERE field_problem_id_value=%d",$problem['pid']));
			$problem['uid'] = $user->uid;
			$problem['alias'] = empty($problem['alias'])?$problem['title']:problem_text($problem['alias']);
			$contest->problems[] = $problem;
		}
		db_set_active('JudgeOnline');
		
		/***** user access *****/
		$users = db_query("SELECT user_id FROM private WHERE contest_id=%d",$item->contest_id);
		db_set_active('default');
		while($name = db_result($users))
		{
			$contest->users[get_uid_by_name($name)] = 1;
		}
		
		$contest->cid = db_result(db_query("SELECT cid FROM {contests} WHERE title='%s' AND start_time=%d",$contest->title,$contest->start_time));
		contest_save($contest,TRUE,TRUE);
		
		//oj_contest_rank_table($contest->cid,FALSE);
	}
	db_set_active('default');
	
	variable_set('last_import_contest',$last_cid);
	if(!$redirect)
		return;
		
	$time = time()-$start_time;
	drupal_set_message("The contests have been updated.\ntime: $time");
	drupal_set_message('Then you should import solutions data.');
	drupal_goto('contests');
}

function reset_contest()
{
	db_query("DELETE FROM {contests}");
	db_query("ALTER TABLE {contests} AUTO_INCREMENT=1000");
	
	db_query("DELETE FROM {contest_access}");
	db_query("ALTER TABLE {contest_access} AUTO_INCREMENT=1");
	
	db_query("DELETE FROM {contest_problem}");
	db_query("ALTER TABLE {contest_problem} AUTO_INCREMENT=1");
	
	db_query("DELETE FROM {contest_rank}");
	db_query("ALTER TABLE {contest_rank} AUTO_INCREMENT=1");
	
	variable_set('last_import_contest',1000);
	
	drupal_set_message("You had delete all contests.");
	drupal_goto('contests');
}
