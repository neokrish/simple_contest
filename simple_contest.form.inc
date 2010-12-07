<?php
// $Id: simple_contest.form.inc,v 1.1.2.1 2010/07/09 04:36:13 4vinoth Exp $
/**
 * @file
 * 
 * simple_contest.form.inc
 */
 

 
 /**
 * Implementation of hook_form_alter().
 */
function _simple_contest_form_alter(&$form, $form_state, $form_id) {

  if ($form_id == 'node_type_form') {
    
    $form['simple_contest'] = array(
      '#type' => 'fieldset',
      '#title' => t('Simple contest'),
      '#collapsible' => TRUE,
			'#collapsed' => TRUE,
      '#weight' => 32,
    );
    $form['simple_contest']['simple_contest'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use this content type as a Simple contest.'),
      '#default_value' => simple_contest_is_simple_contest($form['#node_type']->type),
      '#description' => t('additional configurations can be found in "Simple contest" tab.'),
      '#return_value' => array(),
    );
    
  }
	
  if (!empty($form['#node']->type) && simple_contest_is_contest_submission_type($form['#node']->type)) {
		
		$node = $form['#node'];
		
		// TODO : type should be dynamic
    $nids = simple_contest_get_active_contests('contest');
		
		/*if (empty($nids)){
			return;
		}
		*/
		
		$form['simple_contest'] = array(
			'#type' => 'fieldset',
			'#title' => t('Contest'),
			'#collapsible' => FALSE,
			'#collapsed' => FALSE,
			'#tree' => TRUE,
		);
		
		$options = array();
		foreach($nids as $nid){
			$options[$nid] = theme('simple_contest_node_title', node_load($nid));
		}
		$cnid = '';
		if(isset($node->simple_contest['contest_nid'])){
			$cnid = $node->simple_contest['contest_nid'];
		}
		if (!isset($options[$cnid]) && $cnid>0){
			$options[$cnid] = theme('simple_contest_node_title', node_load($cnid));
		}
		
		$form['simple_contest']['contest_nid'] = array(
			'#type' => 'radios',
			'#title' => t('Choose the contest'),
			'#options' => $options,
			'#default_value' => $cnid,
		);
		$form['simple_contest']['contest_nid_old'] = array(
			'#type' => 'hidden',
			'#value' => $cnid,
		);
		
  }
	
	if (!empty($form['#node']->type) && simple_contest_is_simple_contest($form['#node']->type)) {
		
		$node = $form['#node'];
		
		$form['simple_contest'] = array(
			'#type' => 'fieldset',
			'#title' => t('Contest'),
			'#collapsible' => FALSE,
			'#collapsed' => FALSE,
		);
		
		$options = array(
			SIMPLE_CONTEST_STATUS_INACTIVE => t('Disabled'), 
			SIMPLE_CONTEST_STATUS_ACTIVE => t('Active'),
			SIMPLE_CONTEST_STATUS_CANCELED => t('Canceled'),
			SIMPLE_CONTEST_STATUS_COMPLETED => t('Completed'),
		);
		
		$form['simple_contest']['status'] = array(
			'#type' => 'radios',
			'#title' => t('Contest status'),
			'#options' => $options,
			'#default_value' => $node->simple_contest['status'],
		);
		$form['simple_contest'] += array(
        '#weight' => module_exists('content') ? content_extra_field_weight($node->type, 'simple_contest') : 1,
        '#tree' => TRUE,
      );
  }
}