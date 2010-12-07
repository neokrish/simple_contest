<?php
// $Id: simple_contest.pages.inc,v 1.1.2.1 2010/07/09 04:36:13 4vinoth Exp $

/**
 * @file
 * user pages for simeple contest
 */


/**
 * simple_contest_finalize_page
 */
function simple_contest_finalize_page($node) {
	$output = array();
	
	drupal_set_title(t('Choose winners for @title', array('@title' => check_plain($node->title))));
	
	if (!simple_contest_is_simple_contest($node)){
		return $output;
	}
	
  $settings = simple_contest_get_settings($node->type);
	$status = $node->simple_contest['status'];
	
	// status
	$output[] = array(
		'#value' => "<h2>" . t('Contest status') . "</h2>" . t(simple_contest_status_name($status)), 
		'#weight' => 0,
	);
	
	
	// winners
	if($node->simple_contest['status'] == SIMPLE_CONTEST_STATUS_COMPLETED){
		$output[] = array(
			'#value' => simple_contest_node_winners_themed($node), 
			'#weight' => 1,
		);
	}
	// winners
	//if($node->simple_contest['status'] == SIMPLE_CONTEST_STATUS_ACTIVE){
		$output[] = array(
			'#value' => simple_contest_choose_winners_form($node), 
			'#weight' => 2,
		);
	//}
	return drupal_render($output);
}


function simple_contest_choose_winners_form($node){
	
  $settings = simple_contest_get_settings($node->type);
  //print_r($settings);
  
	$view = $settings['view'];
	//print($view);
	
	return views_embed_view($view, 'default', $node->nid);
	
  /*$form['simple_contest_choose'] = array(
    '#type' => 'fieldset',
    '#title' => t('Choose winners'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => 0,
  );
	
	$form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Finalize'),
    '#weight' => 10,
  );
	*/
	
	//return $form;
}


function simple_contest_choose_winners_form_validate($form, &$form_state) {
	
}

function simple_contest_choose_winners_form_submit($form, &$form_state) {
	
}