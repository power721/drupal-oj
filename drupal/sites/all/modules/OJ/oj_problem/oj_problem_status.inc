<?php

// server side processing:
function oj_status()
{
	$head=array
	(
		array('data' => 'Run ID'),
		array('data' => 'User'),
		array('data' => 'Problem'),
		array('data' => 'Result'),
		array('data' => 'Memory'),
		array('data' => 'Time'),
		array('data' => 'Language'),
		array('data' => 'Code Length'),
		array('data' => 'Submit Time'),
	);

	// one dummy row, needed for initialization
	$dummy_row[]=array
	(
		array('data' => "Loading..."),
		array('data' => "Loading..."),
		array('data' => "Loading..."),
		array('data' => "Loading..."),
		array('data' => "Loading..."),
		array('data' => "Loading..."),
		array('data' => "Loading..."),
		array('data' => "Loading..."),
		array('data' => "Loading..."),
	);
	
	$colwidths=array
	(
		array('bSortable'=>true,'sWidth'=>'8%','asSorting'=>array('desc','asc')),
		array('bSortable'=>false,'sWidth'=>'12%'),
		array('bSortable'=>false,'sWidth'=>'6%'),
		array('bSortable'=>false,'sWidth'=>'30%'),
		array('bSortable'=>true,'sWidth'=>'10%','asSorting'=>array('desc','asc')),
		array('bSortable'=>true,'sWidth'=>'10%','asSorting'=>array('desc','asc')),
		array('bSortable'=>false,'sWidth'=>'3%'),
		array('bSortable'=>true,'sWidth'=>'7%'),
		array('bSortable'=>false,'sWidth'=>'14%'),
	);
	//debug($_GET,'$_GET');
	$url = "ajax.php?q=oj_status_ajax";
	if($_GET['uid'])
		$url .= "&uid=".$_GET['uid'];
	if($_GET['pid'])
		$url .= "&pid=".$_GET['pid'];
	if(isset($_GET['result']))
		$url .= "&result=".$_GET['result'];
	if(isset($_GET['language']))
		$url .= "&language=".$_GET['language'];
	$params=array
	(
		'datatable_options' =>array
		(
			'sPaginationType' => 'full_numbers',
            'sDom' => '<"H"lp<"clear">>rt<"F"ip<"clear">><"clear">',
            //'bStateSave'  => true,  // needs work to remember the independent input search box values
            'sCookiePrefix' => 'pl_',
            'bProcessing' => true,
            'bServerSide' => true,
            'sAjaxSource' => $url,
            'aoColumns' => $colwidths,
            'bJQueryUI' => true,
			'bAutoWidth' => false,
			'iDisplayLength'=>25,
			'aLengthMenu'=>array(25, 50, 100, 200),
        )
    );
	$script=<<<SCR
$(document).ready(function() 
	{
		$("#page_num").keyup( 
		function () 
		{
            t=$('#datatable-1').dataTable();
			if(this.value == null || this.value=="" || this.value<1)
				this.value = 1;
            t.fnPageChange(this.value-1);
        });
		
        $("#search-user").keyup( 
		function () 
		{
            t=$('#datatable-1').dataTable();
            t.fnFilter( this.value, 1 );
        });
        $("#search-pid").keyup( 
		function () 
		{
            t=$('#datatable-1').dataTable();
            t.fnFilter( this.value, 2 );
         });
		
		$("#search-result").change( 
		function () 
		{
            t=$('#datatable-1').dataTable();
            t.fnFilter( this.value, 3 );
        });
        $("#search-language").change( 
		function () 
		{
            t=$('#datatable-1').dataTable();
            t.fnFilter( this.value, 6 );
         });
        $("#search_button").click( function ()
		{
         	t=$('#datatable-1').dataTable();
			t.fnFilter( $("#search-user").val(), 1 );
			t.fnFilter( $("#search-pid").val(), 2 );
			t.fnFilter( $("#search-result").val(), 3 );
			t.fnFilter( $("#search-language").val(), 6 );
        });
} );
SCR;

	drupal_add_js($script,'inline');
	//drupal_set_html_head("<META HTTP-EQUIV=Refresh content=".variable_get('status_refresh_time',60).">");
	
	# add the themeroller widget set (bJQueryUI=true)
	//drupal_add_css(drupal_get_path('module','datatables').'dataTables/media/css/demo_table_jui.css');
	drupal_add_css(drupal_get_path('module','oj').'/misc/jquery-ui/css/start/jquery-ui-1.7.3.custom.css');

	global $user;
	
	$pid = $_GET['pid'];
	if(isset($_GET['result']))
		$result = $_GET['result'];
	
	$inputboxes = '<table style="border:0px;background-color:#fedc6b; float:left">
                <tr><td>Filter User:</td><td><input id="search-user" type=text size=15 value='.$_GET['uid'].'></td>
                <td>Problem ID:</td><td><input id="search-pid" type=text size=10 value='.$pid.'></td>
				<td>Result:<select size=1 id=search-result>
				<option value=>All</option>
				<option value=0 '.(isset($result)&&$result==0?'selected':'').'>Accepted</option>
				<option value=1 '.($result==1?'selected':'').'>Presentation Error</option>
				<option value=2 '.($result==2?'selected':'').'>Time Limit Exceeded</option>
				<option value=3 '.($result==3?'selected':'').'>Memory Limit Exceeded</option>
				<option value=4 '.($result==4?'selected':'').'>Wrong Answer</option>
				<option value=5 '.($result==5?'selected':'').'>Runtime Error</option>
				<option value=6 '.($result==6?'selected':'').'>Output Limit Exceeded</option>
				<option value=7 '.($result==7?'selected':'').'>Compile Error</option>';
	if($user->uid==1||$user->roles['5']!=NULL)
		$inputboxes .= '<option value=98 '.($result==98?'selected':'').'>System Error</option>
						<option value=99 '.($result==99?'selected':'').'>Validate Error</option>
						<option value=100 '.($result==100?'selected':'').'>Data Error</option>
						<option value=10000 '.($result==10000?'selected':'').'>Waiting</option>';
	$results = array
	(
		'' 	=> 'All',
		0 	=> 'Accepte',
		1 	=> 'Presentation Error',
		2 	=> 'Time Limit Exceeded',
		3 	=> 'Memory Limit Exceeded',
		4 	=> 'Wrong Answer',
		5 	=> 'Runtime Error',
		6	=> 'Output Limit Exceeded',
		7 	=> 'Compile Error',
		98 	=> 'System Erro',
		99 	=> 'Validate Erro',
		100 => 'Data Error',
		1000 => 'Waiting',
	);
	$page = 1;
	if(!empty($_GET['iDisplayLength']))
		$page = $_GET['iDisplayStart']/$_GET['iDisplayLength']+1;
	$languages = oj_languages(TRUE);
	$options = "<option value=>All</option>";
	foreach($languages as $key => $value)
	{
		$options .= "<option value=$key>$value</option>\n";
	}
	$inputboxes .= '</select></td>
				<td>Language:<select size=1 id=search-language>'.
				$options
				.'</select></td>
				<td>Page:<input id="page_num" type=text size=10 value='.$page.'></td>
				</tr>
				</table>
				<input id="search_button" type="submit" value="Search">';
	return $inputboxes . theme_datatable($head,$dummy_row,$params);
}
/*
function oj_status_ajax()
{
	$lim_off=db_escape_string($_GET['iDisplayStart']);
	$lim_rowc=db_escape_string($_GET['iDisplayLength']);
	
	$allrecord=db_result(db_query("SELECT COUNT(*) FROM {solution}"));
	$sql="SELECT SQL_CALC_FOUND_ROWS s.sid,s.cid,s.num,u.name,s.uid,s.pid,s.result,s.memory,s.time,s.language,s.code_length,s.in_date FROM {solution} s LEFT JOIN {users} u ON u.uid=s.uid WHERE 1";
    
	if(!empty($_GET['sSearch_1']))
        $sql.=" AND u.name LIKE '" . $_GET['sSearch_1']  . "%%' ";
	else if($_GET['uid'])
	{	
		$d_user = user_load($_GET['uid']);
		$_GET['sSearch_1'] = $d_user->name;
		$sql.=" AND u.name = '" . $_GET['sSearch_1']  . "' ";
	}
	
	if(!empty($_GET['sSearch_2']))
		$pid = $_GET['sSearch_2'];
	else if($_GET['pid'])
		$pid = $_GET['pid'];
    if(!empty($pid))
		$sql.=" AND  s.pid LIKE '" . $pid . "%%'";
	
	if($_GET['sSearch_3']!=NULL)
		$result = $_GET['sSearch_3'];
	else if(!is_NULL($_GET['result']))
		$result = $_GET['result'];
    if(!is_NULL($result))
		$sql.=" AND  s.result = " . $result;
	
	if($_GET['sSearch_6']!=NULL)
		$language = $_GET['sSearch_6'];
	else if(!is_NULL($_GET['language']))
		$language = $_GET['language'];
	if(!is_NULL($language))
		$sql.=" AND  s.language = " . $language;
	
    $sqlorderby=" ORDER BY "; 
    $ordercomma="";  
	
    if ($_GET['iSortingCols'] > 0)
	{
        for($s=0; $s < $_GET['iSortingCols']; $s++)
		{
			switch($_GET["iSortCol_$s"])
			{
				case 1: $sqlorderby.= $ordercomma . "sid " . $_GET["sSortDir_$s"];
						$ordercomma=",";
						break;
				case 4: $sqlorderby.= $ordercomma . "s.result,memory " . $_GET["sSortDir_$s"].",time,code_length";  
						$ordercomma=",";
						break;
				case 5: $sqlorderby.= $ordercomma . "s.result,time " . $_GET["sSortDir_$s"].",memory,code_length";  
						$ordercomma=",";
						break;
				case 7: $sqlorderby.= $ordercomma . "code_length " . $_GET["sSortDir_$s"].",time,memory";  
						$ordercomma=",";
						break;
			}
        }
    }
     
	$sqlorderby.= $ordercomma . "sid desc"; 
    if(!empty($sqlorderby))
		$sql.=$sqlorderby;
    
    $sql.=" LIMIT $lim_off, $lim_rowc";
    $items=db_query($sql);
	
	$languages = oj_languages(TRUE);
	$result_type = oj_get_status_result();
	
    $filteredrec=db_result(db_query("SELECT FOUND_ROWS()"));
	
    $out= '{ "sEcho":' . $_GET['sEcho'] . ', ';
    $out.='"iTotalRecords": ' . $allrecord .', ';
    $out.='"iTotalDisplayRecords": ' . $filteredrec . ', ';
    $out.='"aaData": [ ';
    echo $out;
    
    $comma="";
    while ($r = db_fetch_array($items))
	{
		if(!empty($r['cid'])&&contest_is_running($r['cid']))
		{
			$problem_num = chr($r['num']-1+ord('A'));
			$linkbeg=addslashes('<a href="' . url("contest/{$r['cid']}/problem/".$problem_num) . '">');
			$linkend=addslashes('</a>');
			$problem_url = $linkbeg . $r['cid'].'-'.$problem_num . $linkend;
		}
		else
		{
			$linkbeg=addslashes('<a href="' . url('problem/' . $r['pid']) . '">');
			$linkend=addslashes('</a>');
			$problem_url = $linkbeg . $r['pid'] . $linkend;
		}
        echo $comma;$comma=",";
        
		date_default_timezone_set("Asia/Chongqing");
		$row= '["' .$r['sid'].
			 '","' . "<a href=" . url('user/'.$r['uid']) . ">"  .  $r['name'] . "</a>" . 
			 '","' . $problem_url .
			 '","' . getResultDescript($r['result']) .
			 '","' . ($r['result']==0?$r['memory']."K":"") .
			 '","' . ($r['result']==0?$r['time'] . "MS":"") .
			 '","' . (show_code_access($r['sid'])?'<a href=' . url('code/'.$r['sid']) . '>':'') .$languages[$r['language']] . (show_code_access($r['sid'])?'</a>':'') .
			 '","' . $r['code_length'] . 'b' . 
			 '","' . date('Y-m-d H:i:s',$r['in_date']) . 
			 '"]';

        echo $row;
    }
    echo ']}';
}
*/
// server side processing:
function oj_problem_status($node=NULL)
{
	$pid = $node->field_pid['0']['value'];
	$head=array
	(
		array('data' => 'Run ID'),
		array('data' => 'User'),
		array('data' => 'Memory'),
		array('data' => 'Time'),
		array('data' => 'Language'),
		array('data' => 'Code Length'),
		array('data' => 'Submit Time'),
	);

	// one dummy row, needed for initialization
	$dummy_row[]=array
	(
		array('data' => "Loading..."),
		array('data' => "Loading..."),
		array('data' => "Loading..."),
		array('data' => "Loading..."),
		array('data' => "Loading..."),
		array('data' => "Loading..."),
		array('data' => "Loading..."),
	);
	
	$colwidths=array
	(
		array('bSortable'=>true,'sWidth'=>'10%'),
		array('bSortable'=>false,'sWidth'=>'16%'),
		array('bSortable'=>true,'sWidth'=>'10%'),
		array('bSortable'=>true,'sWidth'=>'10%'),
		array('bSortable'=>false,'sWidth'=>'6%'),
		array('bSortable'=>true,'sWidth'=>'8%'),
		array('bSortable'=>false,'sWidth'=>'20%'),
	);
	
	$params=array
	(
		'datatable_options' =>array
		(
			'sPaginationType' => 'full_numbers',
            'sDom' => '<"H"lp<"clear">>rt<"F"ip<"clear">><"clear">',
            //'bStateSave'  => true,  // needs work to remember the independent input search box values
            'sCookiePrefix' => 'pl_',
            'bProcessing' => true,
            'bServerSide' => true,
            'sAjaxSource' => '?q=oj_problem_status.ajax/'.$pid,
            'aoColumns' => $colwidths,
            'bJQueryUI' => true,
			'bAutoWidth' => false,
			'iDisplayLength'=>20,
			'aLengthMenu'=>array(10, 20, 50, 100),
        )
    );
	$params['datatable_options']['aaSorting']=array(array(3,'asc'),array(2,'asc'),array(5,'asc'),array(1,'desc'));
	// script to initialize field seach box events
	
	$script=<<<SCR
$(document).ready(function() 
	{
        $("#search-language").change( 
		function () 
		{
            t=$('#datatable-1').dataTable();
            t.fnFilter( this.value, 6 );
         });
} );
SCR;
	drupal_add_js($script,'inline');
	
	# add the themeroller widget set (bJQueryUI=true)
	//drupal_add_css(drupal_get_path('module','datatables').'dataTables/media/css/demo_table_jui.css');
	drupal_add_css(drupal_get_path('module','oj').'/misc/jquery-ui/css/start/jquery-ui-1.7.3.custom.css');	
	
	$languages = oj_languages(TRUE);
	$options = "<option value=>All</option>";
	foreach($languages as $key => $value)
	{
		$options .= "<option value=$key>$value</option>\n";
	}
	$inputboxes='<table style="border:0px;background-color:#fedc6b; float:right">
                <tr>
				<td>
				Language:<select size=1 id=search-language>'.
				$options
				.'</select>
				</td>
				</tr>
				</table>
				';
	/*
	$item = db_fetch_object(db_query("SELECT field_submit_value,field_accepted_value,
				field_submit_user_value,field_solved_value
				FROM {content_type_problem} WHERE field_pid_value=%d",$pid));
	$submit = $item->field_submit_value;
	$accepted = $item->field_accepted_value;
	$submit_user = $item->field_submit_user_value;
	$solved = $item->field_solved_value;
	*/
	$items =db_query("SELECT result,COUNT(*) as sum FROM {solution} WHERE pid=%d GROUP BY result",$pid);
	$oj_result = oj_get_status_result();
	while($item =  db_fetch_object($items))
	{
		$data[$oj_result[$item->result]] = (int)$item->sum;
		$links[$oj_result[$item->result]] = url("status",array('absolute'=>TRUE))."?pid=$pid&result=".$item->result;
	}
	$scr = problem_status_generate_pie_chart_2($data,$links);
	return $scr . $inputboxes . theme_datatable($head,$dummy_row,$params);
}

function oj_problem_status_ajax($pid=0)
{
	$lim_off=db_escape_string($_GET['iDisplayStart']);
	$lim_rowc=db_escape_string($_GET['iDisplayLength']);
	
	$allrecord=db_result(db_query("SELECT COUNT(*) FROM {solution} WHERE pid=%d",$pid));
	$sql="SELECT SQL_CALC_FOUND_ROWS s.sid,u.name,s.uid,s.pid,s.result,s.memory,s.time,s.language,s.code_length,s.in_date FROM {solution} s LEFT JOIN {users} u ON u.uid=s.uid WHERE 1";
	
	$sql.=" AND s.pid=$pid AND s.result=0";
    // NOTE: use double %, because drupal printf()s it before sending to mysql...
	
	if (!empty($_GET['sSearch_1']))
        $sql.=" AND u.name LIKE '" . $_GET['sSearch_1']  . "%%' ";
	
    if (!empty($_GET['sSearch_2']))
		$sql.=" AND  s.pid = " . $_GET['sSearch_2'];
	
    if ($_GET['sSearch_3']!=NULL)
		$sql.=" AND  s.result = " . $_GET['sSearch_3'];
	
	if ($_GET['sSearch_6']!=NULL)
		$sql.=" AND  s.language = " . $_GET['sSearch_6'];
    // because of condition listing, one odering is always needed: based on nid
    // to be able to group the same notification (with just differenct conditions) together
    $sqlorderby=" ORDER BY "; 
    $ordercomma="";  
	
    if ($_GET['iSortingCols'] > 0)
	{
        for($s=0; $s < $_GET['iSortingCols']; $s++)
		{
			switch($_GET["iSortCol_$s"])
			{
				case 0: $sqlorderby.= $ordercomma . "sid " . $_GET["sSortDir_$s"];
						$ordercomma=",";
						break;
				case 2: $sqlorderby.= $ordercomma . "s.result,memory " . $_GET["sSortDir_$s"].",time,code_length";  
						$ordercomma=",";
						break;
				case 3: $sqlorderby.= $ordercomma . "s.result,time " . $_GET["sSortDir_$s"].",memory,code_length";  
						$ordercomma=",";
						break;
				case 5: $sqlorderby.= $ordercomma . "code_length " . $_GET["sSortDir_$s"].",time,memory";  
						$ordercomma=",";
						break;
			}
        }
    }
    
	//if($pid!=NULL)
	//	$sqlorderby.= $ordercomma . "s.time,s.memory,s.code_length"; 
    if(!empty($sqlorderby))
		$sql.=$sqlorderby;
    
    # limit is simply based on user choice
    $sql.=" LIMIT $lim_off, $lim_rowc";
    // Query that mess finally!
    $result=db_query($sql);
	$language = oj_languages(TRUE);
    //$language = array('G++','GCC','JAVA','Pascal','Masm32');
	$result_type =oj_get_status_result();
    // Query the rows counted without the limit
    $filteredrec=db_result(db_query("SELECT FOUND_ROWS()"));
	
    // Start constructing JSON data
    $out= '{ "sEcho":' . $_GET['sEcho'] . ', ';
    $out.='"iTotalRecords": ' . $allrecord .', ';
    $out.='"iTotalDisplayRecords": ' . $filteredrec . ', ';
    $out.='"aaData": [ ';
    print $out;
    // deliver the rows 
    $comma=""; // comma must not be written after the last row (IE)
    while ($r = db_fetch_array($result))
	{
      	foreach ($r as $key => $value)
      	{
      		$r[$key] = trim($value);
      	}
        print $comma; $comma=",";
        $linkbeg=addslashes('<a href="' . url('problem/' . $r['pid']) . '">');
        $linkend=addslashes('</a>');   
		
		$row= '["' .$r['sid'].
			 '","' . "<a href=" . url('user/'.$r['uid']) . ">"  .  $r['name'] . "</a>" . 
			 '","' . ($r['result']==0?$r['memory']."K":"") .
			 '","' . ($r['result']==0?$r['time'] . "MS":"") .
			 '","' . (show_code_access($r['sid'])?'<a href=' . url('code/'.$r['sid']) . '>':'') .$language[$r['language']] . (show_code_access($r['sid'])?'</a>':'') .
			 '","' . $r['code_length'] . 'b' . 
			 '","' . date('Y-m-d H:i:s',$r['in_date']) . 
			 '"]';

        print $row;
    }
    print ']}';
}

function import_solution($p1=1000,$p2=1099,$redirect=TRUE)
{
	if($p1<0||$p2<0||$p1>$p2)
	{
		drupal_set_message("solution import:Invalid Arguments.");
		if($redirect)
			drupal_goto('status');
		return;
	}
	$start_time = time();
	$last_sid = variable_get('last_import_solution',1000);
	
	set_time_limit(0);
	if($p1==NULL)
	{
		$p1=1000;
		$p2=1099;
	}
	if($p2==NULL)
		$p2=1099;
	
	$num = 0;
	db_set_active('JudgeOnline');
	db_query("SET NAMES 'latin1'");
	
	$query="SELECT
				*,
				UNIX_TIMESTAMP(in_date) AS in_date_t
			FROM
				solution
			WHERE
				solution_id>=%d
				AND solution_id<=%d
			ORDER BY
				solution_id";
	$result=db_query($query,$p1,$p2);
	db_set_active('default');
	while($solution = db_fetch_object($result))
	{
		$last_sid = max($last_sid,$solution->solution_id);
		$items=db_query("SELECT sid FROM {solution} WHERE in_date=%d",$solution->in_date_t);
		if($item = db_fetch_object($items))
		{
			if($item->sid!=NULL)
				continue;
		}
		
		$uid = 0;
		$items=db_query("SELECT uid FROM {users} WHERE name='%s'",$solution->user_id);
		if($item = db_fetch_object($items))
			$uid = $item->uid;
		db_set_active('JudgeOnline');
		$source_code = db_result(db_query("SELECT uncompress(source) FROM source_code WHERE solution_id=%d",$solution->solution_id));
		$compileinfo = db_result(db_query("SELECT error FROM compileinfo WHERE solution_id=%d",$solution->solution_id));
		db_set_active('default');
		$pid = db_result(db_query("SELECT field_pid_value FROM {content_type_problem} WHERE field_problem_id_value=%d",$solution->problem_id));
		$cid = NULL;
		if($solution->contest_id)
			$cid = db_result(db_query("SELECT cid FROM {contests} WHERE contest_id=%d",$solution->contest_id));
		$language = array(0,1,3,2,4);
		db_query("INSERT INTO {solution} 
		(pid,cid,uid,time,memory,in_date,result,language,ip,num,code_length,source,error) VALUES 
		(%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d,%d,compress('%s'),'%s')",
			$pid,
			$cid,
			$uid,
			$solution->time,
			$solution->memory,
			$solution->in_date_t,
			$solution->result,
			$language[$solution->language],
			$solution->ip,
			$solution->num+1,
			$solution->code_length,
			phpcharset($source_code,'UTF-8'),
			$compileinfo
			) or drupal_set_message('Insert into database error.');
		
		$num++;
	}
	variable_set('last_import_solution',$last_sid);
	if(!$redirect)
		return;
	
	$msg = "You had import $num solution";
	if($num>1)
		$msg.="s";
	$msg.=", execution time: ".(time()-$start_time)."s.";
	drupal_set_message($msg);
	drupal_goto('status');
}

function reset_solution()
{
	$query="DELETE FROM {solution} WHERE 1";
	db_query($query);
	$query="ALTER TABLE {solution} AUTO_INCREMENT=1000";
	db_query($query);
	
	variable_set('last_import_solution',1000);
	
	drupal_set_message("You had delete all solutions.");
	drupal_goto('status');
}

function oj_problem_status_callback($node)
{
	return 'Problem Status';
}
/*
function problem_status_generate_pie_chart($data)
{
	$pie = ofc_api_element('pie');
	$pie->set('alpha', 0.8);

	$fade = ofc_api_element('fade');

	$bounce = ofc_api_element('bounce');
	$bounce->set('distance', 5);

	$pie->set('animate', array($fade, $bounce));
	$pie->set('start-angle', 0);
	$pie->set('tip', '#val# of #total#<br>#percent#');
	$chart_colors = array("#579575","#839557","#958c12","#953579","#4b5de4","#d8b83f","#ff5800","#0085cc");
	$pie->set('colours', $chart_colors);

	foreach($data as $key => $value)
	{
		$item = array
		(
			'value' => $value,
			'label' => $key,
		);
		$pie->add('value', $item);
	}

	$chart = ofc_api_chart();
	$chart->add('element', $pie);
	
	$title = array
	(
		'text' => 'Statistics',
		'style' => 'color: #f0f0f0; font-size: 20px',
	);
	$bg_colour = '#202020';
	$chart->set('title', $title);
	$chart->set('bg_colour', $bg_colour);

	return "<div class=open_flash_chart>".ofc_api_render($chart)."</div>";
}
*/
function problem_status_generate_pie_chart_2($data,$links = array(),$title_s="")
{
	if(empty($data))
		return "";
	foreach($data as $key => $value)
	{
		$slice = new pie_value($value,$key);
		$slice->on_click($links[$key]);
		$d[] = $slice;
	}
	$pie = new pie();
	
	$pie->set_animate(true);
	$bounce = new pie_bounce(5);
	$pie->add_animation($bounce);
	//$pie->set_label_colour('#432BAF');
	$pie->set_alpha(0.8);
	$pie->set_tooltip('#label#:#percent#<br>#val# of #total#');
	
	//$chart_colors = array("#579575","#839557","#958c12","#953579","#4b5de4","#d8b83f","#ff5800","#0085cc");
	$chart_colors = array_values(oj_get_status_color());
	$pie->set_colours($chart_colors);

	$pie->set_values($d);

	$chart = new open_flash_chart();
	$chart->add_element($pie);
	if($title_s=="")
		$title_s = 'Statistics';
	$title = new title($title_s);
	$title->set_style('color: #f0f0f0; font-size: 20px');
	$chart->set_title($title);
	$chart->set_bg_colour('#202020');

	return "<div class=open_flash_chart>".ofc_api_render($chart)."</div>";
}
