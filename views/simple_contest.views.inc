<?php
// $Id: simple_contest.views.inc,v 1.1.2.1 2010/07/09 04:36:13 4vinoth Exp $




/**
 * Implementation of hook_views_data().
 */
function simple_contest_views_data() {

	$data = array();
  
  $data += simple_contest_data_table_simple_contest_statistics();
	$data += simple_contest_data_table_simple_contest_results();

  return $data;
}


// ---------- Table simple_contest_statistics

function simple_contest_data_table_simple_contest_statistics() {
  $data['simple_contest_statistics']['table']['group']  = t('Simple contest statistics');

  $data['simple_contest_statistics']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
  );
	
  $data['simple_contest_statistics']['nid'] = array(
    'title' => t('Contest'),
    'help' => t('Contest node.'),
    'field' => array(
      'handler' => 'views_handler_field_node',
    ),
    'relationship' => array(
      'title' => t('Contest node'),
      'help' => t("Bring in information about the Contest node."),
      'base' => 'node',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Contest node'),
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
      'allow empty' => TRUE, 
      'help' => t('filter for specific Contest.'),
    ),
    'argument' => array(
      'name' => t('winners in specified contest'),
      'name field' => 'title', // the field to display in the summary.
      'validate type' => 'nid',
      'handler' => 'views_handler_argument_node_nid',
      'parent' => 'views_handler_argument_numeric', // make sure parent is included
      'help' => t('winners  filtered for specified contest.'),
    ),
  );
  $data['simple_contest_statistics']['winners_count'] = array(
    'title' => t('Contest Winners count'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
      'help' => t('Number of winners choosen for this contest.'),
    ),
		'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
		'sort' => array(
      'handler' => 'views_handler_sort',
    ),

  );
	
  $data['simple_contest_statistics']['participants_count'] = array(
    'title' => t('Contest participants count'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
      'help' => t('Number of participants on this contest.'),
    ),
		'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
		'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
	
  $data['simple_contest_statistics']['status'] = array(
    'title' => t('Contest status'),
    'help' => t('Contest status.'),
    'real field' => 'status',
    'field' => array(
      'handler' => 'views_handler_field_simple_contest_statistics_status',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_simple_contest_statistics_status',
    ),
		'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
	
  $data['simple_contest_statistics']['timstamp'] = array(
    'title' => t('Contest timstamp'),
    'help' => t('Contest timstamp when the results were calculated.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );
	
  return $data;
}

// ------------ Table simple_contest_results

function simple_contest_data_table_simple_contest_results() {
  $data['simple_contest_results']['table']['group']  = t('Simple contest results');
  $data['simple_contest_results']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
		'users' => array(
      'left_field' => 'uid',
      'field' => 'uid',
    ),
  );

  $data['simple_contest_results']['nid'] = array(
    'title' => t('Contest'),
    'help' => t('Contest node.'),
    'field' => array(
      'handler' => 'views_handler_field_node',
    ),
    'relationship' => array(
      'title' => t('Contest node'),
      'help' => t("Bring in information about the Contest node."),
      'base' => 'node',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Contest node'),
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
      'allow empty' => TRUE, 
      'help' => t('filter for specific Contest.'),
    ),
    'argument' => array(
      'name' => t('winners in specified contest'),
      'name field' => 'title', // the field to display in the summary.
      'validate type' => 'nid',
      'handler' => 'views_handler_argument_node_nid',
      'parent' => 'views_handler_argument_numeric', // make sure parent is included
      'help' => t('winners  filtered for specified contest.'),
    ),
  );
	
  $data['simple_contest_results']['wnid'] = array(
    'title' => t('Winner Nid'),
    'help' => t('The node ID of the winner.'),
    'field' => array(
      'handler' => 'views_handler_field_node',
    ),
		'relationship' => array(
      'title' => t('winner node'),
      'help' => t("Bring in information about the winner node."),
      'base' => 'node',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('winner node'),
    ),
  );

	$data['simple_contest_results']['uid'] = array(
    'title' => t('Winner Uid'),
    'help' => t('The user ID of the winner.'),
    'field' => array(
      'handler' => 'views_handler_field_user',
    ),
		'relationship' => array(
      'title' => t('winner user'),
      'help' => t("Bring in information about the winner user."),
      'base' => 'users',
      'field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('winner user'),
    ),
		'argument' => array(
      'name' => t('winner user of the contest.'),
      'name field' => 'name', // the field to display in the summary.
      'validate type' => 'uid',
      'handler' => 'views_handler_argument_users_uid',
      'parent' => 'views_handler_argument_numeric', // make sure parent is included
      'help' => t('filter for specified user.'),
    ),
		'filter' => array(
      'handler' => 'views_handler_filter_numeric',
      'numeric' => TRUE,
      'allow empty' => TRUE, 
      'help' => t('filter for specific Contest.'),
    ),
  );
	
  $data['simple_contest_results']['weight'] = array(
    'title' => t('weight/rank of the winner'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
      'help' => t('weight/rank of the winner(1 based).'),
    ),
		'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
		'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  return $data;
}


/**
 * Implementation of hook_views_handlers().
 */
function simple_contest_views_handlers() {
  return array(
		'info' => array(
      'path' => drupal_get_path('module', 'simple_contest') .'/views',
    ),
    'handlers' => array(
		
      'views_handler_field_simple_contest_statistics_status' => array(
        'parent' => 'views_handler_field',
      ),
			
      'views_handler_filter_simple_contest_statistics_status' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),

    ),
  );
}


function simple_contest_views_plugins() {
  return array(
		
    'style' => array(
      'simple_contest_selector' => array(
        'title' => t('Simple contest selector'),
        'help' => t('Displays rows with checkboxes to choose winners.'),
        'handler' => 'simple_contest_plugin_style',
        'parent' => 'table',
        'uses row plugin' => FALSE,
        'uses fields' => TRUE,
        'uses options' => FALSE,
        'type' => 'normal',
				'path' => drupal_get_path('module', 'simple_contest') . '/views',
        //'theme' => 'simple_contest_views_selector',
				'theme' => 'views_view_bulk',
        'even empty' => FALSE,
				'theme file' => 'simple_contest.theme.inc',
        'theme path' => drupal_get_path('module','simple_contest')."/",
      ),
    ),
  );
}