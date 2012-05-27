<?php

function oj_contest_list()
{
	drupal_set_title(t('Contests List'));
	$head=array
	(
		array('data' => 'ID'),
		array('data' => 'Title'),
		array('data' => 'Start Time'),
		array('data' => 'End Time'),
		array('data' => 'Access'),
		array('data' => 'Status'),
	);

	$dummy_row[]=array
	(
		array('data' => "Loading..."),
		array('data' => "Loading..."),
		array('data' => "Loading..."),
		array('data' => "Loading..."),
		array('data' => "Loading..."),
		array('data' => "Loading..."),
	);
	
	$colwidths=array
	(
		array('bSortable'=>true,'sWidth'=>'10%','asSorting'=>array('desc','asc')),
		array('bSortable'=>true,'sWidth'=>'30%'),
		array('bSortable'=>true,'sWidth'=>'20%'),
		array('bSortable'=>true,'sWidth'=>'20%'),
		array('bSortable'=>true,'sWidth'=>'10%'),
		array('bSortable'=>false,'sWidth'=>'10%'),
	);
	
	$params=array
	(
		'datatable_options' =>array
		(
			'sPaginationType' => 'full_numbers',
            'sDom' => '<"H"lp<"clear">>rt<"F"ip<"clear">><"clear">',
            'bStateSave'  => true,  // needs work to remember the independent input search box values
            'sCookiePrefix' => 'pl_',
            'bProcessing' => true,
            'bServerSide' => true,
            'sAjaxSource' => 'ajax.php?q=oj_contest_list_ajax',
            'aoColumns' => $colwidths,
            'bJQueryUI' => true,
			'bAutoWidth' => false,
			'iDisplayLength' => 20,
			'aLengthMenu' => array(10, 20, 50, 100),
        )
    );
	
	$script=<<<SCR
	$(document).ready(function() 
	{
        $("#searchid").keyup( 
		function () 
		{
                t=$('#datatable-1').dataTable();
                t.fnFilter( this.value, 0 );
        });
        $("#searchtitle").keyup( 
		function () 
		{
            t=$('#datatable-1').dataTable();
            t.fnFilter( this.value, 1 );
         });
        $("#search_button").click( function ()
		{
         	t=$('#datatable-1').dataTable();
			t.fnFilter( $("#searchid").val(), 0 );
			t.fnFilter( $("#searchtitle").val(), 1 );
        });
} );
SCR;
	drupal_add_js($script,'inline');
	drupal_add_js(drupal_get_path('module','oj_contest').'/oj_contest.js');
	
	drupal_add_css(drupal_get_path('module','datatables').'/dataTables/media/css/demo_table_jui.css');
	drupal_add_css(drupal_get_path('module','oj').'/misc/jquery-ui/css/cupertino/jquery-ui-1.7.3.custom.css');
	$inputboxes='<table style="border:0px;background-color:#fedc6b; float:left">
                <tr><td>Filter ID:</td><td><input id="searchid" type=text size=15></td></tr>
                <tr><td>Contest Title:</td><td><input id="searchtitle" type=text size=15></td></tr>
				</table>
				<input id="search_button" type="submit" value="Search">
				';
	if(user_access('create contest'))
	{
		$inputboxes .= '<div style="float:right;">
						<a href="contest/add" id="add_contest" style="color:black">Add a contest</a>
						</div>';
		drupal_add_js($js,'inline');
	}
	return $inputboxes . theme_datatable($head,$dummy_row,$params);
}
/*
function oj_contest_list_ajax()
{
	$lim_off=db_escape_string($_GET['iDisplayStart']);
	$lim_rowc=db_escape_string($_GET['iDisplayLength']);
	
	$allrecord=db_result(db_query("SELECT COUNT(*) FROM {contests}"));
	$sql="SELECT SQL_CALC_FOUND_ROWS * FROM {contests} c WHERE defunct=0";
	
	if (!empty($_GET['sSearch_0']))
		$sql.=" AND cid LIKE '" . $_GET['sSearch_0'] . "%%'";
	if (!empty($_GET['sSearch_1']))
        $sql.=" AND title LIKE '" . $_GET['sSearch_1']  . "%%' ";
	
    $sqlorderby=" ORDER BY "; 
    $ordercomma="";  
	
    if ($_GET['iSortingCols'] > 0)
	{
        for($s=0; $s < $_GET['iSortingCols']; $s++)
		{
			switch($_GET["iSortCol_$s"])
			{
				case 0: $sqlorderby.= $ordercomma . "cid " . $_GET["sSortDir_$s"];
						$ordercomma=",";
						break;
				case 1: $sqlorderby.= $ordercomma . "title " . $_GET["sSortDir_$s"].",cid desc";  
						$ordercomma=",";
						break;
				case 2: $sqlorderby.= $ordercomma . "start_time " . $_GET["sSortDir_$s"].",cid desc";  
						$ordercomma=",";
						break;
				case 3: $sqlorderby.= $ordercomma . "end_time " . $_GET["sSortDir_$s"].",cid desc";  
						$ordercomma=",";
						break;
				case 4: $sqlorderby.= $ordercomma . "access " . $_GET["sSortDir_$s"].",cid desc";  
						$ordercomma=",";
						break;
				//case 5: $sqlorderby.= $ordercomma . "status " . $_GET["sSortDir_$s"].",cid";  
				//		$ordercomma=",";
				//		break;
			}
        }
    }
    
    if(!empty($sqlorderby))
		$sql.=$sqlorderby;
    
    $sql.=" LIMIT $lim_off, $lim_rowc";
    $result=db_query($sql);
	
    $filteredrec=db_result(db_query("SELECT FOUND_ROWS()"));
	
    $out= '{ "sEcho":' . $_GET['sEcho'] . ', ';
    $out.='"iTotalRecords": ' . $allrecord .', ';
    $out.='"iTotalDisplayRecords": ' . $filteredrec . ', ';
    $out.='"aaData": [ ';
    print $out;
	date_default_timezone_set("Asia/Chongqing");
    $comma="";
    while ($r = db_fetch_array($result))
	{
      	foreach ($r as $key => $value)
      	{
      		$r[$key] = trim($value);
      	}
        print $comma; $comma=",";
        $linkbeg=addslashes('<a href="' . url('contest/' . $r['cid']) . '">');
        $linkend=addslashes('</a>');   
		
		$row= '["' . $linkbeg . $r['cid'] .  $linkend . 
			 '","' . $linkbeg . addslashes($r['title']) .  $linkend . 
			 '","' . date('Y-m-d H:i:s',$r['start_time']) . 
			 '","' . date('Y-m-d H:i:s',$r['end_time']) . 
			 '","' . get_contest_type($r['access']) . 
			 '","' . get_contest_status_desc($r['cid']) . 
			 '"]';

        print $row;
    }
    print ']}';
}
*/
function oj_contest_problem_list($cid)
{
	oj_contest_password_access($cid);
	global $user;
	$path = url(drupal_get_path('module','oj').'/misc/images/');
	drupal_add_css(drupal_get_path('module','datatables').'dataTables/media/css/demo_table_jui.css');
	drupal_add_css(drupal_get_path('module','oj').'/misc/jquery-ui/css/sunny/jquery-ui-1.7.3.custom.css');

	$head=array
	(
		array('data' => ''),
		array('data' => 'ID'),
		array('data' => 'Title'),
		array('data' => 'Ratio(AC/Submit)'),
	);

	$items = db_query("SELECT * FROM {contest_problem} WHERE cid=%d ORDER BY num",$cid);
	while($item = db_fetch_object($items))
	{
		$linkbeg = '<a href="' . url("contest/$cid/problem/".chr($item->num-1+ord('A'))) . '">';
        $linkend = '</a>';
		$status = "";
		if($user->uid)
		{
			$result = db_result(db_query("SELECT min(result) AS result FROM {solution} WHERE cid=%d AND pid=%d AND uid=%d",$cid,$item->pid,$user->uid));
			if($result)
				$status = "<img src=".$path."wrong.gif />";
			else if($result != null)
				$status = "<img src=".$path."accepted.gif />";
		}
		$num = "";
		if(is_admin_login())
			$num = "<a href=".url('problem/'.$item->pid).">".$item->pid."</a> ";
		$num .= "Problem ".chr($item->num-1+ord('A'));
		$title = $item->alias;
		if(empty($title))
		{
			$problem = problem_load($item->pid);
			$title = $problem->title;
		}
		
		$accepted = db_result(db_query("SELECT COUNT(*) FROM {solution} WHERE result=0 AND cid=%d AND pid=%d",$cid,$item->pid));
		$submit = db_result(db_query("SELECT COUNT(*) FROM {solution} WHERE cid=%d AND pid=%d",$cid,$item->pid));
		$ratio = "";
		if($submit)
		{
			$ratio = sprintf("%.0f",$accepted*100/$submit);
			$ratio .= "%";
		}
		$ratio .= "(<a href=".url("contest/$cid/status/")."&pid=".chr($item->num-1+ord('A'))."&result=0".">$accepted</a>/<a href=".url("contest/$cid/status/")."&pid=".chr($item->num-1+ord('A')).">$submit</a>)";
		
		$dummy_row[] = array
		(
			array('data' => $status),
			array('data' => $num),
			array('data' => $linkbeg . $title . $linkend),
			array('data' => $ratio),
		);
	}
	if(!isset($dummy_row))
		$dummy_row[] = array('','','','',);
	
	$colwidths=array
	(
		array('bSortable'=>false,'sWidth'=>'5%'),
		array('bSortable'=>false,'sWidth'=>'18%'),
		array('bSortable'=>false,'sWidth'=>'67%'),
		array('bSortable'=>false,'sWidth'=>'10%'),
	);
	
	$params=array
	(
		'datatable_options' =>array
		(
            'sDom' => '<"H"<"clear">>rt<"F"<"clear">><"clear">',
            'aoColumns' => $colwidths,
            'bJQueryUI' => true,
			'bAutoWidth' => false,
			'aaSorting' => array(),
			'iDisplayLength'=>26,
        )
    );
	
	return theme_datatable($head,$dummy_row,$params);
}

function oj_contest_status($cid)
{
	oj_contest_password_access($cid);
	$breadcrumb = array();
	$breadcrumb[] = l('Home', '<front>');
	$breadcrumb[] = l('Contests', 'contests');
	$breadcrumb[] = l($cid, 'contest/'.$cid);
	drupal_set_breadcrumb($breadcrumb);
	
	//$item = db_fetch_object(db_query("SELECT * FROM {contests} WHERE cid=%d",$cid));
	drupal_set_title(t('Contest Status'));
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
            'sAjaxSource' => "?q=oj_contest_status.ajax/".$cid,
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
	
	//drupal_add_css(drupal_get_path('module','datatables').'dataTables/media/css/demo_table_jui.css');
	drupal_add_css(drupal_get_path('module','oj').'/misc/jquery-ui/css/start/jquery-ui-1.7.3.custom.css');

	global $user;
	$pid = $_GET['pid'];
	if(is_numeric($pid))
		$pid = chr($pid-1+ord('A'));
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
	
	$languages = oj_contest_language($cid,FALSE);
	$options = '<option value=\'\'  selected>All</option>';
	foreach($languages as $key => $value)
		$options .= "<option value=$key>$value</option>\n";
	$inputboxes .= '</select></td>
				<td>Language:<select size=1 id=search-language>'.
				$options
				.'</select></td></tr>
				</table>
				<input id="search_button" type="submit" value="Search">';
			
	return $inputboxes . theme_datatable($head,$dummy_row,$params);
}

function oj_contest_status_ajax($cid)
{
	$lim_off = db_escape_string($_GET['iDisplayStart']);
	$lim_rowc = db_escape_string($_GET['iDisplayLength']);
	
	$allrecord = db_result(db_query("SELECT COUNT(*) FROM {solution} WHERE cid=%d",$cid));
	$sql = "SELECT SQL_CALC_FOUND_ROWS s.sid,u.name,s.uid,s.pid,s.result,s.memory,s.time,s.language,s.code_length,s.in_date FROM {solution} s LEFT JOIN {users} u ON u.uid=s.uid WHERE cid=$cid";

	if(!empty($_GET['sSearch_1']))
		$uid = $_GET['sSearch_1'];
	else if($_GET['uid'])
		$uid = $_GET['uid'];
	if(!empty($uid))
		$sql .= " AND u.name LIKE '" .  $uid . "%%' ";
	
	if(!empty($_GET['sSearch_2']))
		$pid = $_GET['sSearch_2'];
	else if($_GET['pid'])
		$pid = $_GET['pid'];
	if(!empty($pid))
	{
		if(!is_numeric($pid))
			$pid = ord(strtoupper($pid[0]))-ord('A')+1;
		$pid = db_result(db_query("SELECT pid FROM {contest_problem} WHERE cid=%d AND num=%d",$cid,$pid));
	}
    if(!empty($pid))
		$sql .= " AND  s.pid LIKE '" . $pid . "%%'";
	
	if($_GET['sSearch_3']!=NULL)
		$result = $_GET['sSearch_3'];
	else if(!is_NULL($_GET['result']))
		$result = $_GET['result'];
    if(!is_NULL($result))
		$sql .= " AND  s.result = " . $result;
	
	if($_GET['sSearch_6']!=NULL)
		$language = $_GET['sSearch_6'];
	
	if(!is_NULL($language))
		$sql .= " AND  s.language = " . $language;
	
    $sqlorderby = " ORDER BY "; 
    $ordercomma = "";  
	
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
     
	$sqlorderby .= $ordercomma . "sid desc"; 
    if(!empty($sqlorderby))
		$sql.=$sqlorderby;
    
    $sql .= " LIMIT $lim_off, $lim_rowc";
   
    $items = db_query($sql);
	$languages = oj_languages(TRUE);
    
	$result_type = oj_get_status_result();
    
    $filteredrec = db_result(db_query("SELECT FOUND_ROWS()"));
	
    $out = '{ "sEcho":' . $_GET['sEcho'] . ', ';
    $out .= '"iTotalRecords": ' . $allrecord .', ';
    $out .= '"iTotalDisplayRecords": ' . $filteredrec . ', ';
    $out .= '"aaData": [ ';
    print $out;
   
    $comma = "";
    while ($r = db_fetch_array($items))
	{
      	foreach ($r as $key => $value)
      	{
      		$r[$key] = trim($value);
      	}
        print $comma; $comma = ",";
		
		$num = db_result(db_query("SELECT num FROM {contest_problem} WHERE cid=%d AND pid=%d",$cid,$r['pid']));
        $num = chr($num-1+ord('A'));
		$linkbeg = addslashes('<a href="' . url("contest/$cid/problem/".$num) . '">');
        $linkend = addslashes('</a>');
		
		$nick = oj_user_nick($r['uid']);
        $user_linkbeg = addslashes('<a href="' . url('user/') . $r['uid'] . '/status" title="'.$nick.'">');
        $user_linkend = addslashes('</a>');
		
		date_default_timezone_set("Asia/Chongqing");
		$flag = show_code_access($r['sid']);
		$row= '["' . $r['sid'] .
			 '","' . $user_linkbeg . $r['name'] . $user_linkend . 
			 '","' . $linkbeg . $num . $linkend .
			 '","' . getResultDescript($r['result']) .
			 '","' . ($r['result']==0&&$flag?$r['memory']."K":"") .
			 '","' . ($r['result']==0&&$flag?$r['time'] . "MS":"") .
			 '","' . ($flag?'<a href=' . url('code/'.$r['sid']) . '>':'') .$languages[$r['language']] . ($flag?'</a>':'') .
			 '","' . ($flag?$r['code_length'].'b':'') . 
			 '","' . date('Y-m-d H:i:s',$r['in_date']) . 
			 '"]';

        print $row;
    }
    print ']}';
}

function oj_contest_problem_status($num=NULL,$cid=NULL)
{
	oj_contest_password_access($cid);
	$breadcrumb = array();
	$breadcrumb[] = l('Home', '<front>');
	$breadcrumb[] = l('Contests', 'contests');
	$breadcrumb[] = l($cid, 'contest/'.$cid);
	$breadcrumb[] = l('problem', 'contest/'.$cid.'/problem');
	$breadcrumb[] = l($num, 'contest/'.$cid.'/problem/'.$num);
	drupal_set_breadcrumb($breadcrumb);
	drupal_set_title("Contest Problems $num Status");
	
	$num = ord($num)-ord('A')+1;
	$pid = db_result(db_query("SELECT pid FROM {contest_problem} WHERE cid=%d AND num=%d",$cid,$num));
	
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
            'sAjaxSource' => "?q=oj_contest_problem_status.ajax/$pid/$cid",
            'aoColumns' => $colwidths,
            'bJQueryUI' => true,
			'bAutoWidth' => false,
			'iDisplayLength'=>20,
			'aLengthMenu'=>array(10, 20, 50, 100),
        )
    );
	$params['datatable_options']['aaSorting']=array(array(3,'asc'),array(2,'asc'),array(5,'asc'),array(1,'desc'));
	
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
	
	//drupal_add_css(drupal_get_path('module','datatables').'dataTables/media/css/demo_table_jui.css');
	drupal_add_css(drupal_get_path('module','oj').'/misc/jquery-ui/css/start/jquery-ui-1.7.3.custom.css');
	
	$languages = oj_contest_language($cid,FALSE);
	$options = '<option value=\'\'  selected>All</option>';
	foreach($languages as $key => $value)
		$options .= "<option value=$key>$value</option>\n";
		
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
	
	$sql = "SELECT result,COUNT(*) as sum FROM {solution} WHERE pid=%d AND cid=%d GROUP BY result";
	$items =db_query($sql,$pid,$cid);
	$oj_result = oj_get_status_result();
	while($item =  db_fetch_object($items))
	{
		$data[$oj_result[$item->result]] = (int)$item->sum;
		$links[$oj_result[$item->result]] = url("contest/$cid/status",array('absolute'=>TRUE))."&pid=$num&result=".$item->result;
	}
	$scr = problem_status_generate_pie_chart_2($data,$links);
	return $scr . $inputboxes . theme_datatable($head,$dummy_row,$params);
}

function oj_contest_problem_status_ajax($pid=NULL,$cid=NULL)
{
	$lim_off=db_escape_string($_GET['iDisplayStart']);
	$lim_rowc=db_escape_string($_GET['iDisplayLength']);
	
	$allrecord=db_result(db_query("SELECT COUNT(*) FROM {solution} WHERE pid=%d AND cid=%d",$pid,$cid));
	$sql="SELECT SQL_CALC_FOUND_ROWS s.sid,u.name,s.uid,s.pid,s.result,s.memory,s.time,s.language,s.code_length,s.in_date FROM {solution} s LEFT JOIN {users} u ON u.uid=s.uid WHERE 1";
	
	$sql.=" AND s.pid=$pid AND s.result=0";
	
	if ($_GET['sSearch_6']!=NULL)
		$sql.=" AND  s.language = " . $_GET['sSearch_6'];
	$sql .= " AND cid=%d";
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
    
    if(!empty($sqlorderby))
		$sql.=$sqlorderby;
    
    $sql.=" LIMIT $lim_off, $lim_rowc";
    $result=db_query($sql,$cid);
	$language = oj_languages(TRUE);
	$result_type =oj_get_status_result();
    $filteredrec=db_result(db_query("SELECT FOUND_ROWS()"));
	
    $out= '{ "sEcho":' . $_GET['sEcho'] . ', ';
    $out.='"iTotalRecords": ' . $allrecord .', ';
    $out.='"iTotalDisplayRecords": ' . $filteredrec . ', ';
    $out.='"aaData": [ ';
    print $out;
    $comma="";
    while ($r = db_fetch_array($result))
	{
      	foreach ($r as $key => $value)
      	{
      		$r[$key] = trim($value);
      	}
        print $comma; $comma=",";
        $linkbeg=addslashes('<a href="' . url('problem/' . $r['pid']) . '">');
        $linkend=addslashes('</a>');  
		
		$nick = oj_user_nick($r['uid']);
		$flag = show_code_access($r['sid']);
		$row= '["' .$r['sid'].
			 '","' . "<a href=".url('user/'.$r['uid'])." title=".$nick.">".$r['name']."</a>" . 
			 '","' . ($r['result']==0&&$flag?$r['memory']."K":"") .
			 '","' . ($r['result']==0&&$flag?$r['time'] . "MS":"") .
			 '","' . ($flag?'<a href=' . url('code/'.$r['sid']) . '>':'') .$language[$r['language']] . ($flag?'</a>':'') .
			 '","' . ($flag?$r['code_length'].'b':'') . 
			 '","' . date('Y-m-d H:i:s',$r['in_date']) . 
			 '"]';

        print $row;
    }
    print ']}';
}

function oj_contest_ranklist($cid)
{
	$contest = contest_load($cid);
	oj_contest_password_access($cid);
	
	$breadcrumb = array();
	$breadcrumb[] = l('Home', '<front>');
	$breadcrumb[] = l('Contests', 'contests');
	$breadcrumb[] = l($cid, 'contest/'.$cid);
	drupal_set_breadcrumb($breadcrumb);
	$title = 'Contest Rank';
	if(oj_contest_rank_frozen($contest))
		$title .= '(frozen)';
	drupal_set_title($title);
	
	//oj_contest_rank_table($cid);
	$head=array
	(
        array('data' => 'Rank'),
        array('data' => 'User ID'),
       // array('data' => 'Nick Name'),
        array('data' => 'Solved'),
        array('data' => 'Penalty'),
	);
	if(!empty($contest->problems))
		foreach($contest->problems as $problem)
			$head[] = array('data' => '<a href=./problem/'.chr($problem['num']-1+ord('A')).'>'.chr($problem['num']-1+ord('A')).'</a>');
	
	$dummy_row=array();
	foreach($head as $r)
		$dummy_row[0][] = array('data' => "Loading...");
	
	$colwidths=array
	(
        array('bSortable'=>false,'sWidth'=>'8px'),
        array('bSortable'=>false,'sWidth'=>'1em'),
		//array('bSortable'=>false,'sWidth'=>'1em'),
        array('bSortable'=>false,'sWidth'=>'8px'),
        array('bSortable'=>false,'sWidth'=>'1.5em'),
    );
	if(!empty($contest->problems))
		foreach($contest->problems as $problem)
			$colwidths[] = array('bSortable'=>false,'sWidth'=>'1em');
	
	$params=array('datatable_options' =>
           array('sPaginationType' => 'full_numbers',
                'sDom' => '<"H"lp<"clear">>rt<"F"i<"clear">><"clear">',
                // 'bStateSave'  => true,  // needs work te remember the independent input search box values
                'sCookiePrefix' => 'ul_',
                'bProcessing' => true,
                'bServerSide' => true,
                'sAjaxSource' => '?q=oj_contest_rank.ajax/'.$cid,
                'aoColumns' => $colwidths,
                'bJQueryUI' => true,
				'bAutoWidth' => false,
				'iDisplayLength'=>50,
				'aLengthMenu'=>array(array25, 50, 100, 200)
			)
        );

  // script to initialize field seach box events
  $script=<<<SCR
$(document).ready(function() {
        $("#searchml").keyup( function () {
                t=$('#datatable-1').dataTable();
                t.fnFilter( this.value, 1 );
        } );
        $("#searchauth").keyup( function () {
                t=$('#datatable-1').dataTable();
                t.fnFilter( this.value, 2 );
         } );
        $("#search_button").click( function () {
         	t=$('#datatable-1').dataTable();
          t.fnFilter( $("#searchml").val(), 1 );
          t.fnFilter( $("#searchauth").val(), 2 );
        } );
} );
SCR;
  drupal_add_js($script,'inline');
  
  # add the themeroller widget set (bJQueryUI=true)
	drupal_add_css(drupal_get_path('module', 'datatables') .'dataTables/media/css/demo_table_jui.css');
	drupal_add_css(drupal_get_path('module','oj').'/misc/jquery-ui/css/redmond/jquery-ui-1.7.3.custom.css');

  $inputboxes='<table style="border:0px;background-color:#fedc6b; float:left">
                      <tr><td>Filter User:</td>
                          <td><input id="searchml" type=text size=15></td></tr>
                      <tr><td>Nick Name:</td>
                          <td><input id="searchauth" type=text size=15></td></tr></table><input id="search_button" type="submit" value="Search"><br>
                          <p id="opInfo"></p>';     
	if(is_admin_login())
		$inputboxes .= '<div style="float:right;">
						<a href="rebuild_rank">Rebuild contest rank</a>
						</div>';
  return $inputboxes . theme_datatable($head,$dummy_row,$params);
}

function oj_contest_rank_ajax($cid)
{
	$contest = contest_load($cid);
	$lim_off=db_escape_string($_GET['iDisplayStart']);
	$lim_rowc=db_escape_string($_GET['iDisplayLength']);
    
	$table = "contest_rank";
	if(oj_contest_rank_frozen($contest))
		$table = "contest_rank_temp";
	$allrecord=db_result(db_query("SELECT COUNT(*) FROM { $table} WHERE cid=%d",$cid));
   
	$sql="
		SELECT
			SQL_CALC_FOUND_ROWS * 
		FROM
			{ $table} r
		LEFT JOIN
			{users} u
		ON
			r.uid = u.uid
		WHERE
			cid = %d
		ORDER BY
			accepts DESC,penalty
		LIMIT %d,%d
		";

    $result=db_query($sql,$cid,$lim_off,$lim_rowc);

    $filteredrec=db_result(db_query("SELECT FOUND_ROWS()"));
    
      // Start constructing JSON data
      $out= '{ "sEcho":' . $_GET['sEcho'] . ', ';
      $out.='"iTotalRecords": ' . $allrecord .', ';
      $out.='"iTotalDisplayRecords": ' . $filteredrec . ', ';
      $out.='"aaData": [ ';
      print $out;
	$idx=1;
      // deliver the rows 
      $comma=""; // comma must not be written after the last row (IE)
      while ($r = db_fetch_array($result))
	  {
      	foreach ($r as $key => $value)
      	{
      		$r[$key] = trim($value);
      	}
        print $comma; $comma=",";
		
		$nick = oj_user_nick($r['uid']);
        $linkbeg = addslashes('<a href="' . url('user/') . $r['uid'] . '/status" title="'.$nick.'">');
        $link_status = addslashes('<a href="' . url("contest/$cid/status/") . '&result=0&uid=' .oj_user_name($r['uid']). '">');
	    $linkend = addslashes('</a>');
                
        $row = '["' . $idx .
             '","' . $linkbeg . $r['name'] . $linkend .
            // '","' . addslashes($nick) .
             '","' . $link_status . $r['accepts'] . $linkend .
             '","' . oj_contest_penalty($r['penalty']);
        foreach($contest->problems as $problem)
		{
			$num = chr($problem['num']-1+ord('A'));
			$str = $r[$num.'_time']?oj_contest_penalty($r[$num.'_time']):'';
			if($r[$num.'_WrongSubmits'])
				$str .= '\n(-'.$r[$num.'_WrongSubmits'].')';
			$row .= '","' . $str;
		}
        $row .=  '"]';
        print $row;
		$idx++;
      }
	/*
	$row = ',["' . ' ' .
             '","' . ' ' .
             '","' . ' ' .
             '","' . ' ' .
             '","' . ' ';
    foreach($contest->problems as $problem)
	{
		$accepted = db_result(db_query("SELECT COUNT(*) FROM {solution} WHERE result=0 AND cid=%d AND pid=%d",$cid,$problem['pid']));
		$submit = db_result(db_query("SELECT COUNT(*) FROM {solution} WHERE cid=%d AND pid=%d",$cid,$problem['pid']));
		$row .= '","' . "$accepted/$submit";
	}
    $row .=  '"]';
    print $row;
	*/
    print ']}';
}

/***** is the contest freeze board *****/
function oj_contest_rank_frozen($contest)
{
	$cur_time = time();
	$result = db_result(db_query("SELECT 1 FROM {contest_rank_temp} WHERE cid=%d LIMIT 1",$contest->cid));
	if(!$result)
		return FALSE;
	$diff = $contest->end_time - $cur_time;
	if(!is_admin_login() && $diff <= 3600 && $diff >= 0)
		return TRUE;
	return FALSE;
}
