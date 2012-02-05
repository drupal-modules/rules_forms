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
 * Implements hook_rules_event_info().
 */
function rules_forms_rules_event_info() {
  $form_events = variable_get('rules_forms_events', array());
  $defaults = array(
    'group' => 'Rules Forms',
    'variables' => rules_forms_event_variables(),
    'access callback' => 'rules_forms_integration_access',
  );

  $events = array();
  foreach ($form_events as $form_id => $label) {
    $events['rules_forms_' . $form_id . '_form_built'] = $defaults + array(
      'label' => t('@form is being built', array('@form' => $label)),
    );
    $events['rules_forms_' . $form_id . '_form_submit'] = $defaults + array(
      'label' => t('@form is submitted', array('@form' => $label)),
    );
    $events['rules_forms_' . $form_id . '_form_validate'] = $defaults + array(
      'label' => t('@form is being validated', array('@form' => $label)),
    );
  }
  return $events;
}

/**
 * Returns some arguments suitable for hook form alter.
 *
 * @see rules_forms_rules_event_info()
 */
function rules_forms_event_variables() {
  return array(
    'form' => array(
      'type' => 'form',
      'label' => t('Form'),
    ),
    'form_state' => array(
      'type' => 'form_state',
      'label' => t('Form state'),
    ),
    'form_id' => array(
      'type' => 'text',
      'label' => t('Form ID'),
    ),
    'user' => array(
      'type' => 'user',
      'label' => t('Logged in user'),
    ),
  );
}

/**
 * Implements hook_rules_condition_info().
 */
function rules_forms_rules_condition_info() {
  $conditions = array();
  $conditions['rules_forms_element_value'] = array(
    'label' => t('Form element has value'),
    'group' => 'Rules forms',
    'parameter' => array(
      'form' => array(
        'type' => 'form',
        'label' => t('Form'),
      ),
      'form_state' => array(
        'type' => 'form_state',
        'label' => t('Form state'),
      ),
      'element' => array(
        'type' => 'text',
        'label' => t('Form element ID'),
        'description' => t('ID of the form element to be targeted.') . ' ' . _rules_forms_element_description(),
      ),
      'value' => array(
        'type' => 'text',
        'label' => t('Value(s)'),
        'description' => t('Value(s) assigned to the form element. If the form element allows multiple values, enter one value per line.'),
      ),
    ),
    'access callback' => 'rules_forms_integration_access',
    'base' => 'rules_forms_condition_element_value',
  );
  return $conditions;
}

/**
 * Implements hook_rules_action_info().
 */
function rules_forms_rules_action_info() {
  $actions = array();
  $actions['rules_forms_element_title'] = array(
      'label' => t('Set the title of an element in the form'),
      'group' => 'Rules forms',
      'parameter' => array(
        'form' => array('type' => 'form', 'label' => t('Form')),
        'element' => array(
          'type' => 'text',
          'label' => t('Form element ID'),
          'description' => t('ID of the form element to be targeted.') . ' ' . _rules_forms_element_description(),
        ),
        'title' => array(
          'type' => 'text',
          'label' => t('Title'),
          'optional' => TRUE,
        ),
      ),
      'access callback' => 'rules_forms_integration_access',
      'base' => 'rules_forms_action_set_title',
  );
  $actions['rules_forms_element_description'] = array(
    'label' => t('Set the description of an element in the form'),
    'group' => 'Rules forms',
    'parameter' => array(
      'form' => array('type' => 'form', 'label' => t('Form')),
      'element' => array(
        'type' => 'text',
        'label' => t('Form element ID'),
        'description' => t('ID of the form element to be targeted.') . ' ' . _rules_forms_element_description(),
      ),
      'title' => array(
        'type' => 'text',
        'label' => t('Description'),
        'optional' => TRUE,
      ),
    ),
    'access callback' => 'rules_forms_integration_access',
    'base' => 'rules_forms_action_set_description',
  );
  $actions['rules_forms_element_access'] = array(
    'label' => t('Hide an element in the form'),
    'group' => 'Rules forms',
    'parameter' => array(
      'form' => array('type' => 'form', 'label' => t('Form')),
      'element' => array(
        'type' => 'text',
        'label' => t('Form element ID'),
        'description' => t('ID of the form element to be targeted.') . ' ' . _rules_forms_element_description(),
      ),
      'access' => array(
        'type' => 'boolean',
        'label' => t('Access'),
        'optional' => TRUE,
        'description' => t('Hides a form element.'),
      ),
    ),
    'access callback' => 'rules_forms_integration_access',
    'base' => 'rules_forms_action_set_access',
  );
  $actions['rules_forms_element_disabled'] = array(
    'label' => t('Disable an element in the form'),
    'group' => 'Rules forms',
    'parameter' => array(
      'form' => array('type' => 'form', 'label' => t('Form')),
      'element' => array(
        'type' => 'text',
        'label' => t('Form element ID'),
        'description' => t('ID of the form element to be targeted.') . ' ' . _rules_forms_element_description(),
      ),
      'access' => array(
        'type' => 'boolean',
        'label' => t('Disable'),
        'optional' => TRUE,
        'description' => t('Disables a form element.'),
      ),
    ),
    'access callback' => 'rules_forms_integration_access',
    'base' => 'rules_forms_action_set_disabled',
  );
  $actions['rules_forms_element_required'] = array(
    'label' => t('Require an element in the form'),
    'group' => 'Rules forms',
    'parameter' => array(
      'form' => array('type' => 'form', 'label' => t('Form')),
      'element' => array(
        'type' => 'text',
        'label' => t('Form element ID'),
        'description' => t('ID of the form element to be targeted.') . ' ' . _rules_forms_element_description(),
      ),
      'require' => array(
        'type' => 'boolean',
        'label' => t('Required'),
        'optional' => TRUE,
        'description' => t('Requires a form element.'),
      ),
    ),
    'access callback' => 'rules_forms_integration_access',
    'base' => 'rules_forms_action_set_required',
  );
  $actions['rules_forms_options'] = array(
    'label' => t('Set multiple value options of an element in the form'),
    'group' => 'Rules forms',
    'parameter' => array(
      'form' => array('type' => 'form', 'label' => t('Form')),
      'element' => array(
        'type' => 'text',
        'label' => t('Form element ID'),
        'description' => t('ID of the form element to be targeted.') . ' ' . _rules_forms_element_description(),
      ),
      'access' => array(
        'type' => 'text',
        'label' => t('Options'),
        'description' => t('<strong>Key-value pairs MUST be specified as "safe_key|Some readable option"</strong>. Use of only alphanumeric characters and underscores is recommended in keys. One option per line.'),
      ),
    ),
    'base' => 'rules_forms_action_set_options',
    'validate' => 'rules_forms_action_set_options_validate',
    'access callback' => 'rules_forms_integration_access',
  );
  $actions['rules_forms_default'] = array(
    'label' => t('Set the default value of an element in the form'),
    'group' => 'Rules forms',
    'parameter' => array(
      'form' => array('type' => 'form', 'label' => t('Form')),
      'element' => array(
        'type' => 'text',
        'label' => t('Form element ID'),
        'description' => t('ID of the form element to be targeted.') . ' ' . _rules_forms_element_description(),
      ),
      'value' => array(
        'type' => 'text',
        'label' => t('Value'),
      ),
    ),
    'base' => 'rules_forms_action_set_element',
    'access callback' => 'rules_forms_integration_access',
  );
  $actions['rules_forms_redirect'] = array(
    'label' => t('Set the redirect target of the form'),
    'group' => 'Rules forms',
    'parameter' => array(
      'form_state' => array('type' => 'form_state', 'label' => t('Form state')),
      'path' => array('type' => 'text', 'label' => t('Path')),
      'query' => array('type' => 'text', 'label' => t('Query'), 'optional' => TRUE),
      'fragment' => array('type' => 'text', 'label' => t('Fragment'), 'optional' => TRUE),
    ),
    'base' => 'rules_forms_action_redirect',
    'access callback' => 'rules_forms_integration_access',
    'help' => t('Enter a Drupal path, path alias, or external URL to redirect to. Enter (optional) queries after "?" and (optional) anchor after "#".'),
  );
  $actions['rules_forms_adjust_weight'] = array(
    'label' => t('Adjust weight of an element in the form'),
    'group' => 'Rules forms',
    'parameter' => array(
      'form' => array('type' => 'form', 'label' => t('Form')),
      'element' => array(
        'type' => 'text',
        'label' => t('Form element ID'),
        'description' => t('ID of the form element to be targeted.') . ' ' . _rules_forms_element_description(),
      ),
      'weight' => array(
        'type' => 'integer',
        'label' => t('Element weight'),
        'description' => t('Low numbers make the element bubble up, high numbers sink it down.'),
      ),
    ),
    'access callback' => 'rules_forms_integration_access',
    'base' => 'rules_forms_action_adjust_weight',
  );
  $actions['rules_forms_prefix_suffix_html'] = array(
    'label' => t('Insert HTML into the prefix/suffix of an element in the form'),
    'group' => 'Rules forms',
    'parameter' => array(
      'form' => array('type' => 'form', 'label' => t('Form')),
      'element' => array(
        'type' => 'text',
        'label' => t('Form element ID'),
        'description' => t('ID of the form element to be targeted.') . ' ' . _rules_forms_element_description(),
      ),
      'prefix' => array(
        'type' => 'text',
        'label' => t('Prefixed HTML'),
        'description' => t('HTML inserted before.'),
      ),
      'suffix' => array(
        'type' => 'text',
        'label' => t('Suffixed HTML'),
        'description' => t('HTML inserted after.'),
      ),
    ),
    'access callback' => 'rules_forms_integration_access',
    'base' => 'rules_forms_action_prefix_suffix_html',
  );
  $actions['rules_forms_set_error'] = array(
    'label' => t('Set an error on the form'),
    'group' => 'Rules forms',
    'parameter' => array(
      'form' => array('type' => 'form', 'label' => t('Form')),
      'element' => array(
        'type' => 'text',
        'label' => t('Form element ID'),
        'description' => t('ID of the form element to be targeted.') . ' ' . _rules_forms_element_description(),
      ),
      'message' => array(
        'type' => 'text',
        'label' => t('Message'),
        'description' => t('The message that should be displayed to the user.'),
      ),
    ),
    'access callback' => 'rules_forms_integration_access',
    'base' => 'rules_forms_action_set_error',
  );
  return $actions;
}

/**
 * Provides a standard explanaition of how to use element IDs with
 * conditions and actions.
 */
function _rules_forms_element_description() {
  return t('Examples on the "Create Article" form: "title" for the title field or "body[und][0][value]" for the body field.');
}

/**
 * Implements hook_rules_data_type_info().
 */
function rules_forms_rules_data_info() {
  return array(
    'form' => array(
      'label' => t('form'),
      'group' => t('Rules forms'),
    ),
    'form_state' => array(
      'label' => t('form state'),
      'group' => t('Rules forms'),
    ),
  );
}

/**
 * Rules Forms integration access callback.
 */
function rules_forms_integration_access($type, $name) {
  return user_access('administer rules forms rules');
}
