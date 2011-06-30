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
    $events[$form_id .'_form_built'] = $defaults + array(
      'label' => t('@form is being built', array('@form' => $label)),
      'group' => 'Rules Forms',
      'variables' => rules_forms_event_variables(),
    );
    $events[$form_id .'_form_submit'] = $defaults + array(
      'label' => t('@form is submitted', array('@form' => $label)),
      'group' => 'Rules Forms',
      'variables' => rules_forms_event_variables(),
    );
    $events[$form_id .'_form_validate'] = $defaults + array(
      'label' => t('@form is being validated', array('@form' => $label)),
      'group' => 'Rules Forms',
      'variables' => rules_forms_event_variables(),
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
      'label' => t('acting user'),
      'handler' => 'rules_forms_variables_global_user',
    ),
  );
}

/**
 * Returns the user account of the acting user.
 *
 * @see rules_forms_rules_event_info()
 * @see rules_forms_event_variables()
 */
function rules_forms_variables_global_user() {
  $uid = $GLOBALS['user']->uid;
  return user_load($uid);
}

/**
 * Implements hook_rules_condition_info().
 */
function rules_forms_rules_condition_info() {
  $conditions = array();
  $conditions['element_value'] = array(
    'label' => t('Form element has value'),
    'group' => 'Rules Forms',
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
        'description' => t('ID of the form element to be targeted.') .' '. _rules_forms_element_description(),
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
  $actions['redirect'] = array(
    'label' => t('Set the redirect target of the form'),
    'group' => 'Rules Forms',
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
  $actions['hide_element'] = array(
    'label' => t('Hide an element of the form'),
    'group' => 'Rules Forms',
    'parameter' => array(
      'form' => array(
        'type' => 'form',
        'label' => t('Form'),
      ),
      'hide' => array(
        'type' => 'text',
        'label' => t('Form element ID'),
        'description' => t('The element that should not appear.') .' '. _rules_forms_element_description(),
      ),
    ),
    'access callback' => 'rules_forms_integration_access',
    'base' => 'rules_forms_action_hide_element',
  );
  $actions['adjust_weight'] = array(
    'label' => t('Adjust weight of an element in the form'),
    'group' => 'Rules Forms',
    'parameter' => array(
      'form' => array(
        'type' => 'form',
        'label' => t('Form'),
      ),
      'element' => array(
        'type' => 'text',
        'label' => t('Form element ID'),
        'description' => t('The element that should be adjusted.') .' '. _rules_forms_element_description(),
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
  $actions['prefix_suffix_html'] = array(
    'label' => t('Insert HTML prefix/suffix code'),
    'group' => 'Rules Forms',
    'parameter' => array(
      'form' => array(
        'type' => 'form',
        'label' => t('Form'),
      ),
      'element' => array(
        'type' => 'text',
        'label' => t('Form element ID'),
        'description' => t('ID of the form element to be targeted.') .' '. _rules_forms_element_description(),
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
  $actions['set_error'] = array(
    'label' => t('Set a form error'),
    'group' => 'Rules Forms',
    'parameter' => array(
      'form' => array(
        'type' => 'form',
        'label' => t('Form'),
      ),
      'element' => array(
        'type' => 'text',
        'label' => t('Form element ID'),
        'description' => t('The element that should be marked.') .' '. _rules_forms_element_description(),
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
  $actions['set_element'] = array(
    'label' => t('Set the default value of a form element'),
    'group' => 'Rules Forms',
    'parameter' => array(
      'form' => array(
        'type' => 'form',
        'label' => t('Form'),
      ),
      'element' => array(
        'type' => 'text',
        'label' => t('Form element ID'),
        'description' => t('The element that should be targeted.') .' '. _rules_forms_element_description(),
      ),
      'value' => array(
        'type' => 'text',
        'label' => t('Default value'),
        'description' => t('The value(s) that will be displayed as default. If the form element allows multiple values, enter one value per line.'),
      ),
    ),
    'access callback' => 'rules_forms_integration_access',
    'base' => 'rules_forms_action_set_element',
  );
  $actions['set_attributes'] = array(
    'label' => t('Set multiple attributes of a form element'),
    'group' => 'Rules Forms',
    'parameter' => array(
      'form' => array(
        'type' => 'form',
        'label' => t('Form'),
      ),
      'element' => array(
        'type' => 'text',
        'label' => t('Form element ID'),
        'description' => t('The element that should be targeted.') .' '. _rules_forms_element_description(),
      ),
      'access' => array(
        'type' => 'boolean',
        'label' => t('Access'),
        'optional' => TRUE,
        'description' => t('Hides a form element.'),
      ),
      'description' => array(
        'type' => 'text',
        'label' => t('Description'),
        'optional' => TRUE,
        'description' => t('The description of the form element.'),
      ),
      'disabled' => array(
        'type' => 'boolean',
        'label' => t('Disabled'),
      ),
      'required' => array(
        'type' => 'boolean',
        'label' => t('Required'),
      ),
      'weight' => array(
        'type' => 'integer',
        'optional' => TRUE,
        'label' => t('Weight'),
      ),
    ),
    'access callback' => 'rules_forms_integration_access',
    'base' => 'rules_forms_action_set_attributes',
    'help' => t('Note: All attributes will be applied to the form element.'),
  );

  $actions['add_element'] = array(
    'label' => t('Add a new form element'),
    'group' => 'Rules Forms',
    'parameter' => array(
      'form' => array(
        'type' => 'form',
        'label' => t('Form'),
      ),
      'type' => array(
        'type' => 'text',
        'label' => t('Element type'),
        'options list' => '_rules_forms_element_options',
        'description' => t('Select the type of element you would like to create.'),
        'restriction' => 'input',
      ),
      'parent' => array(
        'type' => 'text',
        'label' => t('Parent element'),
        'resctriction' => 'input',
        'optional' => TRUE,
        'description' => t('The parent element of the created form element (if any). The parent attribute should not be an input element.'),
      ),
      'name' => array(
        'type' => 'text',
        'label' => t('Element name'),
        'restriction' => 'input',
        'description' => t('The machine-readable name of the form element. Names may only include lower-case letters and underscores.'),
      ),
      'title' => array(
        'type' => 'text',
        'label' => t('Element title'),
        'resctriction' => 'input',
        'description' => t('The title of the form element which will be displayed next to it.')
      ),
      'description' => array(
        'type' => 'text',
        'label' => t('Description'),
        'resctriction' => 'input',
        'optional' => TRUE,
        'description' => t('The description of the form element to be displayed on the form.'),
      ),
      'options' => array(
        'type' => 'text',
        'label' => t('Element options'),
        'optional' => TRUE,
        'description' => t('For form elements that allow multiple options, enter one option for each line. For form elements that do not have multple options this field will be ignored.'),
      ),
      'default_value' => array(
        'type' => 'text',
        'label' => t('Default value'),
        'restriction' => 'input',
        'optional' => TRUE,
        'description' => t('The value(s) that will be displayed as default. If the form element allows multiple values, enter one value per line. For TRUE or FALSE type form elements, enter 1 for TRUE or 0 for FALSE.'),
      ),
      'multiple' => array(
        'type' => 'boolean',
        'label' => t('Allow multiple values'),
        'optional' => TRUE,
        'description' => t('This option applies to select lists only. Enable this option to enable multiple value selections.'),
      ),
      'required' => array(
        'type' => 'boolean',
        'label' => t('Required'),
      ),
      'weight' => array(
        'type' => 'integer',
        'label' => t('Weight'),
        'optional' => TRUE,
      ),
    ),
    'provides' => array(
      'form_element' => array(
        'type' => 'unknown',
        'label' => t('Created form element'),
      ),
    ),
    'access callback' => 'rules_forms_integration_access',
    'base' => 'rules_forms_action_add_element',
    'help' => t('This action does not execute PHP code by itself. To save form data, you will need to set up an execute PHP code action on form submit.'),
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
 * Provides actions for adding form elements.
 */
function _rules_forms_element_options() {
  return array(
    'textfield' => t('Text field'),
    'textarea' => t('Text area'),
    'checkbox' => t('Checkbox'),
    'checkboxes' => t('Checkboxes'),
    'select' => t('Select list'),
    'radios' => t('Radios'),
  );
}

/**
 * Implements hook_rules_data_type_info().
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

/**
 * Rules Forms integration access callback.
 */
function rules_forms_integration_access($type, $name) {
  return user_access('administer rules forms rules');
}
