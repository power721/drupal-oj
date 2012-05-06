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
		array('bSortable'=>true,'sWidth'=>'10%'),
		array('bSortable'=>true,'sWidth'=>'30%'),
		array('bSortable'=>true,'sWidth'=>'20%'),
		array('bSortable'=>true,'sWidth'=>'20%'),
		array('bSortable'=>true,'sWidth'=>'11%'),
		array('bSortable'=>false,'sWidth'=>'11%'),
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
            'sAjaxSource' => '?q=oj_contest_list.ajax',
            'aoColumns' => $colwidths,
            'bJQueryUI' => true,
			'bAutoWidth' => false,
			'iDisplayLength'=>20,
			'aLengthMenu'=>array(array(10, 20, 50, 100),array('10', '20', '50', '100')),
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
	
	drupal_add_css(drupal_get_path('module','datatables').'dataTables/media/css/demo_table_jui.css');
	drupal_add_css(drupal_get_path('module','datatables').'/theme/css/cupertino/jquery-ui-1.8.19.custom.css');
	
	$inputboxes='<table style="border:0px;background-color:#fedc6b; float:left">
                <tr><td>Filter ID:</td><td><input id="searchid" type=text size=15></td></tr>
                <tr><td>Contest Title:</td><td><input id="searchtitle" type=text size=15></td></tr>
				</table>
				<input id="search_button" type="submit" value="Search">';
	return $inputboxes . theme_datatable($head,$dummy_row,$params);
}

function oj_contest_list_ajax()
{
	$lim_off=db_escape_string($_GET['iDisplayStart']);
	$lim_rowc=db_escape_string($_GET['iDisplayLength']);
	
	$allrecord=db_result(db_query("SELECT COUNT(*) FROM {contests}"));
	$sql="SELECT SQL_CALC_FOUND_ROWS * FROM {contests} c WHERE 1";
	
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
				case 1: $sqlorderby.= $ordercomma . "title " . $_GET["sSortDir_$s"].",cid";  
						$ordercomma=",";
						break;
				case 2: $sqlorderby.= $ordercomma . "start_time " . $_GET["sSortDir_$s"].",cid";  
						$ordercomma=",";
						break;
				case 3: $sqlorderby.= $ordercomma . "end_time " . $_GET["sSortDir_$s"].",cid";  
						$ordercomma=",";
						break;
				case 4: $sqlorderby.= $ordercomma . "access " . $_GET["sSortDir_$s"].",cid";  
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
			 '","' . $linkbeg . $r['title'] .  $linkend . 
			 '","' . date('Y-m-d H:i:s',$r['start_time']) . 
			 '","' . date('Y-m-d H:i:s',$r['end_time']) . 
			 '","' . get_contest_type($r['access']) . 
			 '","' . get_contest_status_desc($r['cid']) . 
			 '"]';

        print $row;
    }
    print ']}';
}
function oj_contest_problem_list($cid)
{
	$head=array
	(
		array('data' => ''),
		array('data' => 'ID'),
		array('data' => 'Title'),
		array('data' => 'Ratio(AC/Submit)'),
	);

	$dummy_row[]=array
	(
		array('data' => "Loading..."),
		array('data' => "Loading..."),
		array('data' => "Loading..."),
		array('data' => "Loading..."),
	);
	
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
            'sDom' 			=> '<"H"<"clear">>rt<"F"<"clear">><"clear">',
            'bServerSide' 	=> true,
            'sAjaxSource' 	=> '?q=oj_contest_problem_list.ajax/'.$cid,
            'aoColumns' 	=> $colwidths,
            'bJQueryUI' 	=> true,
			'bAutoWidth'	=> false,
			'iDisplayLength' => 30,
        )
    );
	
	drupal_add_css(drupal_get_path('module','datatables').'dataTables/media/css/demo_table_jui.css');
	drupal_add_css(drupal_get_path('module','datatables').'/theme/css/sunny/jquery-ui-1.8.17.custom.css');

	return theme_datatable($head,$dummy_row,$params);
}

function oj_contest_problem_list_ajax($cid)
{
	global $user;
	$path = url(drupal_get_path('module','oj').'/misc/images/');
	
	print '{"aaData": [ ';
    $comma = '';
	
	$items = db_query("SELECT * FROM {contest_problem} WHERE cid=%d ORDER BY num",$cid);
    while($item = db_fetch_object($items))
	{
        print $comma; $comma=',';
        $linkbeg = addslashes('<a href="' . url("contest/$cid/problem/".chr($item->num-1+ord('A'))) . '">');
        $linkend = addslashes('</a>');
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
		$title = $item->title;
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
		$valueow =  '["' . $status .
					'","' . $num .
					'","' . $linkbeg . $title . $linkend .
					'","' . $ratio .
					'"]';
        print $valueow;
    }
    print ']}';
}

function oj_contest_status($cid)
{
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
	
	drupal_add_css(drupal_get_path('module','datatables').'dataTables/media/css/demo_table_jui.css');
	drupal_add_css(drupal_get_path('module','datatables').'/theme/css/start/jquery-ui-1.8.17.custom.css');
	global $user;
	$pid = $_GET['pid'];
	if(is_numeric($pid))
		$pid = chr($pid-1+ord('A'));
	$inputboxes = '<table style="border:0px;background-color:#fedc6b; float:left">
                <tr><td>Filter User:</td><td><input id="search-user" type=text size=15></td>
                <td>Problem ID:</td><td><input id="search-pid" type=text size=10 value='.$pid.'></td>
				<td>Result:<select size=1 id=search-result><option value=>All</option><option value=0 '.(isset($_GET['result'])?'selected':'').'>Accepted</option><option value=1>Presentation Error</option><option value=2>Time Limit Exceeded</option><option value=3>Memory Limit Exceeded</option><option value=4>Wrong Answer</option><option value=5>Runtime Error</option><option value=6>Output Limit Exceeded</option><option value=7>Compile Error</option>';
	if($user->uid==1||$user->roles['5']!=NULL)
		$inputboxes .= '<option value=98>System Error</option><option value=99>Validate Error</option><option value=100>Data Error</option><option value=10000>Waiting</option>';
	
	$inputboxes .= '</select></td>
				<td>Language:<select size=1 id=search-language><option value=\'\'  selected>All</option><option value=0>G++</option><option value=1>GCC</option><option value=2>Java</option><option value=3>Pascal</option></select></td></tr>
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
        $sql .= " AND u.name LIKE '" . $_GET['sSearch_1']  . "%%' ";
	
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
		//$nick = user_load($r['uid'])->profile_nick;
		$num = db_result(db_query("SELECT num FROM {contest_problem} WHERE cid=%d AND pid=%d",$cid,$r['pid']));
        $num = chr($num-1+ord('A'));
		$linkbeg = addslashes('<a href="' . url("contest/$cid/problem/".$num) . '">');
        $linkend = addslashes('</a>');
		
		date_default_timezone_set("Asia/Chongqing");
		$row= '["' . $r['sid'] .
			 '","' . "<a href=" . url('user/'.$r['uid']) . " title=".$nick.">"  .  $r['name'] . "</a>" . 
			 '","' . $linkbeg . $num . $linkend .
			 '","' . getResultDescript($r['result']) .
			 '","' . ($r['result']==0?$r['memory']."K":"") .
			 '","' . ($r['result']==0?$r['time'] . "MS":"") .
			 '","' . (show_code_access($r['sid'])?'<a href=' . url('code/'.$r['sid']) . '>':'') .$languages[$r['language']] . (show_code_access($r['sid'])?'</a>':'') .
			 '","' . $r['code_length'] . 'b' . 
			 '","' . date('Y-m-d H:i:s',$r['in_date']) . 
			 '"]';

        print $row;
    }
    print ']}';
}

function oj_contest_problem_status($node=NULL)
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
			'aLengthMenu'=>array(array(10, 20, 50, 100),array('10', '20', '50', '100')),
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
	drupal_add_css(drupal_get_path('module','datatables').'dataTables/media/css/demo_table_jui.css');
	drupal_add_css(drupal_get_path('module','datatables').'/theme/css/start/jquery-ui-1.8.17.custom.css');
	$path = drupal_get_path('module','oj').'/misc/js/dist/';
	drupal_add_js($path.'jquery.min.js');
	drupal_add_js($path.'jquery.jqplot.min.js');
	drupal_add_js($path.'plugins/jqplot.pieRenderer.min.js');
	drupal_add_js($path.'plugins/jqplot.donutRenderer.min.js');
	drupal_add_css($path.'jquery.jqplot.min.css');
	$inputboxes='<table style="border:0px;background-color:#fedc6b; float:right">
                <tr>
				<td>
				Language:<select size=1 id=search-language>
				<option value=\'\'  selected>All</option>
				<option value=0>G++</option>
				<option value=1>GCC</option>
				<option value=2>Java</option>
				<option value=3>Pascal</option>
				</select>
				</td>
				</tr>
				</table>
				';
	
	$item = db_fetch_object(db_query("SELECT field_submit_value,field_accepted_value,
				field_submit_user_value,field_solved_value
				FROM {content_type_problem} WHERE field_pid_value=%d",$pid));
	$submit = $item->field_submit_value;
	$accepted = $item->field_accepted_value;
	$submit_user = $item->field_submit_user_value;
	$solved = $item->field_solved_value;
	$scr = "<center><div id=\"chart1\" style=\"height:330px; width:600px;\"></div></center>
			<script language='javascript'>
			$(document).ready(function(){
			  var data = [ ['Total Submissions $submit',0],
			  ['Users (Submitted) $submit_user',0],['Users (Solved) $solved',0],
			";
	$items =db_query("SELECT result,COUNT(*) as sum FROM {solution} WHERE pid=%d GROUP BY result",$pid);
	$i = 0;
	while($item =  db_fetch_object($items))
	{
		$result = getResultDescript($item->result);
		$scr .= "['<a href=\'status?pid=$pid&result={$item->result}\'>$result</a>\t{$item->sum}',{$item->sum}],";
		$i++;
	}
	//seriesColors: [ \"red\", \"#ff03fa\", \"#ff9900\", \"#0692ff\", \"#000000\", \"#bb338f\", 
    //   \"#999999\", \"#1e9e00\", \"#d8b83f\", \"#ff5800\", \"#0085cc\"],
	$scr .= "
			  ];
			  var plot1 = jQuery.jqplot ('chart1', [data], 
				{ 
					title: 'Statistics',
					seriesColors: [ \"#4bb2c5\", \"#c5b47f\", \"#EAA228\", \"#579575\", \"#839557\", \"#958c12\", 
        \"#953579\", \"#4b5de4\", \"#d8b83f\", \"#ff5800\", \"#0085cc\"],
					seriesDefaults: {
					// Make this a pie chart.
					renderer: jQuery.jqplot.PieRenderer, 
					rendererOptions: {
					  // Put data labels on the pie slices.
					  // By default, labels show the percentage of the slice.
					  showDataLabels: true
					}
				  }, 
				  legend: { show:true, location: 'e' }
				}
			  );
			});
			</script>
			";
	//debug($scr);
	return $scr.$inputboxes . theme_datatable($head,$dummy_row,$params);
}

function oj_contest_problem_status_ajax($pid=0)
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

function oj_contest_ranklist($cid)
{
	$breadcrumb = array();
	$breadcrumb[] = l('Home', '<front>');
	$breadcrumb[] = l('Contests', 'contests');
	$breadcrumb[] = l($cid, 'contest/'.$cid);
	drupal_set_breadcrumb($breadcrumb);
	drupal_set_title(t('Contest Rank'));
	
	$contest = contest_load($cid);
	//oj_contest_rank_table($cid);
	$head=array
	(
        array('data' => 'Rank'),
        array('data' => 'User ID'),
        array('data' => 'Nick Name'),
        array('data' => 'Solved'),
        array('data' => 'Penalty'),
	);
	foreach($contest->problems as $problem)
		$head[] = array('data' => '<a href=./problem/'.chr($problem['num']-1+ord('A')).'>'.chr($problem['num']-1+ord('A')).'</a>');
	
	$dummy_row=array();
	foreach($head as $r)
		$dummy_row[0][] = array('data' => "Loading...");
	
	$colwidths=array
	(
        array('bSortable'=>false,'sWidth'=>'6%'),
        array('bSortable'=>false,'sWidth'=>'24%'),
		array('bSortable'=>false,'sWidth'=>'40%'),
        array('bSortable'=>false,'sWidth'=>'10%'),
        array('bSortable'=>false,'sWidth'=>'10%'),
    );
	foreach($contest->problems as $problem)
		$colwidths[] = array('bSortable'=>false,'sWidth'=>'5%');
	
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
				'aLengthMenu'=>array(array(25, 50, 100, 200),array('25', '50', '100', '200'))
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
  drupal_add_css(drupal_get_path('module', 'datatables') .
                 'dataTables/media/css/demo_table_jui.css');

  drupal_add_css(drupal_get_path('module', 'datatables') .
                 '/theme/css/redmond/jquery-ui-1.8.17.custom.css');

  $inputboxes='<table style="border:0px;background-color:#fedc6b; float:left">
                      <tr><td>Filter User:</td>
                          <td><input id="searchml" type=text size=15></td></tr>
                      <tr><td>Nick Name:</td>
                          <td><input id="searchauth" type=text size=15></td></tr></table><input id="search_button" type="submit" value="Search"><br>
                          <p id="opInfo"></p>';     
				  
  return $inputboxes . theme_datatable($head,$dummy_row,$params);
}

function oj_contest_rank_ajax($cid)
{
	$contest = contest_load($cid);
	$lim_off=db_escape_string($_GET['iDisplayStart']);
	$lim_rowc=db_escape_string($_GET['iDisplayLength']);
    
	$allrecord=db_result(db_query("SELECT COUNT(*) FROM {contest_rank} WHERE cid=%d",$cid));
   
	$sql="
		SELECT
			SQL_CALC_FOUND_ROWS * 
		FROM
			{contest_rank} r
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
    
        $linkbeg=addslashes('<a href="' . url('user/') . $r['uid'] . '/status">');
        $linkend=addslashes('</a>');
                
        $row = '["' . $idx .
             '","' . $linkbeg . $r['name'] . $linkend .
             '","' . oj_user_nick($r['uid']) .
             '","' . $r['accepts'] .
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
      print ']}';
}

