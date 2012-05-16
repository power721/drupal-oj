<?php

function import_problem($p1 = 1000,$p2 = 1099)
{
	//debug($p1,'p1');debug($p2,'p2');
	if($p1<0 || $p2<0 || $p1>$p2)
	{
		drupal_set_message("Invalid Arguments");
		drupal_goto('problem/list');
	}
	$start_time = time();
	
	set_time_limit(0);
	if($p1 == NULL)
	{
		$p1 = 1000;
		$p2 = 1099;
	}
	if($p2 == NULL)
		$p2 = 1099;
	//$lim_off=db_escape_string($p1);
	//$lim_row=db_escape_string($p2);
	//if($lim_off==NULL)
	//	$lim_off = 0;
	//debug($lim_off,'$lim_off');
	//debug($lim_row,'$lim_row');
	module_load_include('inc', 'node', 'node.pages');
	
	db_set_active('JudgeOnline');
	db_query("SET NAMES 'latin1'");
	$query = "SELECT *,UNIX_TIMESTAMP(in_date) AS in_date FROM problem WHERE problem_id>=%d AND problem_id<=%d ORDER BY problem_id";
	$result = db_query($query,$p1,$p2);
	//debug($result);
	db_set_active('default');
	$num = 0;
	while($problem = db_fetch_object($result))
	{
		$node = new stdClass;
		//node_object_prepare($node);
		
		$items = db_query("SELECT nid,vid FROM {content_type_problem} WHERE field_problem_id_value=%d",$problem->problem_id);
		if($item = db_fetch_object($items))
		{
			//if($item->nid != NULL)
			//	continue;
			$node->nid = $item->nid;
			$node->vid = $item->vid;
		}
		
		$node->type = 'problem';
		$node->uid = 1;
		$node->status = $problem->defunct=='N'?1:0;
		$node->title = phpcharset($problem->title,'UTF-8');
		$node->changed = $problem->in_date;
		$fields = array
		(
			'description' 	=> 'description',
			'input' 		=> 'input',
			'output' 		=> 'output',
			'input_path' 	=> 'input_path',
			'output_path'	=> 'output_path',
			'sinput'		=> 'sample_input',
			'soutput' 		=> 'sample_output',
			'hint' 			=> 'hint',
			'source'		=> 'source',
			'time' 			=> 'time_limit',
			'memory' 		=> 'memory_limit',
			'accepted'		=> 'accepted',
			'submit' 		=> 'submit',
			'submit_user' 	=> 'submit_user',
			'solved' 		=> 'solved',
			'ctime' 		=> 'case_time_limit',
			'problem_id' 	=> 'problem_id',
			'view' 			=> 'view',
			'error' 		=> 'error',
		);
		foreach($fields as $key => $value)
		{
			$node->{"field_$key"}['0']['value'] = problem_text($problem->$value);
		}
		node_save($node);
		$num++;
	}
	
	$msg = "You had import $num problem";
	if($num>1)
		$msg.="s";
	$msg.=", execution time: ".(time()-$start_time)."s.";
	drupal_set_message($msg);
	drupal_set_message('Then you should import contests data.');
	drupal_goto('problem');
	//drupal_goto('admin/content/node/overview');
}

function reset_problem_access()
{
	global $user;
	if($user->uid==1)
		return true;
	return false;
}

function reset_problem()
{
	$query="DELETE FROM {node} WHERE 1";
	db_query($query);
	$query="ALTER TABLE {node} AUTO_INCREMENT=1";
	db_query($query);
	
	$query="DELETE FROM {node_revisions} WHERE 1";
	db_query($query);
	$query="ALTER TABLE {node_revisions} AUTO_INCREMENT=1";
	db_query($query);
	
	$query="DELETE FROM {content_type_problem} WHERE 1";
	db_query($query);
	
	//db_query("DELETE FROM {url_alias} WHERE dst LIKE '%s%%'", 'problem');
	
	drupal_set_message("You had delete all problems.");
	drupal_goto('problem');
}

function problem_text($text)
{
	$text = phpcharset($text,'UTF-8');
	$text = str_replace(array("\r\n", "\r"), "\n", $text);
	$text = str_replace("\n", "<br>", $text);
	//preg_replace("!<img src=\"(.*)\"/>!","<img src=\"/oj/\1\" />",$text);
	//preg_replace("!<img src='(.*)'/>!","<img src='/oj/\1' />",$text);
	//preg_replace("!<img src=(.*)/>!","<img src=/oj/\1 />",$text);
	if(preg_match("!src=(.*)/>!",$text,$m))
	{
		$r = str_replace("\\","/",$m[1]);
		$text = str_replace($m[1],$r,$text);
	}
	$text = str_replace("src=images", "src=/oj/images", $text);
	$text = str_replace("src=\"images", "src=\"/oj/images", $text);
	$text = str_replace("src='images", "src='/oj/images", $text);
	
	$text = str_replace("src=problem-images", "src=/oj/problem-images", $text);
	$text = str_replace("src=\"problem-images", "src=\"/oj/problem-images", $text);
	$text = str_replace("src='problem-images", "src='/oj/problem-images", $text);
	return $text;
}

