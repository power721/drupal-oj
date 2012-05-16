<?php

/**
* Override or insert PHPTemplate variables into the templates.
*/
function phptemplate_preprocess_page(&$vars)
{
	$vars['oj_login_form'] = drupal_get_form('oj_login_form');
}
