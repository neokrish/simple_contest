<?php
// $Id: simple_contest.admin.inc,v 1.1.2.1 2010/07/09 04:36:13 4vinoth Exp $

/**
 * @file
 * simple_contest settings form.
 */
 
/**
 * simple_contest settings form.
 */
function simple_contest_admin_settings(){
	
}
 
function simple_contest_admin_settings_submit($form, &$form_state) {

  //$values = $form_state['values'];
  
  drupal_set_message('Your changes have been saved.');
  
}

/**
 * simple_contest settings form.
 */
function simple_contest_content_type_settings_form(&$form_state, $type) {

	$form_state['type'] = $type;
  
  $settings = simple_contest_get_settings($type);
  //print_r($settings);
  
  $form['simple_contest'] = array(
    '#type' => 'fieldset',
    '#title' => t('Simple contest'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => 0,
  );
	
  $form['simple_contest']['winner_selection_type'] = array(
      '#type' => 'select',
      '#title' => t('winner selection type'),
      '#default_value' => $settings['winner_selection_type'],
      '#options' => array('auto' => t('Auto'), 'manual' => t('Manual'),),
      '#required' => TRUE,
			'#description' => t('Winners selection type.'),
  ); 
	
  $options = array();
  
  $views = views_get_all_views();
  
  foreach ($views as $name => $view) {
    if ($view->disabled == 0) {
      $options[$name] = $name;
    }
  }
  
  $form['simple_contest']['view'] = array(
      '#type' => 'select',
      '#title' => t('select view for winner selection'),
      '#default_value' => $settings['view'],
      '#options' => $options,
      '#required' => TRUE,
			'#description' => t('This view will be used to finalize the winners.'),
  );
	$fields = array();
	if($settings['view']){
		$view = views_get_view($settings['view']);
		//print_r($view);
		foreach($view->display['default']->display_options['fields'] as $id=>$field){
			if($field['label']){
				$fields[$id] = t($field['label']);
			}
		}
	}else{
		$fields = array('nid' => t('Node ID'));
	}

	$form['simple_contest']['view_field'] = array(
		'#type' => 'select',
		'#title' => t('veiw field that contains the Node ID or user ID.'),
		'#default_value' => $settings['view_field'],
		'#options' => $fields,
		'#required' => TRUE,
		'#description' => t('Select ths depends on the Contest type.'),
  ); 
	
  $form['simple_contest']['results_display_view'] = array(
      '#type' => 'select',
      '#title' => t('results display view'),
      '#default_value' => $settings['results_display_view'],
      '#options' => $options,
      '#required' => TRUE,
			'#description' => t('This view will be used to display the winners.'),
  ); 
  $options = array('user' => 'user', 'node' => 'node');
  
  $form['simple_contest']['winner_type'] = array(
      '#type' => 'select',
      '#title' => t('winner type'),
      '#default_value' => $settings['winner_type'],
      '#options' => $options,
      '#required' => TRUE,
  );
	
  $form['simple_contest']['ref_field'] = array(
      '#type' => 'select',
      '#title' => t('Reference field'),
      '#default_value' => $settings['ref_field'],
      '#options' => _simple_contest_ref_fields($type),
      '#required' => TRUE,
			'#description' => t('Node reference or User reference field depends on the contest type.'),
  );
	
  $form['simple_contest']['max_winners'] = array(
      '#type' => 'select',
      '#title' => t('maximum winners'),
      '#default_value' => $settings['max_winners'],
      '#options' => array(1=>1, 2=>2, 3=>3, 4=>4, 5 => 5, 10 =>10, 15=>15, 20=>20, 25=>25, 50=>50),
  );
  
  $form['simple_contest']['date_field'] = array(
    '#type' => 'select',
    '#title' => t('date field to be used for contest'),
    '#options' => _simple_contest_date_fields($type),
    '#default_value' => $settings['date_field'],
    '#description' => t('Select a date field of this content type to use for contest.'),
    '#required' => TRUE,
  );
	

	$form['email'] = array(
    '#type' => 'fieldset',
    '#title' => t('E-mail Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 1,
  );
	$form['email']['email_to_winner'] = array(
		'#type' => 'checkbox',
		'#title' => t('E-mail to winner'),
		'#default_value' => $settings['email_to_winner'],
		'#options' => array(0 => 0, 1 => 1),
		'#required' => FALSE,
  ); 
	$form['email']['email_to_winner_subject'] = array(
		'#type' => 'textfield',
		'#title' => t('winner e-mail subject'),
		'#default_value' => $settings['email_to_winner_subject'],
  ); 
	$form['email']['email_to_winner_body'] = array(
		'#type' => 'textarea',
		'#title' => t('winner e-mail body'),
		'#default_value' => $settings['email_to_winner_body'],
		'#rows' => 10,
  ); 
	
	$form['email']['email_to_participants'] = array(
		'#type' => 'checkbox',
		'#title' => t('E-mail to participants'),
		'#default_value' => $settings['email_to_participants'],
		'#options' => array(0 => 0, 1 => 1),
		'#required' => FALSE,
  ); 
	$form['email']['email_to_participants_subject'] = array(
		'#type' => 'textfield',
		'#title' => t('participants e-mail subject'),
		'#default_value' => $settings['email_to_participants_subject'],
  ); 
	
	$form['email']['email_to_participants_body'] = array(
		'#type' => 'textarea',
		'#title' => t('participants e-mail body'),
		'#default_value' => $settings['email_to_participants_body'],
		'#rows' => 10,
  ); 
	
	$form['email']['email_to_admin'] = array(
		'#type' => 'checkbox',
		'#title' => t('E-mail to admin'),
		'#default_value' => $settings['email_to_admin'],
		'#options' => array(0 => 0, 1 => 1),
		'#required' => FALSE,
  ); 
	$form['email']['email_to_admin_subject'] = array(
		'#type' => 'textfield',
		'#title' => t('admin e-mail subject'),
		'#default_value' => $settings['email_to_admin_subject'],
  ); 
	
	$form['email']['email_to_admin_body'] = array(
		'#type' => 'textarea',
		'#title' => t('admin e-mail body'),
		'#default_value' => $settings['email_to_admin_body'],
		'#rows' => 10,
  ); 
	
	if (module_exists('token')) {
    $form['email']['token_help'] = array(
      '#title' => t('Replacement patterns'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
		);

    $form['email']['token_help']['help'] = array(
			'#value' => theme('token_help', 'simple_contest'),
		);
  }
	
	if (module_exists('date_repeat')) {
	
		module_load_include('inc', 'simple_contest', 'simple_contest.repeat');
		
		_simple_contest_date_repeat_widget($form, $form_state, $settings);
		
	}
	
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#weight' => 10,
  );
  
  return $form;
}


function bsguitar_theme_cart_review_table() {
  

  // Set up table header.
  $header = array(
    array('data' => t('Qty'), 'class' => 'qty'),
    array('data' => t('Products'), 'class' => 'products'),
    array('data' => t('Price'), 'class' => 'price'),
  );

  $context = array();
	
	$type = '';
	
  // Set up table rows.
  $contents = uc_cart_get_contents();
  foreach ($contents as $item) {
    $price_info = array(
      'price' => $item->price,
      'qty' => $item->qty,
    );

		$node = node_load($item->nid);

    $context['revision'] = 'altered';
    $context['type'] = 'cart_item';
    $context['subject'] = array(
      'cart' => $contents,
      'cart_item' => $item,
      'node' => $node,
    );

    $total = uc_price($price_info, $context);
    $subtotal += $total;

    $description = check_plain($item->title) . uc_product_get_description($item);

    // Remove node from context to prevent the price from being altered.
    $context['revision'] = 'themed-original';
    $context['type'] = 'amount';
    unset($context['subject']);
		
		if ($type != bsguitar_product_type_replace($node->type) ){ // add new header
			$type = bsguitar_product_type_replace($node->type);
			
			$rows[] = array(
				array('data' => ''),
				array('data' => $type, 'class' => 'products'),
				array('data' => ''),
			);
		}
    $rows[] = array(
      array('data' => t('@qty&times;', array('@qty' => $item->qty)), 'class' => 'qty'),
      array('data' => $description, 'class' => 'products'),
      array('data' => uc_price($total, $context), 'class' => 'price'),
    );
  }

  // Add the subtotal as the final row.
  if ($show_subtotal) {
    $context = array(
      'revision' => 'themed-original',
      'type' => 'amount',
    );
    $rows[] = array(
      'data' => array(array('data' => '<span id="subtotal-title">' . t('Subtotal:') . '</span> ' . uc_price($subtotal, $context), 'colspan' => 4, 'class' => 'subtotal')),
      'class' => 'subtotal',
    );
  }

  return theme('table', $header, $rows, array('class' => 'soccer-cart'));
}


function simple_contest_content_type_settings_form_validate($form, &$form_state) {

	module_load_include('inc', 'date_api', 'date_api_ical');	
	_simple_contest_date_repeat_widget_validate($form, &$form_state);
	
}

function simple_contest_content_type_settings_form_submit($form, &$form_state) {

  $settings = simple_contest_get_settings($form_state['type']);
	
	$settings['type'] = $form_state['type'];
	
  foreach (simple_contest_settings_info() as $setting => $default) {
    $settings[$setting] = $form_state['values'][$setting];
  }
	
	variable_set('simple_contest_' . $form_state['type'], $settings);
	
	_simple_contest_date_repeat_widget_submit($form, &$form_state);
	
  drupal_set_message('Your changes have been saved.');

}


function _simple_contest_date_fields($content_type = NULL) {
  $fields = array();
  $content_type_info = _content_type_info();
  if ($content_type_info['content types'][$content_type]) {
    foreach ($content_type_info['content types'][$content_type]['fields'] as $field) {
      if (in_array($field['type'], array('date', 'datestamp', 'datetime'))) {
        $fields[$field['field_name']] = $field['widget']['label'];
      }
    }
  }
  return $fields;
}

function _simple_contest_ref_fields($content_type = NULL) {
  $fields = array();
  $content_type_info = _content_type_info();
  if ($content_type_info['content types'][$content_type]) {
    foreach ($content_type_info['content types'][$content_type]['fields'] as $field) {
      if (in_array($field['type'], array('nodereference', 'userreference'))) {
        $fields[$field['field_name']] = $field['widget']['label'];
      }
    }
  }
  return $fields;
}


