<?php
// $Id: simple_contest.token.inc,v 1.1.2.1 2010/07/09 04:36:13 4vinoth Exp $
/**
 * @file
 * 
 * simple_contest.token.inc
 */
 

/**
 * Implementation of hook_token_values().
 */
function simple_contest_token_values($type = 'all', $object = NULL) {
  $values = array();
  if ($type == 'simple_contest' && is_object($object)) {
		$values = array();
		
		token_include();
		
		$settings = $object['settings'];
		$values["max-winners"]  = $settings['max_winners'];
		$values["winner-type"]  = $settings['winner_type'];
		$values["participant-rank"]  = $object['participant_rank'];
		
		// contest_node
		$contest_node = $object['contest_node'];
    $node_values = node_token_values('node', $contest_node);
		foreach($node_values as $key => $value){
			$values["contest-node-$key"]  = $value;
		}
		
		// contest participant node
		$contest_participant_node = $object['contest_participant_node'];
    $node_values = node_token_values('node', $contest_participant_node);
		foreach($node_values as $key => $value){
			$values["participant-node-$key"]  = $value;
		}
		
		// contest participant node
		$contest_participant_user = $object['contest_participant_user'];
    $user_values = user_token_values('user', $contest_participant_user);
		foreach($user_values as $key => $value){
			$values["participant-user-$key"]  = $value;
		}
		
  }
  return $values;
}

/**
 * Implementation of hook_token_list().
 */
function simple_contest_token_list($type = 'all') {
  if ($type == 'simple_contest' || $type == 'all') {
		
		$tokens['simple_contest']['participant-rank'] = t('Participant Rank');
		$tokens['simple_contest']['max-winners'] = t('Maxnum winners');
		$tokens['simple_contest']['winner-type'] = t('Winner type [node, user].');
		
		token_include();
		
		// contest_node
		$contest_node = $object['contest_node'];
    $node_values = node_token_list('node');
		foreach($node_values['node'] as $key => $value){
			$tokens['simple_contest']["contest-node-$key"]  = $value;
		}
		
		// contest participant node
		$participant_node = $object['participant_node'];
    $node_values = node_token_list('node');
		foreach($node_values['node'] as $key => $value){
			$tokens['simple_contest']["participant-node-$key"]  = $value;
		}
		
		// contest participant node
		$contest_participant_user = $object['participant_user'];
    $user_values = user_token_list('user');
		foreach($user_values['user'] as $key => $value){
			$tokens['simple_contest']["participant-user-$key"]  = $value;
		}
  }
	return $tokens;
}

