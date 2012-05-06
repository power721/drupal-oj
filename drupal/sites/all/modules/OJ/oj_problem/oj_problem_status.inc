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
	$url = "?q=oj_status.ajax";
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
                <tr><td>Filter User:</td><td><input id="search-user" type=text size=15 value='.$_GET['uid'].'></td>
                <td>Problem ID:</td><td><input id="search-pid" type=text size=10 value='.$_GET['pid'].'></td>
				<td>Result:<select size=1 id=search-result><option value=>All</option><option value=0 '.(isset($_GET['result'])&&$_GET['result']==0?'selected':'').'>Accepted</option><option value=1>Presentation Error</option><option value=2>Time Limit Exceeded</option><option value=3>Memory Limit Exceeded</option><option value=4>Wrong Answer</option><option value=5>Runtime Error</option><option value=6>Output Limit Exceeded</option><option value=7>Compile Error</option>';
	if($user->uid==1||$user->roles['5']!=NULL)
		$inputboxes .= '<option value=98>System Error</option><option value=99>Validate Error</option><option value=100>Data Error</option><option value=10000>Waiting</option>';
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
	$inputboxes .= '</select></td>
				<td>Language:<select size=1 id=search-language><option value=>All</option><option value=0>G++</option><option value=1>GCC</option><option value=2>Java</option><option value=3>Pascal</option></select></td></tr>
				</table>
				<input id="search_button" type="submit" value="Search">';
			
	return $inputboxes . theme_datatable($head,$dummy_row,$params);
}

function oj_status_ajax()
{
	$lim_off=db_escape_string($_GET['iDisplayStart']);
	$lim_rowc=db_escape_string($_GET['iDisplayLength']);
	
	$allrecord=db_result(db_query("SELECT COUNT(*) FROM {solution}"));
	$sql="SELECT SQL_CALC_FOUND_ROWS s.sid,u.name,s.uid,s.pid,s.result,s.memory,s.time,s.language,s.code_length,s.in_date FROM {solution} s LEFT JOIN {users} u ON u.uid=s.uid WHERE 1";
    // NOTE: use double %, because drupal printf()s it before sending to mysql...
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
    // Query that mess finally!
    $items=db_query($sql);
	$languages = oj_languages(TRUE);
    //$languages = array('G++','GCC','JAVA','Pascal','Masm32');
	$result_type = oj_get_status_result();
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
    while ($r = db_fetch_array($items))
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
			 '","' . (show_code_access($r['sid'])?'<a href=' . url('code/'.$r['sid']) . '>':'') .$languages[$r['language']] . (show_code_access($r['sid'])?'</a>':'') .
			 '","' . $r['code_length'] . 'b' . 
			 '","' . date('Y-m-d H:i:s',$r['in_date']) . 
			 '"]';

        print $row;
    }
    print ']}';
}
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

function import_solution($p1=1000,$p2=1099)
{
	if($p1<0||$p2<0||$p1>$p2)
	{
		drupal_set_message("Invalid Arguments");
		drupal_goto('status');
	}
	$start_time = time();
	
	set_time_limit(0);
	if($p1==NULL)
	{
		$p1=1000;
		$p2=1099;
	}
	if($p2==NULL)
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
		$items=db_query("SELECT sid FROM {solution} WHERE pid=%d AND uid=%d AND in_date=%d",$solution->problem_id,$solution->user_id,$solution->in_date);
		if($item = db_fetch_object($items))
		{
			if($item->sid!=NULL)
				continue;
		}
		$uid = 0;
		$items=db_query("SELECT uid FROM {users} WHERE name='%s'",$solution->user_id);
		if($item = db_fetch_object($items))
		{
			$uid = $item->uid;
		}
		$language = array(0,1,3,2,4);
		db_query("INSERT INTO {solution} (pid,uid,time,memory,in_date,result,language,ip,num,code_length,source,error) VALUES (%d,%d,%d,%d,%d,%d,%d,'%s',%d,%d,compress('%s'),'%s')",
			$solution->problem_id,
			$uid,
			$solution->time,
			$solution->memory,
			$solution->in_date,
			$solution->result,
			$language[$solution->language],
			$solution->ip,
			$solution->num,
			$solution->code_length,
			phpcharset($solution->source,'UTF-8'),
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

