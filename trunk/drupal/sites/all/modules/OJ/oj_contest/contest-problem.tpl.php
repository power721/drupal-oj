<?php

//NEED MODIFY

/**
 * @file contest-problem.tpl.php
 *
 * Theme implementation to display a contest problem.
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
<div id="node-<?php print $num; ?>" class="node contest-problem clear-block">
  
	
	<div class="tabs" align=center>
	<ul class="tabs secondary">
	<?php
	for($i=1;$i<=$count;$i++)
		print '<li><a href='.url("contest/$cid/problem/".chr($i-1+ord('A'))).( $i==$num?' class="active"':'').'>'.chr($i-1+ord('A'))."</a></li>\n";
	?>
	</ul>
	</div>
	
	
  <center>
  <p align=center>
  <?php print $img.$title; ?>
 <br>
 <font color=blue>Time Limit</font>:<?php print $node->field_time[0]['value'] ?>MS
 <font color=blue>Memory Limit</font>:<?php print $node->field_memory[0]['value'] ?>K<br>
 <font color=red>Total Submit</font>:<?php print $submited_link; ?>
 <font color=red>Accepted</font>:<?php print $accepted_link; ?><br>
 <?php if($node->field_ctime[0]['value']<$node->field_time[0]['value']): ?>
 <font color=green>Case Time Limit:<?php print $node->field_ctime[0]['value'] ?>MS</font><br>
 <?php endif;?>
 <font size="3" color="blue">
	[<a href=<?php print url("contest/$cid/problem/".chr($num-1+ord('A')).'/submit') ?>>Submit</a>]&nbsp;&nbsp;
	[<a href=<?php print url("contest/$cid/problem/".chr($num-1+ord('A')).'/status') ?>>Status</a>]&nbsp;&nbsp; 
	[<a href=<?php print url("contest/$cid/discuss/".chr($num-1+ord('A'))) ?>>Discuss</a>]
  </font>
 </p>
 </center>
 
  <div class="content">
	
  <fieldset class="fieldgroup field-description collapsible">
  <legend>
  Description
  </legend>
    <div class="field-items contest-problem">
    <?php print oj_check_markup($node->field_description[0]['value'],$node->field_description[0]['format']) ?>
	</div>
   </fieldset>
   
  <fieldset class="fieldgroup field-input collapsible">
  <legend>
  Input
  </legend>
    <div class="field-items contest-problem">
    <?php print oj_check_markup($node->field_input[0]['value'],$node->field_input[0]['format']) ?>
	</div>
   </fieldset>
   
   <fieldset class="fieldgroup field-output collapsible">
  <legend>
  Output
  </legend>
    <div class="field-items contest-problem">
    <?php print oj_check_markup($node->field_output[0]['value'],$node->field_output[0]['format']) ?>
	</div>
   </fieldset>
   <fieldset class="fieldgroup field-sinput collapsible">
  <legend>
  Sample Input
  </legend>
    <div class="field-items contest-problem">
	<?php print oj_check_markup($node->field_sinput[0]['value'],$node->field_sinput[0]['format']);?>
	</div>
   </fieldset>
   
   <fieldset class="fieldgroup field-soutput collapsible">
  <legend>
  Sample Output
  </legend>
    <div class="field-items contest-problem">
	<?php print oj_check_markup($node->field_soutput[0]['value'],$node->field_soutput[0]['format']);?>
	</div>
   </fieldset>
   
   <?php if($node->field_hint[0]['value']): ?>
   <fieldset class="fieldgroup field-hint collapsible">
	<legend>
	Hint
	</legend>
    <div class="field-items contest-problem">
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
	[<a href=<?php print url("contest/$cid/problem/".chr($num-1+ord('A')).'/submit') ?>>Submit</a>]&nbsp;&nbsp;
	[<a href=<?php print url("contest/$cid/problem/".chr($num-1+ord('A')).'/status') ?>>Status</a>]&nbsp;&nbsp; 
	[<a href=<?php print url("contest/$cid/discuss/".chr($num-1+ord('A'))) ?>>Discuss</a>]
	</font>
	</p>
  </div>

  <?php print $links; ?>
</div>
