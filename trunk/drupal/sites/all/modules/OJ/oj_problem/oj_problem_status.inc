<?php

// server side processing:
function oj_status($pid=0,$s_result=0,$user=0,$s_language=0)
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
            'sAjaxSource' => "?q=oj_status.ajax/$pid/$s_result/$user/$s_language",
            'aoColumns' => $colwidths,
            'bJQueryUI' => true,
			'bAutoWidth' => false,
			'iDisplayLength'=>50,
			'aLengthMenu'=>array(array(25, 50, 100, 200),array('25', '50', '100', '200')),
        )
    );
	$script=<<<SCR
$(document).ready(function() 
	{
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
	drupal_add_css(drupal_get_path('module','datatables').'dataTables/media/css/demo_table_jui.css');
	drupal_add_css(drupal_get_path('module','datatables').'/theme/css/start/jquery-ui-1.8.17.custom.css');
	global $user;
	$inputboxes = '<table style="border:0px;background-color:#fedc6b; float:left">
                <tr><td>Filter User:</td><td><input id="search-user" type=text size=15></td>
                <td>Problem ID:</td><td><input id="search-pid" type=text size=15></td>
				<td>Result:<select size=1 id=search-result><option value=\'\'  selected>All</option><option value=0>Accepted</option><option value=1>Presentation Error</option><option value=2>Time Limit Exceeded</option><option value=3>Memory Limit Exceeded</option><option value=4>Wrong Answer</option><option value=5>Runtime Error</option><option value=6>Output Limit Exceeded</option><option value=7>Compile Error</option>';
	if($user->uid==1||$user->roles['5']!=null)
		$inputboxes .= '<option value=98>System Error</option><option value=99>Validate Error</option><option value=10000>Waiting</option>';
	$inputboxes .= '</select></td>
				<td>Language:<select size=1 id=search-language><option value=\'\'  selected>All</option><option value=0>G++</option><option value=1>GCC</option><option value=2>Java</option><option value=3>Pascal</option><option value=4>Masm32</option></select></td></tr>
				</table>
				<input id="search_button" type="submit" value="Search">';
			
	return $inputboxes . theme_datatable($head,$dummy_row,$params);
}

function oj_status_ajax($pid=0,$s_result=0,$user=0,$s_language=0)
{
	$s_result--;
	$s_language--;
	
	$lim_off=db_escape_string($_GET['iDisplayStart']);
	$lim_rowc=db_escape_string($_GET['iDisplayLength']);
    //debug($_GET);
	
	$allrecord=db_result(db_query("SELECT COUNT(*) FROM {solution}"));
	$sql="SELECT SQL_CALC_FOUND_ROWS s.sid,u.name,s.uid,s.pid,s.result,s.memory,s.time,s.language,s.code_length,s.in_date FROM {solution} s LEFT JOIN {users} u ON u.uid=s.uid WHERE 1";
	//debug($allrecord);
      // NOTE: use double %, because drupal printf()s it before sending to mysql...
	
	if (!empty($_GET['sSearch_1']))
        $sql.=" AND u.name LIKE '" . $_GET['sSearch_1']  . "%%' ";
	if($user!=0)
	{	
		$d_user = user_load(array('uid'=>$user));
		$_GET['sSearch_1'] = $d_user->name;
		$sql.=" AND u.name = '" . $_GET['sSearch_1']  . "' ";
	}
	if($pid!=0)
		$_GET['sSearch_2'] = $pid;
    if (!empty($_GET['sSearch_2']))
		$sql.=" AND  s.pid = " . $_GET['sSearch_2'];
    if($s_result!=-1)
		$_GET['sSearch_3'] = (string)$s_result;
    if ($_GET['sSearch_3']!=null)
		$sql.=" AND  s.result = " . $_GET['sSearch_3'];
	if($s_language!=-1)
		$_GET['sSearch_6'] = (string)$s_language;
	if ($_GET['sSearch_6']!=null)
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
    
    # limit is simply based on user choice
    $sql.=" LIMIT $lim_off, $lim_rowc";
	//debug($sql);
    // Query that mess finally!
    $result=db_query($sql);
	
    $language = array('G++','GCC','JAVA','Pascal','Masm32');
	$result_type = array
	(
		0 => "AC",
		1 => "PE",
		2 => "TLE",
		3 => "MLE",
		4 => "WA",
		5 => "RE",
		6 => "OLE",
		7 => "CE",
		98 => "System Error",
		99 => "Validate Error",
		10000 => "Waiting"
	);
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
		date_default_timezone_set("Asia/Chongqing");
		$row= '["' .$r['sid'].
			 '","' . "<a href=" . url('user/'.$r['uid']) . ">"  .  $r['name'] . "</a>" . 
			 '","' . $linkbeg . $r['pid'] . $linkend .
			 '","' . getResultDescript($r['result']) .
			 '","' . ($r['result']==0?$r['memory']."K":"") .
			 '","' . ($r['result']==0?$r['time'] . "MS":"") .
			 '","' . (show_code_access($r['sid'])?'<a href=' . url('code/'.$r['sid']) . '>':'') .$language[$r['language']] . (show_code_access($r['sid'])?'</a>':'') .
			 '","' . $r['code_length'] . 'b' . 
			 '","' . date('Y-m-d h:m:s',$r['in_date']) . 
			 '"]';

        print $row;
    }
    print ']}';
}
// server side processing:
function oj_problem_status($node=null)
{
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
            'sAjaxSource' => '?q=oj_problem_status.ajax/'.$node->field_pid['0']['value'],
            'aoColumns' => $colwidths,
            'bJQueryUI' => true,
			'bAutoWidth' => false,
			'iDisplayLength'=>50,
			'aLengthMenu'=>array(array(25, 50, 100, 200),array('25', '50', '100', '200')),
        )
    );
	$params['datatable_options']['aaSorting']=array(array(3,'asc'),array(2,'asc'),array(5,'asc'),array(1,'desc'));
	//debug($params);
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
	//drupal_set_html_head("<META HTTP-EQUIV=Refresh content=".variable_get('status_refresh_time',60).">");
	
	# add the themeroller widget set (bJQueryUI=true)
	drupal_add_css(drupal_get_path('module','datatables').'dataTables/media/css/demo_table_jui.css');
	drupal_add_css(drupal_get_path('module','datatables').'/theme/css/start/jquery-ui-1.8.17.custom.css');
	
	$inputboxes='<table style="border:0px;background-color:#fedc6b; float:right">
                <tr>
				<td>
				Language:<select size=1 id=search-language>
				<option value=\'\'  selected>All</option>
				<option value=0>G++</option>
				<option value=1>GCC</option>
				<option value=2>Java</option>
				<option value=3>Pascal</option>
				<option value=4>Masm32</option>
				</select>
				</td>
				</tr>
				</table>
				';
	return $inputboxes . theme_datatable($head,$dummy_row,$params);
}

function oj_problem_status_ajax($pid=0)
{
	
	$lim_off=db_escape_string($_GET['iDisplayStart']);
	$lim_rowc=db_escape_string($_GET['iDisplayLength']);
    //debug($_GET);
	
	$allrecord=db_result(db_query("SELECT COUNT(*) FROM {solution} WHERE pid=%d",$pid));
	$sql="SELECT SQL_CALC_FOUND_ROWS s.sid,u.name,s.uid,s.pid,s.result,s.memory,s.time,s.language,s.code_length,s.in_date FROM {solution} s LEFT JOIN {users} u ON u.uid=s.uid WHERE 1";
	
	$sql.=" AND s.pid=$pid AND s.result=0";
      // NOTE: use double %, because drupal printf()s it before sending to mysql...
	//if($user!=null)
	//	$_GET['sSearch_1'] = $user;
	if (!empty($_GET['sSearch_1']))
        $sql.=" AND u.name LIKE '" . $_GET['sSearch_1']  . "%%' ";
    if (!empty($_GET['sSearch_2']))
		$sql.=" AND  s.pid = " . $_GET['sSearch_2'];
   // if($s_result!=-1)
	//	$_GET['sSearch_3'] = $s_result;
    if ($_GET['sSearch_3']!=null)
		$sql.=" AND  s.result = " . $_GET['sSearch_3'];
	//if($s_language!=-1)
	//	$_GET['sSearch_6']=$s_language;
	if ($_GET['sSearch_6']!=null)
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
     
	//if($pid!=null)
	//	$sqlorderby.= $ordercomma . "s.time,s.memory,s.code_length"; 
    if(!empty($sqlorderby))
		$sql.=$sqlorderby;
    
    # limit is simply based on user choice
    $sql.=" LIMIT $lim_off, $lim_rowc";
	//debug($sql);
    // Query that mess finally!
    $result=db_query($sql);
	
    $language = array('G++','GCC','JAVA','Pascal','Masm32');
	$result_type = array
	(
		0 => "AC",
		1 => "PE",
		2 => "TLE",
		3 => "MLE",
		4 => "WA",
		5 => "RE",
		6 => "OLE",
		7 => "CE",
		98 => "System Error",
		99 => "Validate Error",
		10000 => "Waiting"
	);
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
			 '","' . date('Y-m-d h:m:s',$r['in_date']) . 
			 '"]';

        print $row;
    }
    print ']}';
}

function getResultDescript($i)
{
	if ($i == 0)
		return "<font color=red><b>Accepted</b></font>";
	if ($i == 7)
		return "<font color=#1e9e00>Compile Error</font>";
	if ($i == 2)
		return "<font color=#ff9900>Time Limit Exceed</font>";
	if ($i == 3)
		return "<font color=#0692ff>Memory Limit Exceed</font>";
	if ($i == 4)
		return "<font color=#000000>Wrong Answer</font>";
	if ($i == 5)
		return "<font color=#bb338f>Runtime Error</font>";
	if ($i == 6)
		return "<font color=#999999>Output Limit Exceed</font>";
	if ($i == 1)
		return "<font color=#ff03fa>Presentation Error</font>";
	if ($i == 10000)
		return "<font color=green>Waiting</font>";
	if ($i == 98)
		return "<font color=black>System Error</font>";
	if ($i == 99)
		return "Validate Error";
	else
		return "Other";
}

function import_solution($p1=1000,$p2=1099)
{
	if($p1<0||$p2<0||$p1>$p2)
	{
		drupal_set_message("Invalid Arguments");
		drupal_goto('status');
	}
	$start_time = time();
	if($p1==null)
	{
		$p1=1000;
		$p2=1099;
	}
	if($p2==null)
		$p2=1099;
	
	db_set_active('JudgeOnline');
	db_query("SET NAMES 'latin1'");
	
	$query="SELECT *,s.solution_id,uncompress(c.source) AS source,UNIX_TIMESTAMP(s.in_date) AS in_date FROM solution s 
	LEFT JOIN source_code c ON s.solution_id=c.solution_id 
	LEFT JOIN compileinfo e ON s.solution_id=e.solution_id 
	WHERE s.solution_id>=%d AND s.solution_id<=%d ORDER BY s.solution_id";
	$result=db_query($query,$p1,$p2);
	db_set_active('default');
	$num = 0;
	while($solution = db_fetch_object($result))
	{
		$items=db_query("SELECT sid FROM {solution} WHERE sid=%d",$solution->solution_id);
		if($item = db_fetch_object($items))
		{
			if($item->sid!=null)
				continue;
		}
		$uid = 0;
		$items=db_query("SELECT uid FROM {users} WHERE name='%s'",$solution->user_id);
		if($item = db_fetch_object($items))
		{
			$uid = $item->uid;
			//debug($item);
			//debug($solution->user_id);
		}
		
		db_query("INSERT INTO {solution} (pid,uid,time,memory,in_date,result,language,ip,num,code_length,source,error) VALUES (%d,%d,%d,%d,%d,%d,%d,'%s',%d,%d,compress('%s'),'%s')",
			$solution->problem_id,
			$uid,
			$solution->time,
			$solution->memory,
			$solution->in_date,
			$solution->result,
			$solution->language,
			$solution->ip,
			$solution->num,
			$solution->code_length,
			$solution->source,
			$solution->error
			) or drupal_set_message('Insert into database error.');

		$num++;
	}
	
	$msg = "You had import $num solution";
	if($num>1)
		$msg.="s";
	$msg.=", execution time: ".(time()-$start_time)."s.";
	drupal_set_message($msg);
	drupal_goto('status');
}

function reset_solution_access()
{
	global $user;
	if($user->uid==1)
		return true;
	return false;
}

function reset_solution()
{
	$query="DELETE FROM {solution} WHERE 1";
	db_query($query);
	$query="ALTER TABLE {solution} AUTO_INCREMENT=1000";
	db_query($query);
	
	drupal_set_message("You had delete all solutions.");
	drupal_goto('status');
}

function oj_problem_status_access($node)
{
	global $user;
	if($node->type=='problem')
		return true;
	return false;
}

function oj_problem_status_callback($node)
{
	return 'Problem Status';
}

