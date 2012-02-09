<?php
echo '<pre>';

system('cd ..');

 system('dir');
//$last_line = system('mysql -u root -pswust306 drupal6 < drupal6.sql', $retval);
// Printing additional info
echo '
</pre>
<hr />Last line of the output: ' . $last_line . '
<hr />Return value: ' . $retval;
$con = mysql_connect("localhost","root","swust306");
if (!$con)
  {
  die('Could not connect: ' . mysql_error());
  }

mysql_select_db("drupal6", $con);
echo '<pre>';
$items=mysql_query("SHOW TABLES");
while($item=mysql_fetch_object($items))
	print_r($item);
mysql_close($con);
echo '</pre>';
?> 