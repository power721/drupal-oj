<?php

// server side processing:
function oj_problem_list()
{
	$head=array
	(
		array('data' => 'ID'),
		array('data' => 'Title'),
		array('data' => 'AC'),
		array('data' => 'Submit'),
		array('data' => 'Ratio'),
		array('data' => 'Date'),
		//array('data' => 'Admin'),
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
		//array('data' => "Loading..."),
	);

	$colwidths=array
	(
        array('bSortable'=>true,'sWidth'=>'10%'),
        array('bSortable'=>false,'sWidth'=>'60%'),
        array('bSortable'=>true,'sWidth'=>'10%'),
        array('bSortable'=>true,'sWidth'=>'10%'),
        array('bSortable'=>true,'sWidth'=>'10%'),
        array('bSortable'=>true,'sWidth'=>'20%'),
        /*array('bSortable'=>true),
        array('bSortable'=>false),
        array('bSortable'=>true),
        array('bSortable'=>true),
        array('bSortable'=>true),
        array('bSortable'=>true),*/
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
            'sAjaxSource' => '?q=oj_problem_list.ajax',
            'aoColumns' => $colwidths,
            'bJQueryUI' => true,
			'bAutoWidth' => false,
			'iDisplayLength'=>25,
			'aLengthMenu'=>array(array(25, 50, 100, 200),array('25', '50', '100', '200'))
        )
    );

	// script to initialize field seach box events
	$script=<<<SCR
$(document).ready(function() 
	{
        $("#searchid").keyup
		( 
			function () 
			{
				t=$('#datatable-1').dataTable();
				t.fnFilter( this.value, 0 );
			}
		);
        $("#searchtitle").keyup
		( 
			function () 
			{
				t=$('#datatable-1').dataTable();
				t.fnFilter( this.value, 1 );
			}
		);
        $("#search_button").click( function ()
		{
         	t=$('#datatable-1').dataTable();
			t.fnFilter( $("#searchid").val(), 0 );
			t.fnFilter( $("#searchtitle").val(), 1 );
        });
} );
SCR;
	drupal_add_js($script,'inline');
  
	# add the themeroller widget set (bJQueryUI=true)
	drupal_add_css(drupal_get_path('module','datatables').'dataTables/media/css/demo_table_jui.css');
	drupal_add_css(drupal_get_path('module','oj').'/misc/jquery-ui/css/sunny/jquery-ui-1.7.3.custom.css');
	
	$inputboxes='<table style="border:0px;background-color:#fedc6b; float:left">
                <tr><td>Filter ID:</td><td><input id="searchid" type=text size=15></td></tr>
                <tr><td>Problem Title:</td><td><input id="searchtitle" type=text size=15></td></tr>
				</table>
				<input id="search_button" type="submit" value="Search">';         
	return $inputboxes . theme_datatable($head,$dummy_row,$params);
}

function oj_problem_list_ajax()
{
	$lim_off=db_escape_string($_GET['iDisplayStart']);
	$lim_rowc=db_escape_string($_GET['iDisplayLength']);
    
	$allrecord=db_result(db_query("SELECT COUNT(*) FROM {content_type_problem}"));
   
	$sql="SELECT SQL_CALC_FOUND_ROWS * FROM {node} n LEFT JOIN {content_type_problem} p ON n.nid=p.nid WHERE n.type='problem' AND n.status=1";
   
      // NOTE: use double %, because drupal printf()s it before sending to mysql...
    if (!empty($_GET['sSearch_0']))
		$sql.=" AND p.field_pid_value LIKE '" . $_GET['sSearch_0'] . "%%'";
    if (!empty($_GET['sSearch_1']))
        $sql.=" AND n.title LIKE '%%" . $_GET['sSearch_1']  . "%%' ";
     
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
				case 0: $sqlorderby.= $ordercomma . "field_pid_value " . $_GET["sSortDir_$s"];
						$ordercomma=",";
						break;
				case 2: $sqlorderby.= $ordercomma . "field_accepted_value " . $_GET["sSortDir_$s"];  
						$ordercomma=",";
						break;
				case 3: $sqlorderby.= $ordercomma . "field_submit_value " . $_GET["sSortDir_$s"];  
						$ordercomma=",";
						break;
				case 4: $sqlorderby.= $ordercomma . "field_accepted_value/field_submit_value " . $_GET["sSortDir_$s"];  
						$ordercomma=",";
						break;
				case 5: $sqlorderby.= $ordercomma . "changed " . $_GET["sSortDir_$s"];  
						$ordercomma=",";
						break;
			}
        }
    }
      
    $sqlorderby.= $ordercomma . "field_pid_value "; // the always needed sorting
    if(!empty($sqlorderby))
		$sql.=$sqlorderby;
    
    # limit is simply based on user choice
    $sql.=" LIMIT $lim_off, $lim_rowc";
	
    // Query that mess finally!
    $result=db_query($sql);
    
    // Query the rows counted without the limit
    $filteredrec=db_result(db_query("SELECT FOUND_ROWS()"));

    // Start constructing JSON data
    $out= '{ "sEcho":' . $_GET['sEcho'] . ', ';
    $out.='"iTotalRecords": ' . $allrecord .', ';
    $out.='"iTotalDisplayRecords": ' . $filteredrec . ', ';
    $out.='"aaData": [ ';
    print $out;
	date_default_timezone_set("Asia/Chongqing");
    // deliver the rows 
    $comma=""; // comma must not be written after the last row (IE)
    while ($r = db_fetch_array($result))
	{
      	foreach ($r as $key => $value)
      	{
      		$r[$key] = trim($value);
      	}
		$node = problem_load($r['field_pid_value']);
		if(function_exists('contest_problem_access')&&!contest_problem_access($node))
			continue;
        print $comma; $comma=",";
        $linkbeg=addslashes('<a href="' . url('problem/' . $r['field_pid_value']) . '">');
        $linkend=addslashes('</a>'); 
		$ups = get_user_problem_status($r['field_pid_value']);
		$path = url(drupal_get_path('module','oj').'/misc/images/');
		$img = "";
		if($ups==1)
			$img = "<img title=accept src={$path}accepted.gif />";
		else if($ups==-1)
			$img = "<img title=wrong src={$path}wrong.gif />";
        $row= '["'.$r['field_pid_value'].$img.
             '","'.$linkbeg . $r['title'].$linkend.
             '","'."<a href=".url('status')."?pid={$r['field_pid_value']}&result=0>".$r['field_accepted_value']."</a>".
             '","'."<a href=".url('status')."?pid={$r['field_pid_value']}>".$r['field_submit_value']."</a>".
             '","'.($r['field_submit_value']==0?0:ceil($r['field_accepted_value']/$r['field_submit_value']*100)).'%'. 
             '","'.date('Y-m-d',$r['changed']). 
             '"]';
        print $row;
    }
    print ']}';
}