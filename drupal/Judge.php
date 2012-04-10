<?php
/**
 * @file
 * The PHP page that serves all page requests on a Drupal installation.
 *
 * The routines here dispatch control to the appropriate handler, which then
 * prints the appropriate page.
 *
 * All Drupal code is released under the GNU General Public License.
 * See COPYRIGHT.txt and LICENSE.txt.
 */

require_once './includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
//debug($argc,'$argc');
//debug($argv,'$argv');
array_shift($argv);
$method = array_shift($argv);
if($method == 'judge')
{
	$sid = array_shift($argv);
	if($sid == null)
	{
		return;
	}
	$judge_list = new Judge_thread($sid);
	//debug($judge_list);
}
else if($method == 'rejudge')
{
	$type = array_shift($argv);
	if($type == 'pid')
	{
		$pid = array_shift($argv);
		$results = $argv;
		debug($pid,'pid');
		debug($results,'results');
		oj_rejudge_problem_callback($pid,$result);
		return;
	}
	
	$id2 = array_shift($argv);
	
	oj_rejudge_callback($type,$id1,$id2,$result);
}
?>