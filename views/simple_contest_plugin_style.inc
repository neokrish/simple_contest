<?php
// $Id: simple_contest_plugin_style.inc,v 1.1.2.1 2010/07/09 04:36:37 4vinoth Exp $

// Thanks to views bulk operations

class simple_contest_plugin_style extends views_plugin_style_table {

  function option_definition() {
    $options = parent::option_definition();
  
    return $options; 
  }
	
	
  function render() {
    // We build the groups here to pass them to the node_selector function through the form.
    $sets = $this->render_grouping($this->view->result, $this->options['grouping']);
    $this->sets = $sets;

    // Append suffix to avoid clashing between multiple VBOs on same page.
    static $form_suffix;
    if (isset($form_suffix)) {
      $form_suffix++;
    }
    else {
      $form_suffix = 1;
    }
    return drupal_get_form('simple_contest_views_selector_form__' . $form_suffix, $this);
  }
	
  function strip_view() {
    if (isset($this->view->query->pager)) {
      $this->view->pager_original = $this->view->query->pager;
      $this->view->query->pager = NULL;
    }
    $this->view->exposed_widgets = NULL;
    $this->view->display_handler->set_option('header', '');
    $this->view->display_handler->set_option('footer', '');
    $this->view->display_handler->set_option('use_pager', FALSE);
    $this->view->attachment_before = '';
    $this->view->attachment_after = '';
    $this->view->feed_icon = NULL;
  }
}

