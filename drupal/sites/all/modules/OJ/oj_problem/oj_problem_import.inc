<?php

function import_problem($p1=1000,$p2=1099)
{
	//debug($p1,'p1');debug($p2,'p2');
	if($p1<0||$p2<0||$p1>$p2)
	{
		drupal_set_message("Invalid Arguments");
		drupal_goto('problem/list');
	}
	$start_time = time();
	if($p1==null)
	{
		$p1=1000;
		$p2=1099;
	}
	if($p2==null)
		$p2=1099;
	//$lim_off=db_escape_string($p1);
	//$lim_row=db_escape_string($p2);
	//if($lim_off==null)
	//	$lim_off = 0;
	//debug($lim_off,'$lim_off');
	//debug($lim_row,'$lim_row');
	module_load_include('inc', 'node', 'node.pages');
	
	db_set_active('JudgeOnline');
	db_query("SET NAMES 'latin1'");
	$query="SELECT *,UNIX_TIMESTAMP(in_date) AS in_date FROM problem WHERE problem_id>=%d AND problem_id<=%d ORDER BY problem_id";
	$result=db_query($query,$p1,$p2);
	//debug($result);
	db_set_active('default');
	$num = 0;
	while($problem = db_fetch_object($result))
	{
		//debug($problem,'problem id:'.$problem->problem_id);
		$items=db_query("SELECT nid FROM {content_type_problem} WHERE field_problem_id_value=%d",$problem->problem_id);
		if($item = db_fetch_object($items))
		{
			if($item->nid!=null)
				continue;
		}
		$node = new stdClass;
		$node->type='problem';
		node_object_prepare($node);
		$node->uid=1;
		$node->language='';
		$node->revision=0;
		$node->status=$problem->defunct=='N'?1:0;
		$node->title=phpcharset($problem->title,'UTF-8');
		$node->changed=$problem->in_date;
		$node->field_description['0']['value']=phpcharset($problem->description,'UTF-8');
		$node->field_input['0']['value']=phpcharset($problem->input,'UTF-8');
		$node->field_output['0']['value']=phpcharset($problem->output,'UTF-8');
		$node->field_input_path['0']['value']=$problem->input_path;
		$node->field_output_path['0']['value']=$problem->output_path;
		$node->field_sinput['0']['value']=phpcharset($problem->sample_input,'UTF-8');
		$node->field_soutput['0']['value']=phpcharset($problem->sample_output,'UTF-8');
		$node->field_hint['0']['value']=phpcharset($problem->hint,'UTF-8');
		$node->field_source['0']['value']=phpcharset($problem->source,'UTF-8');
		$node->field_time['0']['value']=$problem->time_limit;
		$node->field_memory['0']['value']=$problem->memory_limit;
		$node->field_accepted['0']['value']=$problem->accepted;
		$node->field_submit['0']['value']=$problem->submit;
		$node->field_submit_user['0']['value']=$problem->submit_user;
		$node->field_solved['0']['value']=$problem->solved;
		$node->field_ctime['0']['value']=$problem->case_time_limit;
		$node->field_problem_id['0']['value']=$problem->problem_id;
		$node->field_view['0']['value']=$problem->view;
		$node->field_error['0']['value']=$problem->error;
		$node->field_problem_id['0']['value']=$problem->problem_id;
		node_save($node);
		$num++;
	}
	
	$msg = "You had import $num problem";
	if($num>1)
		$msg.="s";
	$msg.=", execution time: ".(time()-$start_time)."s.";
	drupal_set_message($msg);
	drupal_goto('problem/list');
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
	drupal_goto('problem/list');
}
