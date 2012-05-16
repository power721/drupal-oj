<?php

function recent_contests()
{
	drupal_set_title(t('Recent Contests on Other OJs'));
	
	$json = @file("http://acm.nankai.edu.cn/contests.json");
	$data = json_decode($json[0],TRUE);
	
	$head=array
	(
		array('data' => 'OJ'),
		array('data' => 'Title'),
		array('data' => 'Start Time'),
		array('data' => 'Access'),
	);
	if($data)
	foreach($data as $value)
	{
        $linkbeg = '<a href="' . $value['link'] . '" target=blank>';
        $linkend = '</a>';
		
		if('Topcoder' == $value['oj'])
			$value['oj'] = '<font color=red>Topcoder</font>';
		if(strpos($value['name'],'SRM') !== FALSE)
			$value['oj'] = '<b>' . $value['oj'] . '</b>';
		
        $dummy_row[]=array
		(
			array('data' => $value['oj']),
			array('data' => $linkbeg . $value['name'] . $linkend),
			array('data' => $value['start_time'] . '  ' . $value['week']),
			array('data' => $value['access']),
		);
	}
	
	$colwidths=array
	(
		array('bSortable'=>true,'sWidth'=>'10%'),
		array('bSortable'=>true,'sWidth'=>'57%'),
		array('bSortable'=>true,'sWidth'=>'26%'),
		array('bSortable'=>false,'sWidth'=>'7%'),
	);
	
	$params=array
	(
		'datatable_options' =>array
		(
            'sDom' => '<"H"<"clear">>rt<"F"<"clear">><"clear">',
            //'bServerSide' => true,
            //'sAjaxSource' => '?q=recent_contests.ajax',
            'aoColumns' => $colwidths,
            'bJQueryUI' => true,
			'bAutoWidth' => false,
			'iDisplayLength'=>-1,
        )
    );
	$params['datatable_options']['aaSorting']=array(array(2,'asc'),array(0,'asc'),array(1,'asc'));

	drupal_add_css(drupal_get_path('module','datatables').'dataTables/media/css/demo_table_jui.css');
	drupal_add_css(drupal_get_path('module','datatables').'/theme/css/cupertino/jquery-ui-1.8.19.custom.css');

	return theme_datatable($head,$dummy_row,$params);
}

function recent_contests_cmp($a,$b)
{
	return strcmp($a['start_time'] , $b['start_time']);
}

function next_srm()
{
	if($value = get_next_srm())
	{
		print "<a href={$value['link']}><font color=blue>{$value['name']} {$value['start_time']}</font></a></br>";
	}
}

function get_next_srm()
{
	$json = @file("http://acm.nankai.edu.cn/contests.json");
	$data = json_decode($json[0],TRUE);
	if(!empty($data))
		foreach($data as $value)
		{
			if('Topcoder' == $value['oj'] && strpos($value['name'],'SRM') !== FALSE)
			{
				return $value;
			}
		}
	return FALSE;
}
