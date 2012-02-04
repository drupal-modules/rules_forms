<?php

/**
 * @file
 * Rules events, conditions, and actions hooks for Rules Forms module.
 */

/**
 * Implements hook_rules_file_info().
 */
function rules_forms_rules_file_info() {
  return array('includes/rules_forms.eval');
}

/**
 * Implements hook_rules_event_info().
 */
function rules_forms_rules_event_info() {
  $form_events = variable_get('rules_forms_event_info', array());
  $defaults = array(
    'group' => 'Rules forms',
    'access callback' => 'rules_forms_integration_access',
  );

  $events = array();
  foreach ($form_events as $form_id => $info) {
    $events['rules_forms_' . $form_id . '_form_built'] = $defaults + array(
      'label' => t('@form is being built', array('@form' => $info['label'])),
      'variables' => rules_forms_event_variables() + rules_forms_element_variables($form_id),
    );
    $events['rules_forms_' . $form_id . '_form_submit'] = $defaults + array(
      'label' => t('@form is submitted', array('@form' => $info['label'])),
      'variables' => rules_forms_event_variables() + rules_forms_element_variables($form_id),
    );
    $events['rules_forms_' . $form_id . '_form_validate'] = $defaults + array(
      'label' => t('@form is being validated', array('@form' => $info['label'])),
      'variables' => rules_forms_event_variables() + rules_forms_element_variables($form_id),
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
 * Adds element variables for the form.
 */
function rules_forms_element_variables($form_id) {
  $variables = array();
  $events = variable_get('rules_forms_event_info', array());
  if (isset($events[$form_id]['elements']) && is_array($events[$form_id]['elements'])) {
    foreach ($events[$form_id]['elements'] as $key => $info) {
      $variables[$info['type'] .':'. $key] = array(
        'type' => 'form_element',
        'label' => $info['label'],
      );
    }
  }
  return $variables;
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
        'type' => 'form_element',
        'label' => t('Form element'),
        'description' => t('The form element to be targeted.'),
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
  $actions['rules_forms_set_error'] = array(
    'label' => t('Set a form error'),
    'group' => 'Rules forms',
    'parameter' => array(
      'form' => array('type' => 'form', 'label' => t('Form')),
      'element' => array(
        'type' => 'form_element',
        'label' => t('Form element'),
        'description' => t('The form element to be targeted.'),
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
  $actions['rules_forms_set_attribute'] = array(
    'label' => t('Set element attribute'),
    'group' => 'Rules forms',
    'parameter' => array(
      'form' => array(
        'type' => 'form',
        'label' => t('Form'),
        'restriction' => 'selector',
      ),
      'element' => array(
        'type' => 'form_element',
        'label' => t('Form element'),
        'restriction' => 'input',
        'description' => t('The form element to be targeted.'),
      ),
      'attribute' => array(
        'type' => 'text',
        'label' => t('Element attribute'),
        'restriction' => 'input',
        'options list' => '_rules_forms_attribute_options',
        'description' => t('The element attribute to set.'),
      ),
      'value' => array(
        'type' => 'text',
        'label' => t('Value'),
        'restriction' => 'input',
        'description' => t('The attribute value. For boolean values enter 0 for FALSE and 1 for TRUE. ' .
            'For multiple option values such has select lists, <strong>key-value pairs MUST be specified '.
            'as "safe_key|Some readable option"</strong>. Use of only alphanumeric characters and underscores '.
            'is recommended in keys. One option per line.'),
      ),
    ),
    'callbacks' => array(
      'access' => 'rules_forms_integration_access',
      'form_alter' => 'rules_forms_action_attribute_form_alter',
    ),
    'base' => 'rules_forms_action_set_attribute',
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
    'form_element' => array(
      'label' => t('form element'),
      'group' => 'Rules forms',
      'ui class' => 'RulesFormsDataElement',
    ),
  );
}

/**
 * Alters the form for action: Set element property. Taken from Rules module.
 */
function rules_forms_action_attribute_form_alter(&$form, &$form_state, $options, RulesAbstractPlugin $element) {
  $first_step = empty($element->settings['element']);
  $form['reload'] = array(
    '#weight' => 10,
    '#type' => 'submit',
    '#name' => 'reload',
    '#value' => $first_step ? t('Continue') : t('Reload form'),
    '#limit_validation_errors' => array(array('parameter', 'element')),
    '#submit' => array('rules_action_type_form_submit_rebuild'),
    '#ajax' => rules_ui_form_default_ajax(),
  );

  // Use ajax and trigger as the reload button.
  $form['parameter']['type']['settings']['element']['#ajax'] = $form['reload']['#ajax'] + array(
    'event' => 'change',
    'trigger_as' => array('name' => 'reload'),
  );

  // Hide all form elements other than the element selector.
  if ($first_step) {

    // In the first step show only the type select.
    foreach (element_children($form['parameter']) as $key) {
      if ($key != 'element') {
        unset($form['parameter'][$key]);
      }
    }

    unset($form['submit']);
    unset($form['provides']);
  }
  else {
    // Hide the reload button in case js is enabled and it's not the first step.
    $form['reload']['#attributes'] = array('class' => array('rules-hide-js'));
  }
}

/**
 * Provides attribute options for form elements.
 *
 * @param $element
 *   The element to return options for.
 * @param $param
 *   The name of the parameter to return options for.
 */
function _rules_forms_attribute_options(RulesPlugin $element, $name) {
  $options = array();
  if (isset($element->settings['element'])) {
    // Element types are prepended to keys in the element select list.
    $type = array_shift(explode(':', $element->settings['element']));

    // Add standard attributes.
    $options = array(
      '#access' => 'access',
      '#attributes' => 'attributes',
      '#prefix' => 'prefix',
      '#suffix' => 'suffix',
      '#weight' => 'weight',
    );

    // Add attributes common to input elements.
    switch ($type) {
      case 'textfield':
      case 'textarea':
      case 'select':
      case 'radio':
      case 'checkbox':
      case 'checkboxes':
        $options += array(
          '#default_value' => 'default value',
          '#description' => 'description',
          '#disabled' => 'disabled',
          '#required' => 'required',
          '#tree' => 'tree',
        );
        break;
    }

    // Add attributes for multiple option fields.
    if (in_array($type, array('select', 'checkbox', 'radios', 'checkboxes'))) {
      $options += array('#options' => 'options');
    }

    if ($type == 'checkboxes') {
      $options += array('#multiple' => 'multiple');
    }

    asort($options);
  }
  return $options;
}

/**
 * Rules Forms integration access callback.
 */
function rules_forms_integration_access($type, $name) {
  return user_access('administer rules forms rules');
}
