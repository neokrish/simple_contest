<?php
//$Id: simple_contest.repeat.inc,v 1.1.2.1 2010/07/09 04:36:13 4vinoth Exp $
/**
 * @file
 * Implementation of Date Repeat API for the contest.
 * mostly adopted from date/date/date_repeat.inc, thanks to karens.
 */

 
/**
 * Widget processing for date repeat form element.
 *
 */
function _simple_contest_date_repeat_widget(&$element, $form_state, $settings) {

	$type = $form_state['type'];

	$desc = t("You can set repeat rules below to schedule the creation of Contests automatically. ");
	
	$element['schedule'] = array(
		'#type' => 'fieldset',
		'#title' => t('Contest auto create scheduler'),
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
		'#description' => $desc,
	);
		
	$element['schedule']['auto_create'] = array(
		'#type' => 'checkbox',
		'#title' => t('Enable contest auto create using repeat rule.'),
		'#default_value' => $settings['auto_create'],
		'#options' => array(0 => 0, 1 => 1),
		'#required' => FALSE,
	); 
	
	// time zone
	$tz = '';
	if($settings['tz_handling'] != ''){
		$tz = date_get_timezone_db($settings['tz_handling']);
	}
	
  $element['schedule']['rrule'] = array(
		'#title' => t('Repeat rule'),
    '#type' => 'date_repeat_rrule',
    '#default_value' => isset($settings['rrule']['rrule']) ? $settings['rrule']['rrule'] : '',
    '#date_timezone' => $tz,
    '#date_format'      => date_limit_format('Y-m-d', array('year', 'month', 'day')),
    '#date_text_parts'  => array(),
    '#date_increment'   => 1,
    '#date_year_range'  => '-3:+3',
    '#date_label_position' => 'within',
    '#prev_value' => isset($settings['rrule']['value']) ? $settings['rrule']['value'] : '',
    '#prev_value2' => isset($settings['rrule']['value2']) ? $settings['rrule']['value2'] : '',
    '#prev_rrule' => isset($settings['rrule']['rrule']) ? $settings['rrule']['rrule'] : '',
    '#date_repeat_widget' => module_exists('date_popup') ? 'date_popup' : 'date_select',
    '#date_repeat_collapsed' => FALSE,
  );
		
	$options = array(
		'site' => t("Site's time zone"),
		'date' => t("Date's time zone"),
		'user' => t("User's time zone"),
		'utc' => 'UTC',
		'' => t('No time zone conversion'),
	);
	
	$element['schedule']['tz_handling'] = array(
		'#type' => 'select',
		'#title' => t('Time zone handling'),
		'#default_value' => $settings['tz_handling'],
		'#options' => $options,
		'#description' => t('Select the timezone handling method to be used for this date.'),
	);
	
	$def_node = simple_contest_get_def_node($type, TRUE); // always refresh here?
	$element['schedule']['contest_defaults_nid'] = array(
    '#value' => $def_node->nid,
    '#type' => 'hidden',
  );
	
	$element['schedule']['defaults_nid_desc'] = array(
		//'#type' => 'markup',
    '#title' => t('Contest defaults node'),
    '#value' => t("Default node is the *Unpublished* template node which will be used when creating scheduled contests. The default node for this contest is !node.", array('!node' => l($def_node->title, 'node/' . $def_node->nid))),
  );
	
  return $element;
}


/**
 * Validation for date repeat form element.
 *
 * Create multiple values from the RRULE results.
 * Lots more work needed here.
 */
function _simple_contest_date_repeat_widget_validate($element, &$form_state) {

	$form_values = $form_state['values'];
		
	if(!$form_values['auto_create']){
		return;
	}
	
  require_once( drupal_get_path('module', 'date_repeat') .'/date_repeat_form.inc');
	module_load_include('inc', 'date_api', 'date_api_ical');
		
	$form_state['values']['rrule'] = $form['#post']['schedule']['rrule'];
	$values = $form['#post']['schedule']['rrule'];
	

	
  // If no repeats are set, clean up the form and return.
  if ($form_values['FREQ'] == 'NONE') {    
    return;
  }

  // Require the UNTIL date for now.
  // The RRULE has already been created by this point, so go back
  // to the posted values to see if this was filled out.
  $error_field = implode('][', $element['#parents']) .'][rrule][UNTIL][datetime][date';
  if (empty($form_values['UNTIL']['datetime'])) {
    form_set_error($error_field, t('The UNTIL value is required for repeating dates.'));
  }
  if (form_get_errors()) {
    return;
  }

  // If the rule, the start date, or the end date have changed, re-calculate
  // the repeating dates, wipe out the previous values, and populate the
  // field with the new values.

  // TODO
  // Is it right to not do anything unless there are changes? Will that
  // confuse anyone? Commenting that out for now...

	/*
	$item = array();
	
	// Avoid undefined index problems on dates that don't have all parts.
	
	foreach (array('value', 'value2', 'timezone', 'offset', 'offset2') as $key) {
		 $item[$key] = '';
	}
	*/
	/*$value = date_repeat_build_dates($rrule, $values, $field, $item);
	form_set_value($element, $value, $form_state);
	*/
	
	$rrule = date_api_ical_build_rrule($form_values);
	$form_state['values']['rrule'] = date_ical_parse($rrule);
	$form_state['values']['rrule']['rrule'] = $rrule;
	
}

/**
 * Validation for date repeat form element.
 *
 * Create multiple values from the RRULE results.
 * Lots more work needed here.
 */
function _simple_contest_date_repeat_widget_submit($element, &$form_state) {

	$form_values = $form_state['values'];
	
	
	// delete all nulls for this contest type
	$sql = "DELETE FROM {simple_contest_repeat} 
		WHERE type = '%s' AND nid IS NULL ";
	db_query($sql, $form_state['type']);	
	
	if(!$form_values['auto_create']){
		return;
	}
	
  module_load_include('inc', 'date_repeat', 'date_repeat_form');
	module_load_include('inc', 'date_api', 'date_api_ical');
	
	$rrule = $form_state['values']['rrule']['rrule'];
	
	//$values -- from & to date?
	$type = $settings['type'];
	$settings = simple_contest_get_settings($type);
	$fields = content_fields();
	$field = $fields[$settings['date_field']];
	$values = $form_values;
	simple_contest_date_repeat_build_dates($rrule, $settings, $values, $field, $item);
	
}

/**
 * Helper function to build repeating dates from a $node_field.
 * 
 * Pass in either the RRULE or the $form_values array for the RRULE,
 * whichever is missing will be created when needed.
 */
function simple_contest_date_repeat_build_dates($rrule, $settings, $values, $field, $item) {
	include_once('./'. drupal_get_path('module', 'date_api') .'/date_api_ical.inc');
  $field_name = $field['field_name'];

  // By the time we get here, the start and end dates have been 
  // adjusted back to UTC, but we want localtime dates to do
  // things like '+1 Tuesday', so adjust back to localtime.
	
	$tz = $settings['tz_handling']=='' ? 'none' : $settings['tz_handling'];
	
  $timezone = date_get_timezone($tz, '');
	
  $timezone_db = date_get_timezone_db($tz);
	// start date is now ??
  //$start = date_make_date($item['value'], $timezone_db, $field['type'], $field['granularity']);
	$start = date_make_date(time(), $timezone_db, $field['type'], $field['granularity']);
	
  if ($timezone != $timezone_db){
    date_timezone_set($start, timezone_open($timezone));
  }
	
	$interval = max(1, isset($values['INTERVAL']) ? $values['INTERVAL'] : 1);
	
  switch ($values['FREQ']) {
   case 'DAILY':
     $jump = $interval .' days';
     break;
   case 'WEEKLY':
     $jump = $interval .' weeks';
     break;
   case 'MONTHLY':
     $jump = $interval .' months';
     break;
   case 'YEARLY':
     $jump = $interval .' years';
     break;
  }
	
	$end = drupal_clone($start);
	date_modify($start, $jump);
  
  $duration = date_difference($start, $end);
  $start_datetime = date_format($start, DATE_FORMAT_DATETIME);
  
  if (!empty($values['UNTIL']['datetime'])) {
    $end = date_ical_date($values['UNTIL'], $timezone);
  }
  $end_datetime = date_format($end, DATE_FORMAT_DATETIME);
  
  // Split the RRULE into RRULE and EXDATE parts.
  $parts = date_repeat_split_rrule($rrule);
  $parsed_exceptions = (array) $parts[1];
  $exceptions = array();
  foreach ($parsed_exceptions as $exception) {
    $date = date_ical_date($exception);
    $exceptions[] = date_format($date, 'Y-m-d');
  }
  
	// only the future date?
  //$dates = date_repeat_calc($rrule, $start_datetime, $end_datetime, $exceptions, $timezone);
	$dates = date_repeat_calc($rrule, $start_datetime, $end_datetime, $exceptions, $timezone);
	
	$sql = "INSERT INTO {simple_contest_repeat} (type, start_date, end_date) VALUES  ('%s', '%s', '%s')";
	
  $value = array();
  foreach ($dates as $delta => $date) {
		
    // date_repeat_calc always returns DATE_DATETIME dates, which is
    // not necessarily $field['type'] dates.
    // Convert returned dates back to db timezone before storing.
    $date_start = date_make_date($date, $timezone, DATE_DATETIME, $field['granularity']);  
    date_timezone_set($date_start, timezone_open($timezone_db));    
    $date_end = drupal_clone($date_start);
    date_modify($date_end, '+'. $duration .' seconds');
    $value[$delta] = array(
      'value' => date_format($date_start, date_type_format($field['type'])),
      'value2' => date_format($date_end, date_type_format($field['type'])),
      'offset' => date_offset_get($date_start),
      'offset2' => date_offset_get($date_end),
      //'timezone' => $timezone,
      //'rrule' => $rrule,
    );
		
		// insert into simple_contest_repeat
		db_query($sql, $settings['type'], $value[$delta]['value'], $value[$delta]['value2']);
		
  }
  return $value;
}

