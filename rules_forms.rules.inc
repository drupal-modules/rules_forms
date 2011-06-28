<?php

/**
 * @file
 * Rules events, conditions, and actions hooks for Rules Forms module.
 */

/**
 * Implements hook_rules_file_info().
 */
function rules_forms_rules_file_info() {
  return array('rules_forms.eval');
}

/**
 * Implementation of hook_rules_event_info().
 */
function rules_forms_rules_event_info() {
  $form_events = variable_get('rules_forms_events', array());
  $events = array();
  foreach ($form_events as $form_id => $label) {
    $events[$form_id .'_form_built'] = array(
      'label' => t('@form is being built', array('@form' => $label)),
      'group' => 'Rules Forms',
      'variables' => rules_forms_event_variables(),
    );
    $events[$form_id .'_form_submit'] = array(
      'label' => t('@form is submitted', array('@form' => $label)),
      'group' => 'Rules Forms',
      'variables' => rules_forms_event_variables(),
    );
    $events[$form_id .'_form_validate'] = array(
      'label' => t('@form is being validated', array('@form' => $label)),
      'group' => 'Rules Forms',
      'variables' => rules_forms_event_variables(),
    );
  }
  return $events;
}

/**
 * Returns some arguments suitable for hook form alter.
 */
function rules_forms_event_variables() {
  return array(
    'form' => array('type' => 'form', 'label' => t('Form')),
    'form_state' => array('type' => 'form_state', 'label' => t('Form state')),
    'form_id' => array('type' => 'string', 'label' => t('Form ID')),
  ) + rules_events_global_user_argument();
}

/**
 * Implementation of hook_rules_condition_info().
 */
function rules_forms_rules_condition_info() {
  $conditions = array();
  $conditions['rules_forms_condition_element_value'] = array(
    'label' => t('Form element has value'),
    'group' => 'Rules Forms',
    'parameter' => array(
      'form' => array('type' => 'form', 'label' => t('Form')),
      'form_state' => array('type' => 'form_state', 'label' => t('Form state')),
      'element' => array(
        'type' => 'string',
        'label' => t('Form element ID'),
        'description' => t('ID of the form element to be targeted.') .' '. _rules_forms_element_description(),
      ),
      'value' => array(
        'type' => 'string',
        'label' => t('Value(s)'),
        'description' => t('Value(s) assigned to the form element. If the form element allows multiple values, enter one value per line.'),
      ),
    ),
  ),
}

/**
 * Implementation of hook_rules_action_info().
 */
function rules_forms_rules_action_info() {
  $actions = array();
  $actions['rules_forms_action_redirect'] = array(
    'label' => t('Set the redirect target of the form'),
    'group' => 'Rules Forms',
    'parameter' => array(
      'form' => array('type' => 'form', 'label' => t('Form')),
      'path' => array('type' => 'string', 'label' => t('Path')),
      'query' => array('type' => 'string', 'label' => t('Query')),
      'fragment' => array('type' => 'string', 'label' => t('Fragment')),
    ),
    'help' => t('Enter a Drupal path, path alias, or external URL to redirect to. Enter (optional) queries after "?" and (optional) anchor after "#".'),
  );
  $actions['rules_forms_action_hide_element'] = array(
    'label' => t('Hide an element of the form'),
    'group' => 'Rules Forms',
    'parameter' => array(
      'form' => array('type' => 'form', 'label' => t('Form')),
      'hide' => array(
        'type' => 'string',
        'label' => t('Form element ID'),
        'description' => t('The element that should not appear.') .' '. _rules_forms_element_description(),
      ),
    ),
  );
  $actions['rules_forms_action_adjust_weight'] = array(
    'label' => t('Adjust weight of an element in the form'),
    'group' => 'Rules Forms',
    'parameter' => array(
      'form' => array('type' => 'form', 'label' => t('Form')),
      'element' => array(
        'type' => 'string',
        'label' => t('Form element ID'),
        'description' => t('The element that should be adjusted.') .' '. _rules_forms_element_description(),
      ),
      'weight' => array(
        'type' => 'number',
        'label' => t('Element weight'),
        'description' => t('Low numbers make the element bubble up, high numbers sink it down.'),
      ),
    ),
  );
  $actions['rules_forms_action_prefix_suffix_html'] = array(
    'label' => t('Insert HTML prefix/suffix code'),
    'group' => 'Rules Forms',
    'parameter' => array(
      'form' => array('type' => 'form', 'label' => t('Form')),
      'element' => array(
        'type' => 'string',
        'label' => t('Form element ID'),
        'description' => t('ID of the form element to be targeted.') .' '. _rules_forms_element_description(),
      ),
      'prefix' => array(
        'type' => 'string',
        'label' => t('Prefixed HTML'),
        'description' => t('HTML inserted before.'),
      ),
      'suffix' => array(
        'type' => 'string',
        'label' => t('Suffixed HTML'),
        'description' => t('HTML inserted after.'),
      ),
    ),
  );
  $actions['rules_forms_action_set_error'] = array(
    'label' => t('Set a form error'),
    'group' => 'Rules Forms',
    'parameter' => array(
      'form' => array('type' => 'form', 'label' => t('Form')),
      'element' => array(
        'type' => 'string',
        'label' => t('Form element ID'),
        'description' => t('The element that should be marked.') .' '. _rules_forms_element_description(),
      ),
      'message' => array(
        'type' => 'string',
        'label' => t('Message'),
        'description' => t('The message that should be displayed to the user.'),
      ),
    ),
  );
  $actions['rules_forms_action_set_element'] = array(
  'label' => t('Set the default value of a form element'),
  'group' => 'Rules Forms',
  'parameter' => array(
    'form' => array('type' => 'form', 'label' => t('Form')),
    'element' => array(
      'type' => 'string',
      'label' => t('Form element ID'),
      'description' => t('The element that should be targeted.') .' '. _rules_forms_element_description(),
    ),
    'value' => array(
      'type' => 'string',
      'label' => t('Default value'),
      'description' => t('The value(s) that will be displayed as default. If the form element allows multiple values, enter one value per line.'),
    ),
  );
}

function _rules_forms_element_description() {
  return t('Examples on the "Create Article" form: "title" for the title field or "body_field[body]" for the body field.');
}

/**
 * Implementation of hook_rules_data_type_info().
 */
function rules_forms_rules_data_info() {
  return array(
    'form' => array(
      'label' => t('form'),
      'group' => t('Rules Forms'),
      'property info' => array(),
    ),
    'form_state' => array(
      'label' => t('form state'),
      'group' => t('Rules Forms'),
      'property info' => array(),
    ),
  );
}
