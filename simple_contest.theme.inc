<?php
// $Id: simple_contest.theme.inc,v 1.1.2.1 2010/07/09 04:36:13 4vinoth Exp $

/**
 * @file
 * Theme and template preprocessing code
 */


/**
 * Theme the simple contest node title
 */
function theme_simple_contest_node_title($node) {

  $settings = simple_contest_get_settings($node->type);
	$field_name = $settings['date_field'];
	$field = FALSE;
	
	$content_type_info = _content_type_info();
  if ($content_type_info['content types'][$node->type]) {
    foreach ($content_type_info['content types'][$node->type]['fields'] as $f) {
      if ($f['field_name'] == $field_name) {
				$field = $f;
        break;
      }
    }
  }
	$output = check_plain($node->title) ;
	//$output = "<div>" . check_plain($node->title) . "</div>";
	// TODO : theme the date field
	//$output .= "<div>" . theme('date_display_combination', $node) . "</div>";
	
	return $output;
}
/**
 * Theme the simple contest node title
 */
function theme_simple_contest_on_participant_node($node) {

	$content['contest'] = array(
    '#type' => 'fieldset',
    '#title' => t('Contest'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
	$content['contest']['node'] = array(
		'#value' => "<h2>" . check_plain($node->title) . "</h2>" . node_show($node, TRUE, FALSE),
	);
	$output = drupal_render($content);

	return $output;
}

/**
 * Theme the simple contest results_display
 */
function theme_simple_contest_results_display($node, $results) {

	$content['contest'] = array(
    '#type' => 'fieldset',
    '#title' => t('Contest winners'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
	$content['contest']['winners'] = array(
		'#value' => "<h2>" . check_plain('Winner(s)') . "</h2>" . $results,
	);
	$output = drupal_render($content);

	return $output;
}


/**
 * Provide the ability to select items in a view using checkboxes.
 */			 
function theme_simple_contest_views_selector($element) {
  require_once(drupal_get_path('module', 'views').'/theme/theme.inc');
  
  drupal_add_js('misc/tableselect.js');
  //drupal_add_js(drupal_get_path('module', 'views_bulk_operations').'/views_bulk_operations.js');
  //drupal_add_css(drupal_get_path('module', 'views_bulk_operations') . '/views_bulk_operations.css', 'module');

  $output = '';
  $view = $element['#view'];
  $sets = $element['#sets'];
  $vars = array(
    'view' => $view,
  );
  // Give each group its own headers row.
  foreach ($sets as $title => $records) {
    $headers = array();

    // template_preprocess_views_view_table() expects the raw data in 'rows'.
    $vars['rows'] = $records;
      
    // Render the view as table. Function from views/theme/theme.inc
    template_preprocess_views_view_table($vars);
    
    // Add checkboxes to the header and the rows.
    $hide_select_all = @$view->display['default']->display_options['style_options']['hide_select_all'];
    if (!$hide_select_all) {
      $headers[] = theme('table_select_header_cell');
    }
    else {
      $headers[] = array('class' => 'no_select_all');
    }
    foreach ($vars['header'] as $field => $label) {
      $headers[] = array('data' => $label, 'class' => "views-field views-field-{$vars['fields'][$field]}");
    }
    $rows = array();
    foreach ($records as $num => $object) {
      $row = array('class' => 'rowclick', 'data' => array());
      $row['data'][] =  theme('checkbox', $element['selection'][$num + 1]);
      foreach ($vars['rows'][$num] as $field => $content) {
        $row['data'][] = array('data' => $content, 'class' => "views-field views-field-{$vars['fields'][$field]}");
      }
      $rows[] = $row;
    }
    // Add the first row as option to select all records across all pages.
    if (isset($view->query->pager) && $view->total_rows > $view->get_items_per_page()) {
      $group = count($sets) > 1 ? t('set') : t('page');
      $row = array(
        array(
          'data' => '<span id="vbo-this-page">'. t('All <strong>!objects</strong> rows in this !group are selected.', array('!objects' => count($records), '!group' => $group))
            .'&nbsp;<input type="button" id="vbo-select-all-pages" value="'. t('Select all !objects rows in this view.', array('!objects' => $view->total_rows)) .'" /></span>'
            .'<span id="vbo-all-pages" style="display: none">'. t('All <strong>!objects</strong> rows in this view are selected.', array('!objects' => $view->total_rows))
            .'&nbsp;<input type="button" id="vbo-select-this-page" value="'. t('Select only !objects rows in this !group.', array('!objects' => count($records), '!group' => $group)) .'" /></span>',
          'class' => 'view-field view-field-select-all',
          'colspan' => count($headers) + 1,
        ),
      );
      array_unshift($rows, $row);
    }

    $output .= theme('table', $headers, $rows, array('class' => $vars['class']), $title);
    $output .= theme('hidden', $element['select_all']);
  }
  return theme('form_element', $element, $output);
}

