<?php

/**
 * @file node-problem.tpl.php
 *
 * Theme implementation to display a node.
 *
 * Available variables:
 * - $title: the (sanitized) title of the node.
 * - $content: Node body or teaser depending on $teaser flag.
 * - $picture: The authors picture of the node output from
 *   theme_user_picture().
 * - $date: Formatted creation date (use $created to reformat with
 *   format_date()).
 * - $links: Themed links like "Read more", "Add new comment", etc. output
 *   from theme_links().
 * - $name: Themed username of node author output from theme_username().
 * - $node_url: Direct url of the current node.
 * - $terms: the themed list of taxonomy term links output from theme_links().
 * - $submitted: themed submission information output from
 *   theme_node_submitted().
 *
 * Other variables:
 * - $node: Full node object. Contains data that may not be safe.
 * - $type: Node type, i.e. story, page, blog, etc.
 * - $comment_count: Number of comments attached to the node.
 * - $uid: User ID of the node author.
 * - $created: Time the node was published formatted in Unix timestamp.
 * - $zebra: Outputs either "even" or "odd". Useful for zebra striping in
 *   teaser listings.
 * - $id: Position of the node. Increments each time it's output.
 *
 * Node status variables:
 * - $teaser: Flag for the teaser state.
 * - $page: Flag for the full page state.
 * - $promote: Flag for front page promotion state.
 * - $sticky: Flags for sticky post setting.
 * - $status: Flag for published status.
 * - $comment: State of comment settings for the node.
 * - $readmore: Flags true if the teaser content of the node cannot hold the
 *   main body content.
 * - $is_front: Flags true when presented in the front page.
 * - $logged_in: Flags true when the current user is a logged-in member.
 * - $is_admin: Flags true when the current user is an administrator.
 *
 * @see template_preprocess()
 * @see template_preprocess_node()
 */
?>
<?php drupal_add_js('misc/collapse.js'); ?>
<div id="node-<?php print $node->nid; ?>" class="node<?php if ($sticky) { print ' sticky'; } ?><?php if (!$status) { print ' node-unpublished'; } ?> clear-block">

<?php print $picture ?>

<?php if (!$page): ?>
  <h2><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>
<?php endif; ?>

  <div class="meta">
  <?php if ($submitted): ?>
    <span class="submitted"><?php print $submitted ?></span>
  <?php endif; ?>

  <?php if ($terms): ?>
    <div class="terms terms-inline"><?php print $terms ?></div>
  <?php endif;?>
  </div>
  
	<table width=100% style="border-collapse:separate;">
	<td align=left width=50%><?php print get_previous_problem($node->field_pid[0]['value']) ?></td>
	<td align=right width=50%><?php print get_next_problem($node->field_pid[0]['value']) ?></td>
	</table>

  <center>
  <p align=center>
  <?php 
	$result = get_user_problem_status($node->field_pid[0]['value']);
	$path = url(drupal_get_path('module','oj').'/misc/images/');
	if($result==1) print "<img title=accept src=".$path."accepted.gif />";
	else if($result==-1) print "<img title=wrong src=".$path."wrong.gif />";
  ?>
  <?php print $node->field_pid[0]['value'] ?>:
  
 <a href=<?php print url('problem/'.$node->field_pid[0]['value']) ?>><font color=blue><?php print $node->title ?></font></a>
 <br>
 <font color=blue>Time Limit</font>:<?php print $node->field_time[0]['value'] ?>MS
 <font color=blue>Memory Limit</font>:<?php print $node->field_memory[0]['value'] ?>K<br>
 <font color=red>Total Submit</font>:<a href=<?php print url('status').'?pid='.$node->field_pid[0]['value'] ?>><?php print $node->field_submit[0]['value'] ?></a>
 <font color=red>Accepted</font>:<a href=<?php print url('status').'?pid='.$node->field_pid[0]['value'].'&result=0' ?>><?php print $node->field_accepted[0]['value'] ?></a><br>
 <?php if($node->field_ctime[0]['value']!=30000): ?>
 <font color=green>Case Time Limit:<?php print $node->field_ctime[0]['value'] ?>MS</font><br>
 <?php endif;?>
 </p>
 </center>
 
  <div class="content">
	
  <fieldset class="fieldgroup field-description collapsible">
  <legend>
  Description
  </legend>
    <div class="field-items problem">
    <?php print oj_check_markup($node->field_description[0]['value'],$node->field_description[0]['format']) ?>
	</div>
   </fieldset>
   
  <fieldset class="fieldgroup field-input collapsible">
  <legend>
  Input
  </legend>
    <div class="field-items problem">
    <?php print oj_check_markup($node->field_input[0]['value'],$node->field_input[0]['format']) ?>
	</div>
   </fieldset>
   
   <fieldset class="fieldgroup field-output collapsible">
  <legend>
  Output
  </legend>
    <div class="field-items problem">
    <?php print oj_check_markup($node->field_output[0]['value'],$node->field_output[0]['format']) ?>
	</div>
   </fieldset>
   <fieldset class="fieldgroup field-sinput collapsible">
  <legend>
  Sample Input
  </legend>
    <div class="field-items problem">
	<?php print oj_check_markup($node->field_sinput[0]['value'],$node->field_sinput[0]['format']);?>
	</div>
   </fieldset>
   
   <fieldset class="fieldgroup field-soutput collapsible">
  <legend>
  Sample Output
  </legend>
    <div class="field-items problem">
	<?php print oj_check_markup($node->field_soutput[0]['value'],$node->field_soutput[0]['format']);?>
	</div>
   </fieldset>
   
   <?php if($node->field_hint[0]['value']): ?>
   <fieldset class="fieldgroup field-hint collapsible">
	<legend>
	Hint
	</legend>
    <div class="field-items problem">
    <?php print oj_check_markup($node->field_hint[0]['value'],$node->field_hint[0]['format']) ?>
	</div>
   </fieldset>
   <?php endif;?>
   
   <?php if($node->field_source[0]['value']): ?>
   <fieldset class="fieldgroup field-hint collapsible">
	<legend>
	Source
	</legend>
    <div class="field-itemsitems problem">
    <a href=<?php  print url('search/node/'.$node->field_source[0]['value']) ?>><?php print $node->field_source[0]['value'] ?></a>
	</div>
   </fieldset>
   <?php endif;?>
   <p align="center"><font size="4" color="blue">
	<?php if($user->uid): ?>
	[<a href=<?php print url('problem/'.$node->field_pid[0]['value'].'/submit') ?>>Submit</a>]&nbsp;&nbsp;
	<?php endif; ?>
	[<a href="javascript:history.go(-1)">Go Back</a>]&nbsp;&nbsp; 
	[<a href=<?php print url('problem/'.$node->field_pid[0]['value'].'/status') ?>>Status</a>]&nbsp;&nbsp; 
	[<a href=<?php print url('problem/'.$node->field_pid[0]['value'].'/discuss') ?>>Discuss</a>]
	</font></p>
  </div>

  <?php print $links; ?>
</div>
