<?php

function oj_contest_edit_form($form_state,$cid)
{
	drupal_set_title('Contest Edit');
	drupal_add_js(drupal_get_path('module','oj_contest').'/oj_contest.js');
	drupal_add_css(drupal_get_path('module','oj').'/misc/jquery-ui/css/cupertino/jquery-ui-1.7.3.custom.css');

	date_default_timezone_set("Asia/Chongqing");
	
	$contest = contest_load($cid);
	$account = user_load($contest->uid);
	$form['contest'] = array
	(
		'#type' => 'item',
		'#value' => "Contest -- $cid",
	);
	$form['uid'] = array
	(
		'#type' => 'value',
		'#value' => $contest->uid,
	);
	$form['cid'] = array
	(
		'#type' => 'value',
		'#value' => $contest->cid,
	);
	$form['creator'] = array
	(
		'#type' => 'item',
		'#title' => t('Creator'),
		'#value' => "<a href=".url('user/'.$contest->uid,array('absolute' => TRUE)).">".$account->name."</a>",
	);
	
	$form['title'] = array
	(
		'#title' => t('Title'),
		'#type' => 'textfield',
		'#default_value' => $contest->title,
	);
	$form['access'] = array
	(
		'#title' => t('Access'),
		'#type' => 'radios',
		'#default_value' => $contest->access,
		'#options' => array('Public','Register','Authorized','Password'),
		'#attributes' => array('class'=>'contest_access'),
	);
	$form['password'] = array
	(
		'#title' => t('Password'),
		'#type' => 'textfield',
		'#default_value' => $contest->password,
	);
	
	$js = <<<JS
	$(document).ready(function()
	{
		if($("#edit-access-3").attr("checked") != "checked")
			$("#edit-password-wrapper").hide();
		$(".contest_access").change(function()
		{
			if($("#edit-access-3").attr("checked") == "checked")
				$("#edit-password-wrapper").show();
			else
				$("#edit-password-wrapper").hide();
		});
		
	});
JS;
	drupal_add_js($js,'inline');
	
	$form['start_time'] = array
	(
		'#title' => t('Start Time'),
		'#type' => 'fieldset',
	);
	$form['start_time']['start_time_date'] = array
	(
		
		'#type' => 'textfield',
		'#default_value' => date('Y-m-d',$contest->start_time),
		'#attributes' => array('readonly'=>'readonly'),
		'#size' => 10,
		//'#prefix' => '<table><td>',
		//'#suffix' => '</td>',
	);
	$form['start_time']['start_time_hour'] = array
	(
		'#type' => 'textfield',
		'#default_value' => date('H',$contest->start_time),
		'#size' => 2,
		'#maxlength' => 2,
		//'#prefix' => '<td>',
		//'#suffix' => '</td>',
	);
	$form['start_time']['start_time_min'] = array
	(
		'#type' => 'textfield',
		'#default_value' => date('i',$contest->start_time),
		'#size' => 2,
		'#maxlength' => 2,
		//'#prefix' => '<td>',
		//'#suffix' => '</td>',
	);
	$form['start_time']['start_time_sec'] = array
	(
		'#type' => 'textfield',
		'#default_value' => date('s',$contest->start_time),
		'#size' => 2,
		'#maxlength' => 2,
		//'#prefix' => '<td>',
		//'#suffix' => '</td></table>',
	);
	
	$form['end_time'] = array
	(
		'#title' => t('Start Time'),
		'#type' => 'fieldset',
	);
	$form['end_time']['end_time_date'] = array
	(
		
		'#type' => 'textfield',
		'#default_value' => date('Y-m-d',$contest->end_time),
		'#attributes' => array('readonly'=>'readonly'),
		'#size' => 10,
		//'#prefix' => '<table><td>',
		//'#suffix' => '</td>',
	);
	$form['end_time']['end_time_hour'] = array
	(
		'#type' => 'textfield',
		'#default_value' => date('H',$contest->end_time),
		'#size' => 2,
		'#maxlength' => 2,
		//'#prefix' => '<td>',
		//'#suffix' => '</td>',
	);
	$form['end_time']['end_time_min'] = array
	(
		'#type' => 'textfield',
		'#default_value' => date('i',$contest->end_time),
		'#size' => 2,
		'#maxlength' => 2,
		//'#prefix' => '<td>',
		//'#suffix' => '</td>',
	);
	$form['end_time']['end_time_sec'] = array
	(
		'#type' => 'textfield',
		'#default_value' => date('s',$contest->end_time),
		'#size' => 2,
		'#maxlength' => 2,
		//'#prefix' => '<td>',
		//'#suffix' => '</td></table>',
	);
	
	$form['language'] = array
	(
		'#type' => 'checkboxes',
		'#title' => t('Language'),
		'#default_value' => oj_contest_language($cid),
		'#options' => oj_languages(),
	);
	
	$form['onsite'] = array
	(
		'#type' => 'radios',
		'#title' => t('Onsite'),
		'#default_value' => $contest->onsite,
		'#options' => array('No','Yes'),
	);
	
	$form['description'] = array
	(
		'#title' => t('Description'),
		'#type' => 'textarea',
		'#default_value' => $contest->description,
	);
	
	$form['submit'] = array
	(
		'#type' => 'submit',
		'#value' => t('Submit'),
	);
	return $form;
}

function oj_contest_edit_form_validate($form,&$form_state)
{
	
	list($year,$month,$day) = explode('-',$form_state['values']['start_time_date']);
	$start_time = mktime((int)$form_state['values']['start_time_hour'],(int)$form_state['values']['start_time_min'],(int)$form_state['values']['start_time_sec'],(int)$month,(int)$day,(int)$year);
	list($year,$month,$day) = explode('-',$form_state['values']['end_time_date']);
	$end_time = mktime((int)$form_state['values']['end_time_hour'],(int)$form_state['values']['end_time_min'],(int)$form_state['values']['end_time_sec'],(int)$month,(int)$day,(int)$year);
	
	if($end_time - $start_time < 3600)
	{
		form_set_error('start_time','The contest time is incorrect.');
	}
	$form_state['values']['start_time'] = $start_time;
	$form_state['values']['end_time'] = $end_time;
	if($form_state['values']['access'] == 3)
	{
		if(empty($form_state['values']['password']))
			form_set_error('password','The password is required.');
	}
	else
	{
		$form_state['values']['password'] = NULL;
	}
	$languages = array();
	foreach($form_state['values']['language'] as $value)
	{
		if(!empty($value))
			$languages[] = $value;
	}
	if(!count($languages))
		form_set_error('language','At least one language is required.');
	$form_state['values']['languages'] = implode(';',$languages);
}

function oj_contest_edit_form_submit($form,&$form_state)
{
	$cid = $form_state['values']['cid'];
	db_query("UPDATE {contests} SET title='%s',description='%s',
			start_time=%d,end_time=%d,access=%d,password='%s',
			language='%s',onsite=%d WHERE cid=%d",
			$form_state['values']['title'],
			$form_state['values']['description'],
			$form_state['values']['start_time'],
			$form_state['values']['end_time'],
			$form_state['values']['access'],
			$form_state['values']['password'],
			$form_state['values']['languages'],
			$form_state['values']['onsite'],
			$cid
			);
	if(!db_error())
	{
		//if($form_state['values']['onsite'])
		//	variable_set($cid.'_update_contest_rank',1);
		drupal_set_message("The contest $cid has been updated.");
		drupal_goto("contest/$cid");
	}
}

function oj_contest_admin_access($cid)
{
	global $user;
	if($user->uid==1||$user->roles['5']!=NULL)
		return true;
	return FALSE;
}


function oj_contest_add()
{
	drupal_set_title('Add a new Contest');
	drupal_add_js(drupal_get_path('module','oj_contest').'/oj_contest.js');
	drupal_add_css(drupal_get_path('module','oj').'/misc/jquery-ui/css/cupertino/jquery-ui-1.7.3.custom.css');

	$breadcrumb = array();
	$breadcrumb[] = l('Home', '<front>');
	$breadcrumb[] = l('Contests', 'contests');
	drupal_set_breadcrumb($breadcrumb);
	
	return drupal_get_form('oj_contest_add_form');
}

function oj_contest_add_form($form_state)
{
	global $user;
	date_default_timezone_set("Asia/Chongqing");
	
	$form['uid'] = array
	(
		'#type' => 'value',
		'#value' => $user->uid,
	);

	$form['creator'] = array
	(
		'#type' => 'item',
		'#title' => t('Creator'),
		'#value' => "<a href=".url('user/'.$user->uid,array('absolute' => TRUE)).">".$user->name."</a>",
	);
	
	$form['title'] = array
	(
		'#title' => t('Title'),
		'#type' => 'textfield',
	);
	$form['access'] = array
	(
		'#title' => t('Access'),
		'#type' => 'radios',
		'#default_value' => 0,
		'#options' => array('Public','Register','Authorized','Password'),
		'#attributes' => array('class'=>'contest_access'),
	);
	$form['password'] = array
	(
		'#title' => t('Password'),
		'#type' => 'textfield',
	);
	
	$js = <<<JS
	$(document).ready(function()
	{
		if($("#edit-access-3").attr("checked") != "checked")
			$("#edit-password-wrapper").hide();
		$(".contest_access").change(function()
		{
			if($("#edit-access-3").attr("checked") == "checked")
				$("#edit-password-wrapper").show();
			else
				$("#edit-password-wrapper").hide();
		});
		
	});
JS;
	drupal_add_js($js,'inline');
	
	$form['start_time'] = array
	(
		'#title' => t('Start Time'),
		'#type' => 'fieldset',
	);
	$form['start_time']['start_time_date'] = array
	(
		
		'#type' => 'textfield',
		'#default_value' => date('Y-m-d',time()+3600),
		'#attributes' => array('readonly'=>'readonly'),
		'#size' => 10,
	);
	$form['start_time']['start_time_hour'] = array
	(
		'#type' => 'textfield',
		'#default_value' => date('H',time()+3600),
		'#size' => 2,
		'#maxlength' => 2,
	);
	$form['start_time']['start_time_min'] = array
	(
		'#type' => 'textfield',
		'#default_value' => date('i',time()+3600),
		'#size' => 2,
		'#maxlength' => 2,
	);
	$form['start_time']['start_time_sec'] = array
	(
		'#type' => 'textfield',
		'#default_value' => date('s',time()+3600),
		'#size' => 2,
		'#maxlength' => 2,
	);
	
	$form['end_time'] = array
	(
		'#title' => t('Start Time'),
		'#type' => 'fieldset',
	);
	$form['end_time']['end_time_date'] = array
	(
		
		'#type' => 'textfield',
		'#default_value' => date('Y-m-d',time()+18000),
		'#attributes' => array('readonly'=>'readonly'),
		'#size' => 10,
	);
	$form['end_time']['end_time_hour'] = array
	(
		'#type' => 'textfield',
		'#default_value' => date('H',time()+18000),
		'#size' => 2,
		'#maxlength' => 2,
	);
	$form['end_time']['end_time_min'] = array
	(
		'#type' => 'textfield',
		'#default_value' => date('i',time()+18000),
		'#size' => 2,
		'#maxlength' => 2,
	);
	$form['end_time']['end_time_sec'] = array
	(
		'#type' => 'textfield',
		'#default_value' => date('s',time()+18000),
		'#size' => 2,
		'#maxlength' => 2,
	);
	
	$form['language'] = array
	(
		'#type' => 'checkboxes',
		'#title' => t('Language'),
		'#default_value' => oj_languages(TRUE),
		'#options' => oj_languages(),
	);
	
	$form['onsite'] = array
	(
		'#type' => 'radios',
		'#title' => t('Onsite'),
		'#default_value' => 0,
		'#options' => array('No','Yes'),
	);
	
	$form['description'] = array
	(
		'#title' => t('Description'),
		'#type' => 'textarea',
	);
	
	$form['submit'] = array
	(
		'#type' => 'submit',
		'#value' => t('Submit'),
	);
	return $form;
}

function oj_contest_add_form_validate($form,&$form_state)
{
	if(empty($form_state['values']['title']))
		form_set_error('title','The contest title is required.');
	list($year,$month,$day) = explode('-',$form_state['values']['start_time_date']);
	$start_time = mktime((int)$form_state['values']['start_time_hour'],(int)$form_state['values']['start_time_min'],(int)$form_state['values']['start_time_sec'],(int)$month,(int)$day,(int)$year);
	list($year,$month,$day) = explode('-',$form_state['values']['end_time_date']);
	$end_time = mktime((int)$form_state['values']['end_time_hour'],(int)$form_state['values']['end_time_min'],(int)$form_state['values']['end_time_sec'],(int)$month,(int)$day,(int)$year);
	
	if($end_time - $start_time < 3600)
	{
		form_set_error('start_time','The contest time is incorrect.');
	}
	$form_state['values']['start_time'] = $start_time;
	$form_state['values']['end_time'] = $end_time;
	if($form_state['values']['access'] == 3)
	{
		if(empty($form_state['values']['password']))
			form_set_error('password','The password is required.');
	}
	else
	{
		$form_state['values']['password'] = NULL;
	}
	$languages = array();
	foreach($form_state['values']['language'] as $value)
	{
		if(!empty($value))
			$languages[] = $value;
	}
	if(!count($languages))
		form_set_error('language','At least one language is required.');
	$form_state['values']['languages'] = implode(';',$languages);
}

function oj_contest_add_form_submit($form,$form_state)
{
	db_query("INSERT INTO {contests} (uid,title,description,
			start_time,end_time,access,password,
			language,onsite) VALUES (%d,'%s','%s',%d,%d,%d,'%s','%s',%d)",
			$form_state['values']['uid'],
			$form_state['values']['title'],
			$form_state['values']['description'],
			$form_state['values']['start_time'],
			$form_state['values']['end_time'],
			$form_state['values']['access'],
			$form_state['values']['password'],
			$form_state['values']['languages'],
			$form_state['values']['onsite']
			);
	if(!db_error())
	{
		$cid = db_result(db_query("SELECT LAST_INSERT_ID()"));
		/***** freeze board *****/
		if($form_state['values']['onsite'])
			variable_set($cid.'_update_contest_rank',1);
		
		drupal_set_message("The contest $cid has been created.");
		drupal_goto("contest/$cid");
	}
}

function oj_contest_problems_form(&$form_state,$cid)
{
	drupal_add_js(drupal_get_path('module','datatables').'/theme/js/jquery-ui-1.8.19.custom.min.js');
	drupal_add_js(drupal_get_path('module','oj_contest').'/oj_contest.js');
	$contest = contest_load($cid);
	$status = get_contest_status($cid);
	
	$form['cid'] = array('#type' => 'value', '#value' => $cid);
	
	$form['problems'] = array
	(
		'#type' => 'fieldset',
		'#title' => t('Problems Manage'),
		'#collapsible' => TRUE,
	);
	
	$form['problems']['list'] = array
	(
		'#prefix' => '<div id="contest-problem-list">',
		'#suffix' => '</div>',
	);
	
	$form['problems']['list']['table_start'] = array
	(
		'#value' => '<table id=contest-problems><thead><th></th><th>Num</th><th>PID</th><th>Alias</th><th></th><th>title</th></thead>',
	);
	
	if(empty($form_state['values']['problems']))
		$problems = $contest->problems;
	else
		$problems = $form_state['values']['problems'];
	if(empty($problems))
		$problems[] = array('pid'=>'','alias'=>NULL);
	
	$delta = 0;
	foreach($problems as $key => $problem)
	{
		$form['problems']['list'][$delta] = _oj_contest_problems_form($delta,$problem,$status);
		$delta++;
	}
	
	$form['problems']['list']['table_end'] = array
	(
		'#value' => '</table>',
	);
	if($status < 2)
	{
		//$index = max(array_keys($problems));
		//if($index<24)
		$form['problems']['add_problem'] = array
		(
			'#type' => 'submit',
			'#value' => t('Add a problem'),
			'#submit' => array('contest_add_problem_submit'),
			'#ahah' => array
			(
				'path' => 'contest_add_problem.ajax',
				'wrapper' => 'contest-problem-list',
			),
		);
		
		$form['problems']['submit'] = array
		(
			'#type' => 'submit',
			'#value' => t('Submit'),
			'#validate' => array('oj_contest_problems_validate'),
			'#submit' => array('oj_contest_problems_submit'),
		);
	}
	//dsm($form);
	//dsm($form_state);
	return $form;
}

function _oj_contest_problems_form($key,$problem,$status)
{
	$form = array
	(
		'#tree' => TRUE,
	);
	
	if($status < 1)
	{
		$form['del'] = array
		(
			'#type' => 'image_button',
			//'#value' => $key,
			'#src' => drupal_get_path('module','oj').'/misc/images/ico_delete.gif',
			'#prefix' => '<tr><td>',
			'#suffix' => '</td>',
			'#submit' => array('contest_del_problem_submit'),
			/*'#ahah' => array
			(
				'path' => "contest_del_problem.ajax",
				'wrapper' => "contest-problem-list",
			),*/
			'#parents' => array('problems', $key, 'del'),
		);
	}
	$form['num'] = array
	(
		'#type' => 'item',
		'#value' => chr($key+ord('A')),
		'#prefix' => '<td>',
		'#suffix' => '</td>',
		'#parents' => array('problems', $key, 'num'),
	);

	$form['pid'] = array
	(
		'#type' => 'textfield',
		'#default_value' => $problem['pid'],
		'#size' => 4,
		'#maxlength' => 4,
		'#prefix' => '<td>',
		'#suffix' => '</td>',
		/*'#ahah' => array
		(
			'path' => "oj_contest_problem_title.ajax/$key",
			'wrapper' => "problem-title-$key",
		),*/
		'#parents' => array('problems', $key, 'pid'),
	);
		
	$form['alias'] = array
	(
		'#type' => 'textfield',
		'#default_value' => $problem['alias'],
		'#size' => 40,
		'#prefix' => '<td>',
		'#suffix' => '</td>',
		'#parents' => array('problems', $key, 'alias'),
	);
	
	$node = problem_load($problem['pid']);
	$form['separator'] = array
	(
		'#value' => '<td></td><td>',
		'#parents' => array('problems', $key, 'separator'),
	);
	$title = !empty($problem['pid'])&&empty($node->title)?'No such problem!':'<a href='.url('problem/'.$problem['pid']).'>'.$node->title.'</a>';
	$form['title'] = array
	(
		'#type' => 'item',
		'#value' => $title,
		'#prefix' => "<div id=problem-title-$key>",
		'#suffix' => '</div>',
		'#parents' => array('problems', $key, 'title'),
	);
	$form['tr_end'] = array
	(
		'#value' => '</td></tr>',
		'#parents' => array('problems', $key, 'tr_end'),
	);
	
	return $form;
}

function oj_contest_problems_validate($form,&$form_state)
{
	$problems = array();
	foreach($form_state['values']['problems'] as $key => $problem)
	{
		$pid = $problem['pid'];
		if($pid)
		{
			$alias = $problem['alias'];
			$node = problem_load($pid);
			if(empty($node->title))
				;//form_set_error("problems][$key][pid",'The problem is not exists.');
			else
			{
				if(!isset($problems[$pid]))
					$problems[$pid] = $alias;
				else if($alias)
					$problems[$pid] = $alias;
			}
		}
	}
	$count = count($problems);
	if($count == 0)
		form_set_error('','At least one problem required.');
	if($count > 26)
		form_set_error('','The problems is too many.');
	$form_state['values']['#problems'] = $problems;
	//dsm($form_state);
}

function oj_contest_problems_submit($form,&$form_state)
{
	global $user;
	//dsm($form_state);
	$cid = $form_state['values']['cid'];
	//dsm($cid);
	db_query("DELETE FROM {contest_problem} WHERE cid=%d",$cid);
	$num = 1;
	foreach($form_state['values']['#problems'] as $pid => $alias)
	{
		db_query("INSERT INTO {contest_problem} (cid,pid,uid,alias,num) VALUES (%d,%d,%d,'%s',%d)",
				$cid,
				$pid,
				$user->uid,
				$alias,
				$num++);
	}
	drupal_set_message("The problems have been updated.");
}

function oj_contest_delete($cid)
{
	
}

function oj_contest_add_problem($cid)
{
	$contest = contest_load($cid);
	
}

function oj_contest_add_user($cid)
{
	
}

function oj_contest_add_announcement($cid)
{
	
}

function oj_contest_datetime($contest)
{
	$start_time = date($contest->start_time,'custom','Y-m-d H:i:s');
	$end_time = date($contest->end_time,'custom','Y-m-d H:i:s');
	
}

function contest_problem_title_ajax($pid)
{
	$title = problem_load($pid,TRUE)->title;
	echo empty($title)?'No such problem!':"<a href=problem/$pid>$title</a>";
}

function oj_contest_problem_title_ajax($num)
{
	$form = ahah_callback_helper();

	$pid = $form['problems']['list'][$num]['pid']['#value'];
	$title = problem_load($pid,TRUE)->title;
	
	$textfields = $form['problems']['list'][$num]['title'];
	$textfields['#value'] = empty($title)?'No such problem!':"<a href=problem/$pid>$title</a>";
	unset($textfields['#prefix'], $textfields['#suffix']);
	
	$output = theme('status_messages').drupal_render($textfields);
	drupal_json(array('status' => TRUE, 'data' => $output));
}

function contest_add_problem_submit($form, &$form_state)
{
	//dsm($form_state);
	if($form_state['post']['op'] == 'Add a problem')
	{
		$index = max(array_keys($form_state['values']['problems']));
		if($index >= 25)
		{
			form_set_error('problems','The problems is too many.');
			return;
		}
		if($pid = $form_state['values']['problems'][$index]['pid'])
			$pid = get_next_problem($pid,TRUE);
		$form_state['values']['problems'][] = array('pid'=>$pid,'alias'=>NULL);
		//drupal_set_message('The change will be saved after submit.','info');
	}
}

function contest_add_problem_ajax()
{
	$form = ahah_callback_helper();
	
	$tablefields = $form['problems']['list'];
	unset($tablefields['#prefix'],$tablefields['#suffix']);
	$output = theme('status_messages').drupal_render($tablefields);

	drupal_json(array('status' => TRUE, 'data' => $output));
}

function contest_del_problem_submit($form, &$form_state)
{
	$name = $form_state['clicked_button']['#name'];
	if(preg_match('/problems\[(.*)\]\[del\]/',$name,$m))
		$index = $m[1];
	unset($form_state['values']['problems'][$index]);
	
	oj_contest_problems_validate($form,$form_state);
	oj_contest_problems_submit($form,$form_state);
}

function contest_del_problem_ajax()
{
	$form = ahah_callback_helper();
	
	$tablefields = $form['problems']['list'];
	unset($tablefields['#prefix'],$tablefields['#suffix']);
	$output = theme('status_messages').drupal_render($tablefields);

	drupal_json(array('status' => TRUE, 'data' => $output));
}
