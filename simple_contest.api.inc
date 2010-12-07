<?php
// $Id: simple_contest.api.inc,v 1.1.2.1 2010/07/09 04:36:13 4vinoth Exp $


function simple_contest_finalize_results_all(){

	$fields = content_fields();
	
  $today = date('Y-m-d', time());
	
  foreach (simple_contest_get_types() as $type => $info) {
    
		$settings = simple_contest_get_settings($type);
		
		if(empty($settings['date_field'])){
			watchdog('simple_contest', 'Date field for the contest %type is not set.'  , array('%type' => $type),  WATCHDOG_WARNING);
			continue;
		}
		
		// process only for automatic winner selection types
		if($settings['winner_selection_type'] != 'auto'){
			continue;
		}
		
		$field = $fields[$settings['date_field']];
		$db_info = content_database_info($field);
		$table = $db_info['table'];
		$column_name = '';
		
		foreach ($db_info['columns'] as $column_name => $attributes) {
			$column_name = $db_info['columns'][$column_name]['column'];
			break;
		}
		
		$column_name2 = $column_name . '2'; // todate column from date module
		
		$sql = "SELECT n.nid, " . $column_name .", " . $column_name2 ."  
			FROM {node} n 
			LEFT JOIN " . $table . " ct ON n.nid = ct.nid AND n.vid = ct.vid 
			LEFT JOIN {simple_contest_statistics} sc ON n.nid = sc.nid 
			WHERE n.status <> 0 AND n.type = '%s' AND  
				DATE_FORMAT(" . $column_name2 .", '%Y-%m-%d') < '" . $today . "' 
				AND sc.status = %d
			GROUP BY n.nid ORDER BY " . $column_name ." 
			";
		
		$res = db_query($sql, $type, SIMPLE_CONTEST_STATUS_ACTIVE);
		
		while($row = db_fetch_object($res)){
		
			simple_contest_finalize_results_node(node_load($row->nid));
			
		}
			
  }
}

function simple_contest_finalize_results_node(&$node, $settings = NULL){
	
	$nid = $node->nid;
	$type = $node->type;
	
	if(!$settings){
		$settings = simple_contest_get_settings($type);
	}
		
	$view = views_get_view($settings['view']);
	
	if(!$view){
		watchdog('simple_contest', 'Cannot find the view %view for the contest %type.'  , array('%view' => $settings['view'], '%type' => $type),  WATCHDOG_WARNING);
		continue 2;
	}

	$args = array($nid, $node->{$settings['date_field'][0]['value']}, $node->{$settings['date_field'][0]['value2']}); // views must have two atleast null arguments

	$view->init_display();
	$view->display_handler->set_option('items_per_page', $values['max_winners']);
	$view->display_handler->set_option('offset', 0);
	$view->preview('default', $args);

	$winners = array();

	if (empty($view->result) || !is_array($view->result)) {
		return $items;
	}

	$count = 0;
	$field = ($settings['winner_type'] == 'user') ? 'uid' : 'nid';
	foreach ($view->result as $i => $row) {
		if(!isset($row->$field)){
			watchdog('simple_contest', 'The view must have the field ' . $field . ' to select winenr for the contest ' . $type,  WATCHDOG_WARNING);
			break;
		}
		$winners[] = $row->$field;
		if($count >= $settings['max_winners']){
			break;
		}
		
	}
	
	simple_contest_save_result($node, $winners);
	
}

function simple_contest_save_result(&$node, $winners){

	global $language;
	$settings = simple_contest_get_settings($node->type);
	
	// delete the old
	$sql = "DELETE FROM {simple_contest_results} WHERE nid = %d";
	db_query($sql, $node->nid);
	
	$sql = "UPDATE {simple_contest_statistics} SET status = %d, winners_count = %d, timestamp = %d WHERE nid = %d";
	db_query($sql, SIMPLE_CONTEST_STATUS_COMPLETED, count($winners), time(), $node->nid);
	
	$new_winners = array();
	
	if ($settings['winner_type'] == 'user'){ //
		$weight = 0;
		foreach($winners as $uid){ // to keep the weight			
			$new_winners[$uid] = array('nid' => NULL, 'weight' => $weight);
			$weight++; //rank
		}
	}else if ($settings['winner_type'] == 'node'){
		// find the user ids
		$nids = implode(',', $winners);
		$sql = 'SELECT nid, uid FROM {node} where nid in (' . $nids . ')';
		$res = db_query($sql);
		
		while($row = db_fetch_object($res)){
			$tmp_winners[$row->nid] = $row->uid;
		}
		$weight = 0;
		foreach($winners as $nid){ // to keep the weight
			$uid = 0; 
			if(isset($tmp_winners[$nid])){
				$uid = $tmp_winners[$nid];
			}
			$new_winners[$uid] = array('nid' => $nid, 'weight' => $weight);
			$weight++; //rank
		}
	}
	
	drupal_alter('simple_contest_results', $node, $new_winners);
	
	$sql = "INSERT INTO {simple_contest_results} (nid, uid, wnid, weight) VALUES (%d, %d, %d, %d)";
	foreach($new_winners as $uid => $info){
		db_query($sql, $node->nid, $uid, $info['nid'], $info['weight']);
	}
	
	//invoke hooks
	module_invoke_all('simple_contest', $node, 'completed', $new_winners);
	
	if($settings['email_to_winner']){
		foreach($new_winners as $uid => $info){
		
			$user = user_load(array('uid' => $uid));
			if ($info['nid']){
				$participant_node = node_load($info['nid']);
			}else{
				$participant_node = NULL;
			}
			
			$params = array(
				'contest_node' => $node, 
				'participant_user' => $user, 
				'participant_node' => $participant_node, 
				'settings' => $settings, 
				'participant_rank' => $info['weight'], 
			);
			
			drupal_mail('simple_contest', 'winners_mail', $user->mail, $language, $params);
			
		}
		
	}
	
	if($settings['email_to_participants']){
		
		$ref_field = $settings['ref_field'];
		
		$participants = $node->$ref_field;
		
		foreach($participants as $delta => $field){
			
			if ($settings['winner_type'] == 'user'){
				
				if(isset($new_winners[$field['uid']])){ // is winner
					continue;
				}
				
				$user = user_load(array('uid' => $field['uid']));
				$participant_node = NULL;
			}else{
				$participant_node = node_load($field['nid']);
				
				if(isset($new_winners[$participant_node->uid])){ // is winner
					continue;
				}
				
				$user = user_load(array('uid' => $participant_node->uid));
			}
			
			$params = array(
				'contest_node' => $node, 
				'participant_user' => $user, 
				'participant_node' => $participant_node, 
				'settings' => $settings, 
				'participant_rank' => -1, // TODO: fix
			);
			
			drupal_mail('simple_contest', 'winners_mail_participants', $user->mail, $language, $params);
		}
	}
	
	if($settings['email_to_admin']){
		$params = array(
			'contest_node' => $node, 
			'settings' => $settings, 
		);
		
		//E-mail to admin
		$admin = user_load(array('uid' => 1));
		drupal_mail('simple_contest', 'admin_winners_mail', $admin->mail, $language, $params);
	}
	
}

function simple_contest_get_node_result($nid){
	$results = array();
	
	$sql = "SELECT * FROM {simple_contest_results} WHERE nid = %d ORDER BY weight";
	$res = db_query($sql, $nid);
	while($row = db_fetch_object($res)){
		$results[$row->uid] = array('nid' => $row->nid, 'weight' => $row->weight);
	}
	
	return $results;
	
}


function simple_contest_repeat_create_contests(){

	foreach (simple_contest_get_types() as $type => $info) {
    
		$settings = simple_contest_get_settings($type);
		
		if ($settings['auto_create'] == 1 && !empty($settings['rrule']) && $settings['contest_defaults_nid']){
			
			/*
			'rrule' => '',									// repeat rule
			'auto_create' => 0,							// repeat rule enabled?
			'tz_handling' => '',						// time zone handling none, site, UTC, user
			'contest_defaults_nid' => '',
			*/
			
			$limit = 1;
			
			$timezone = variable_get('date_default_timezone', 0);
			
			$nextday = date('Y-m-d', strtotime("+1 day"));
			
			$sql = "
				SELECT * FROM {simple_contest_repeat} 
				WHERE nid IS NULL AND type = '%s' AND DATE_FORMAT(start_date, '%Y-%m-%d') <= '". $nextday ."' 
				ORDER BY start_date ASC 
				LIMIT %d ";
			
			$res = db_query($sql, $type, $limit);
			while($row = db_fetch_object($res)){
			
				$def_node = node_load($settings['contest_defaults_nid']);
				$date_field = $settings['date_field'];
				
				if($def_node){
				
					unset($def_node->nid);
					
				}else{
					watchdog('simple_contest', 'Cannot find the default node for %type. Please goto the simple contest admin page and save again.', array('%type' => $type),  WATCHDOG_WARNING);
					$def_node = new stdClass();
					$def_node->title = "Contest - !start - !end";
					$def_node->type = $type;
				}
				
				$def_node->title = t($def_node->title, array('!start' => $row->start_date, '!end' => $row->end_date));
				$def_node->is_new = TRUE;
				$def_node->status = 1; // published
				
				$def_node->{$date_field}[0]['value'] = $row->start_date;
				$def_node->{$date_field}[0]['value2'] = $row->end_date;
				
				node_save($def_node); // save the new node
				
				db_query("UPDATE {simple_contest_repeat} SET nid = %d 
					WHERE rid = %d " , $row->rid);
			}
		}
	}
	
}


function simple_contest_get_active_contests($ctype, $refresh = FALSE){
	static $contests = array();
	
	if (!simple_contest_is_simple_contest($ctype)){
		return array();
	}
	
	if ($refresh){
		unset($contests[$ctype]);
	}
	
	if (isset($contests[$ctype])){
		return $contests[$ctype];
	}
	
	$sql = "SELECT scs.nid FROM {simple_contest_statistics} scs 
		INNER JOIN {node} n ON n.nid = scs.nid 
		WHERE scs.status = 1 AND n.status <> 0 AND n.type = '%s' ORDER BY n.title 
	";
	
	$results = array();
	
	$res = db_query($sql, $ctype);
	while($row = db_fetch_array($res)){
		$results[$row['nid']] = $row['nid']; // node load ?
	}
	
	$contests[$ctype] = $results; // save to static
	
	return $results;
	
	/*
	$type_where = '';	
	if (empty($ctype)){
		$tmps = [];
		foreach ($types as $type => $info) {
			//$settings = simple_contest_get_settings($type);
			$tmps[]= "'$type'";
		}
		$type_where = " AND n.type IN (" . implode(',', $tmps) . ") ";
	}else{
		
	}
	*/
	
}

/*
 * default node template
 */
function simple_contest_get_def_node($type, $refresh){
	static $def_nodes = array();
	
	if (empty($type)){
		return;
	}
	
	if($refresh && isset($def_nodes[$type])){
		unset($def_nodes[$type]);
	}
	
	if(isset($def_nodes[$type]) && $def_nodes[$type]){
		return $def_nodes[$type];
	}
	
	$def_node = FALSE;
	$settings = simple_contest_get_settings($type);
	
	if ($settings['contest_defaults_nid']){
		$def_node = node_load($settings['contest_defaults_nid']);		
	}
	
	if (!$def_node){
	
		$def_node = new stdClass();
		$def_node->is_new = TRUE;
		$def_node->type = $type;
		$def_node->title = "Contest - !start - !end"; // will be replaced on automatic contest creation
		$def_node->status = 0; // unpublished
		
		node_save($def_node); // save it
		
		$def_node = node_load($def_node->nid); // refresh? 
		$settings['contest_defaults_nid'] = $def_node->nid;
		variable_set('simple_contest_' . $type, $settings);
		
	}
	
	$def_nodes[$type] = $def_node;
	
	return $def_node;
}

function simple_contest_referrers($nid,  $fieldname = '', $nodetypes = array(), $order = 'DESC') {
  if ($nodetypes) {
    $filter_nodetypes = "AND n.type IN ('" . implode("', '", $nodetypes) . "')";
  }
  else {
    $filter_nodetypes = '';
  }
  $fields = content_fields();

  switch ($order) {
    case 'TITLE_ASC':
      $order = 'n.title ASC';
      break;

    case 'TITLE_DESC':
      $order = 'n.title DESC';
      break;

    case 'ASC':
    case 'CREATED_ASC':
      $order = 'n.created ASC';
      break;

    default :
    case 'DESC':
    case 'CREATED_DESC':
      $order = 'n.created DESC';
      break;
  }

  $values = array();
	$db_info = content_database_info($fields[$fieldname]);

	$query = "SELECT       n.nid, n.vid, n.title
							FROM         {" . $db_info['table'] . "} nr
							INNER JOIN   {node} n ON n.vid = nr.vid AND n.status = 1 " . $filter_nodetypes . "
							WHERE        nr." . $db_info['columns']['nid']['column'] . " = %d
							ORDER BY     " . $order;
	
	$query = db_rewrite_sql($query);
	$result = db_query($query, $nid);

	while ($value = db_fetch_array($result)) {
		// avoid duplicate referrers by using nid as key
		$values[$value['nid']] = $value;
	}

  return $values;
}

function simple_contest_status_name($status){
	$options = array(
		SIMPLE_CONTEST_STATUS_INACTIVE => 'Disabled', 
		SIMPLE_CONTEST_STATUS_ACTIVE => 'Active',
		SIMPLE_CONTEST_STATUS_CANCELED => 'Canceled',
		SIMPLE_CONTEST_STATUS_COMPLETED => 'Completed',
	);
	if (!isset($options[$status])){
		return '';
	}
	return $options[$status]; //  t() not used.
	
}

function simple_contest_node_winners_themed($node){
	if(!isset($node->simple_contest['status'])){
		return '';
	}
	$settings = simple_contest_get_settings($node->type);
	if($node->simple_contest['status'] == SIMPLE_CONTEST_STATUS_COMPLETED){
		$results_display_view = $settings['results_display_view'];
		$results = views_embed_view($results_display_view, 'default', $node->nid);
		
		return theme('simple_contest_results_display', $node, $results);
		
	}
	
	return '';
}