<?php


// server side processing:
function oj_user_ranklist()
{
	
  $head=array(
         array('data' => 'Rank'),
         array('data' => 'User ID'),
         array('data' => 'Nick Name'),
         array('data' => 'Solved'),
         array('data' => 'Submit'),
         array('data' => 'Ratio'),
        
          );

  // one dummy row, needed for initialization
  $dummy_row=array();
  $dummy_row[]=array(
               array('data' => "Loading..."),
               array('data' => "Loading..."),
               array('data' => "Loading..."),
               array('data' => "Loading..."),
               array('data' => "Loading..."),
               array('data' => "Loading..."),
            
              );

  $colwidths=array(
                   array('bSortable'=>true,'sWidth'=>'6%'),
                   array('bSortable'=>true,'sWidth'=>'24%'),
                   array('bSortable'=>false,'sWidth'=>'40%'),
                   array('bSortable'=>true,'sWidth'=>'10%'),
                   array('bSortable'=>true,'sWidth'=>'10%'),
                   array('bSortable'=>true,'sWidth'=>'10%'),
                
                  );

  $params=array('datatable_options' =>
           array('sPaginationType' => 'full_numbers',
                 'sDom' => '<"H"lp<"clear">>rt<"F"i<"clear">><"clear">',
                 // 'bStateSave'  => true,  // needs work te remember the independent input search box values
                 'sCookiePrefix' => 'ul_',
                 'bProcessing' => true,
                 'bServerSide' => true,
                 'sAjaxSource' => '?q=oj_user_list.ajax',
                 'aoColumns' => $colwidths,
                 'bJQueryUI' => true,
				 'bAutoWidth' => false,
				'iDisplayLength'=>25,
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
function oj_user_list_ajax() {
     
	$lim_off=db_escape_string($_GET['iDisplayStart']);
	$lim_rowc=db_escape_string($_GET['iDisplayLength']);
    
	$allrecord=db_result(db_query("SELECT COUNT(*) FROM {users} WHERE uid>0"));
   
	/*$sql="SELECT SQL_CALC_FOUND_ROWS 
           u.uid,
		   u.name,
		   pv.fid,
		   pv.value
        FROM users u,
           profile_fields pf,
           profile_values pv
       WHERE u.status=1 AND pv.uid=u.uid AND pf.fid=pv.fid AND (pf.name='profile_nick' OR pf.name='profile_solved' OR pf.name='profile_submissions')";
*/
	$sql="SELECT SQL_CALC_FOUND_ROWS * FROM
		(SELECT uid,name,nick,solved,submissions,@rank:=@rank+1 AS rank FROM 
		(SELECT u.uid,u.name,pn.value AS nick,CAST(pa.value AS SIGNED) AS solved,
		CAST(ps.value AS SIGNED) AS submissions FROM users u 
		LEFT JOIN profile_values pn ON u.uid=pn.uid AND pn.fid=1 
		LEFT JOIN profile_values pa ON u.uid=pa.uid AND pa.fid=4 
		LEFT JOIN profile_values ps ON u.uid=ps.uid AND ps.fid=5 
		WHERE status=1)t,(SELECT @rank:=0) r ORDER BY solved desc,submissions)tt WHERE 1";
   
      // NOTE: use double %, because drupal printf()s it before sending to mysql...
      if (!empty($_GET['sSearch_1'])) {
        $sql.=" AND name LIKE '%%" . $_GET['sSearch_1']  . "%%' ";
      }
      if (!empty($_GET['sSearch_2'])) {
        $sql.=" AND nick LIKE '%%" . $_GET['sSearch_2']  . "%%' ";
      }
      // because of condition listing, one odering is always needed: based on nid
      // to be able to group the same notification (with just differenct conditions) together
      $sqlorderby=" ORDER BY "; 
      $ordercomma="";  
      if ($_GET['iSortingCols'] > 0) {
    
        for($s=0; $s < $_GET['iSortingCols']; $s++) {
          switch($_GET["iSortCol_$s"]) {
            case 1: $sqlorderby.= $ordercomma . "name " . $_GET["sSortDir_$s"];
                    $ordercomma=",";
                    break;
            case 3: $sqlorderby.= $ordercomma . "solved " . $_GET["sSortDir_$s"].",submissions desc";  
                    $ordercomma=",";
                    break;
            case 4: $sqlorderby.= $ordercomma . "submissions " . $_GET["sSortDir_$s"].",solved desc";  
                    $ordercomma=",";
                    break;
			case 5: $sqlorderby.= $ordercomma . "solved/submissions " . $_GET["sSortDir_$s"].",solved desc,submissions";  
                    $ordercomma=",";
                    break;
          }
        }
      }
      
      $sqlorderby.= $ordercomma . "solved desc,submissions"; // the always needed sorting
      if (!empty($sqlorderby)) {
        $sql.=$sqlorderby;
      }
    
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
                
        $row= '["' .$r['rank'] .
             '","' . $linkbeg . $r['name'] . $linkend .
             '","' . $r['nick'] .
             '","' . $r['solved'] .
             '","' . $r['submissions'] . 
             '","' . ($r['submissions']==0?0:ceil($r['solved']/$r['submissions']*100)) .'%' . 
            
             '"]';
        print $row;
		$idx++;
      }
      print ']}';
}