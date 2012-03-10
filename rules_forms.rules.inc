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
  $events = array();
  foreach (rules_forms_get_form_info() as $form_id => $info) {
    $defaults = array(
      'group' => 'Rules Forms',
      'access callback' => 'rules_forms_integration_access',
      'variables' => array(
        'form' => array(
          'type' => $form_id,
          'label' => $info['label'],
        ),
        'form_state' => array(
          'type' => $form_id .'_state',
          'label' => t('@form state', array('@form' => $info['label'])),
        ),
      ),
    );
    $events['rules_forms_' . $form_id . '_form_built'] = $defaults + array(
      'label' => t('@form is being built', array('@form' => $info['label'])),
    );
    $events['rules_forms_' . $form_id . '_form_submit'] = $defaults + array(
      'label' => t('@form is submitted', array('@form' => $info['label'])),
    );
    $events['rules_forms_' . $form_id . '_form_validate'] = $defaults + array(
      'label' => t('@form is being validated', array('@form' => $info['label'])),
    );
  }
  return $events;
}

/**
 * Implements hook_rules_condition_info().
 */
function rules_forms_rules_condition_info() {
  $conditions = array();
  // @TODO: The element has value form alter callback should display a
  // form based on the type of value the element has.
  $conditions['rules_forms_element_has_value'] = array(
    'label' => t('Form element has value'),
    'group' => 'Rules Forms',
    'parameter' => array(
      'data' => array(
        'type' => 'form_element',
        'label' => t('Form element'),
        'description' => t('Select the form element to evaluate. You can traverse the form structure using the data selector.'),
        'restriction' => 'selector',
        'wrapped' => TRUE,
        'allow null' => TRUE,
      ),
      'value' => array(
        'type' => '*',
        'label' => t('Value'),
        'description' => t('Value(s) assigned to the form element. If the form element allows multiple values, enter one value per line.'),
        'allow null' => TRUE,
        'optional' => TRUE,
      ),
      'regex' => array(
        'type' => 'boolean',
        'label' => t('Evaluate as regular expression'),
        'optional' => TRUE,
        'description' => t('Perform the data comparison using the provided value as a regular expression. '.
                           'To execute multiple regular expressions, enter one expression per line.'),
      ),
    ),
    'base' => 'rules_forms_condition_element_value',
    'callbacks' => array(
      'form_alter' => 'rules_forms_property_value_form_alter',
      'info_alter' => 'rules_forms_element_value_info_alter',
    ),
  );
  $conditions['rules_forms_attribute_has_value'] = array(
    'label' => t('Form element attribute has value'),
    'group' => 'Rules Forms',
    'parameter' => array(
      'data' => array(
        'type' => '*',
        'label' => t('Form element attribute'),
        'description' => t('Select the form element attribute to evaluate. You can traverse the form structure using the data selector. Element attributes are prefixed by a hash mark (#).'),
        'restriction' => 'selector',
        'wrapped' => TRUE,
        'allow null' => TRUE,
      ),
      'value' => array(
        'type' => '*',
        'label' => t('Value'),
        'description' => t('Value(s) assigned to the form element attribute. If the form element allows multiple values, enter one value per line.'),
        'allow null' => TRUE,
        'optional' => TRUE,
      ),
      'regex' => array(
        'type' => 'boolean',
        'label' => t('Evaluate as regular expression'),
        'optional' => TRUE,
        'description' => t('Perform the data comparison using the provided value as a regular expression. '.
                           'To execute multiple regular expressions, enter one expression per line.'),
      ),
    ),
    'base' => 'rules_forms_condition_attribute_value',
    'callbacks' => array(
      'validate' => 'rules_forms_attribute_get_validate',
      'form_alter' => 'rules_forms_property_value_form_alter',
    ),
  );
  $conditions['rules_forms_element_changed'] = array(
    'label' => t('Form element value has changed'),
    'group' => 'Rules Forms',
    'parameter' => array(
      'form_state' => array(
        'type' => 'form_state',
        'label' => t('Form state'),
      ),
      'data' => array(
        'type' => 'form_element',
        'label' => t('Form element'),
        'description' => t('Select the form element to evaluate. You can traverse the form structure using the data selector. Element attributes are prefixed by a hash mark (#).'),
        'restriction' => 'selector',
        'wrapped' => TRUE,
        'allow null' => TRUE,
      ),
    ),
    'base' => 'rules_forms_condition_element_changed',
  );
  $conditions['rules_forms_button_clicked'] = array(
    'label' => t('Form button was clicked'),
    'group' => 'Rules Forms',
    'parameter' => array(
      'form_state' => array(
        'type' => 'form_state',
        'label' => t('Form state'),
      ),
      'data' => array(
        'type' => 'form_element',
        'label' => t('Form button'),
        'description' => t('Select the form element to evaluate. You can traverse the form structure using the data selector. Element attributes are prefixed by a hash mark (#).'),
        'restriction' => 'selector',
        'wrapped' => TRUE,
        'allow null' => TRUE,
      ),
    ),
    'base' => 'rules_forms_condition_button_clicked',
  );
  return $conditions;
}

/**
 * Implements hook_rules_action_info().
 */
function rules_forms_rules_action_info() {
  $actions = array();
  $actions['rules_forms_attribute_set'] = array(
    'label' => t('Set a attribute of an element in the form'),
    'group' => 'Rules Forms',
    'parameter' => array(
      'data' => array(
        'type' => '*',
        'label' => t('Form element attribute'),
        'restriction' => 'selector',
        'description' => t('Select the form element attribute to modify. You can traverse the form structure using the data selector. Element attributes are prefixed by a hash mark (#).'),
        'wrapped' => TRUE,
        'allow null' => TRUE,
      ),
      'value' => array(
        'type' => '*',
        'label' => t('Value'),
        'description' => t('The new value to set for the specified data.'),
        'allow null' => TRUE,
        'optional' => TRUE,
      ),
    ),
    'base' => 'rules_forms_action_attribute_set',
    'callbacks' => array(
      'validate' => 'rules_forms_attribute_set_validate',
      'form_alter' => 'rules_forms_property_value_form_alter',
      'info_alter' => 'rules_forms_action_attribute_set_info_alter',
    ),
  );
  $actions['rules_forms_set_error'] = array(
    'label' => t('Set an error on the form'),
    'group' => 'Rules Forms',
    'parameter' =>  array(
      // We use the 'data' and 'value' keys to support reuse of the form alter callback.
      'data' => array(
        'type' => 'form_element',
        'label' => t('Targeted element'),
        'restriction' => 'selector',
        'description' => t('Select the form element to target (if any).'),
        'wrapped' => TRUE,
        'allow null' => TRUE,
        'optional' => TRUE,
      ),
      'value' => array(
        'type' => 'text',
        'label' => t('Message'),
        'optional' => TRUE,
        'description' => t('The message that should be displayed to the user.'),
      ),
    ),
    'base' => 'rules_forms_action_set_error',
    'callbacks' => array(
      'form_alter' => 'rules_forms_property_value_form_alter',
    ),
  );
  $actions['rules_forms_redirect'] = array(
    'label' => t('Set the redirect target of the form'),
    'group' => 'Rules Forms',
    'parameter' => array(
      'form_state' => array(
        'type' => 'form_state',
        'label' => t('Form state'),
      ),
      'path' => array(
        'type' => 'text',
        'label' => t('Path'),
      ),
      'query' => array(
        'type' => 'text',
        'label' => t('Query'),
        'optional' => TRUE,
      ),
      'fragment' => array(
        'type' => 'text',
        'label' => t('Fragment'),
        'optional' => TRUE,
      ),
    ),
    'base' => 'rules_forms_action_redirect',
    'help' => t('Enter a Drupal path, path alias, or external URL to redirect to. Enter (optional) queries after "?" and (optional) anchor after "#".'),
  );
  return $actions;
}

/**
 * Implements hook_rules_data_info().
 */
function rules_forms_rules_data_info() {
  // Base data types for form and form_states.
  $data_info['form'] = array(
    'label' => t('form'),
    'group' => t('Rules Forms'),
    'wrap' => TRUE,
    'wrapper class' => 'RulesFormsFormWrapper',
  );
  $data_info['form_state'] = array(
    'label' => t('form state'),
    'group' => t('Rules Forms'),
    'wrap' => TRUE,
    'wrapper class' => 'RulesFormsFormStateWrapper',
  );

  // Each active form has its own data type with unique property info
  // describing the structure of the form.
  foreach (rules_forms_get_form_info() as $form_id => $info) {
    $data_info[$form_id] = array(
      'label' => $info['label'],
      'group' => t('Rules Forms'),
      'parent' => 'form',
      'property info' => $info['properties']['form'],
      'wrap' => TRUE,
      'wrapper class' => 'RulesFormsFormWrapper',
    );
    $data_info[$form_id .'_state'] = array(
      'label' => t('@form state', array('@form' => $info['label'])),
      'group' => t('Rules Forms'),
      'parent' => 'form_state',
      'property info' => $info['properties']['form_state'],
      'wrap' => TRUE,
      'wrapper class' => 'RulesFormsFormStateWrapper',
    );
  }

  // Provide Rules data types for form elements and their attributes.
  // Each form's property info is made up of form elements types that
  // refer to these data types.
  module_load_include('inc', 'rules_forms', 'includes/rules_forms.info');
  $data_info['form_element'] = array(
    'label' => t('form element'),
    'group' => t('Rules Forms'),
    'wrap' => TRUE,
    'wrapper class' => 'RulesFormsElementWrapper',
  );

  foreach (_rules_forms_element_info() as $name => $label) {
    $data_info['form_' . $name] = array(
      'label' => $label,
      'group' => t('Rules Forms'),
      'parent' => 'form_element',
      'wrap' => TRUE,
      'wrapper class' => 'RulesFormsElementWrapper',
      'creation callback' => 'rules_forms_create_element',
    );
  }
  return $data_info;
}

/**
 * Creates a new form element from property info.
 *
 * @param array $values
 *   The property values to set.
 */
function rules_forms_create_element($values = array()) {
  return $values;
}

/**
 * Form alter callback for the data_set action.
 * This function was largely taken from Rules.
 */
function rules_forms_property_value_form_alter(&$form, &$form_state, $options, RulesAbstractPlugin $element) {
  if (!empty($options['init']) && !isset($form_state['rules_element_step'])) {
    $form['negate']['#access'] = FALSE;
    unset($form['parameter']['value']);
    // Make this callback useful for conditions that use a regex field.
    if (isset($form['parameter']['regex'])) {
      unset($form['parameter']['regex']);
    }
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Continue'),
      '#limit_validation_errors' => array(array('parameter', 'data')),
      '#submit' => array('rules_form_submit_rebuild'),
    );
    $form_state['rules_element_step'] = 'data_value';
    // Clear the parameter mode for the value parameter, so its gets the proper
    // default value based upon the type of the the selected data on rebuild.
    unset($form_state['parameter_mode']['value']);
  }
  else {
    // Change the data parameter to be not editable.
    $form['parameter']['data']['settings']['#access'] = FALSE;
    $form['parameter']['data']['info'] = array(
      '#prefix' => '<p>',
      '#markup' => t('<strong>Selected data:</strong> %selector', array('%selector' => $element->settings['data:select'])),
      '#suffix' => '</p>',
    );
    $type = $form['parameter']['value']['settings']['value']['#type'];
    if ($type != 'textfield' && $type != 'textarea') {
      $form['parameter']['regex']['#access'] = FALSE;
      $form['parameter']['regex']['settings']['regex']['#type'] = 'value';
      $form['parameter']['regex']['settings']['regex']['#value'] = 0;
    }
  }
}

/**
 * Validation callback for set attribute actions.
 */
function rules_forms_attribute_set_validate(RulesAbstractPlugin $element) {
  $element->settings += array('data:select' => NULL);
  $info = $element->applyDataSelector($element->settings['data:select'])->info();
  if (strpos($element->settings['data:select'], ':') !== FALSE) {
    if (empty($info['setter callback'])) {
      throw new RulesIntegrityException(t('The selected data property does not support reading.'));
    }
    elseif (!isset($info['attribute info'])) {
      throw new RulesIntegrityException(t('The selected data property is not a form element attribute.'));
    }
  }
}

/**
 * Validation callback for get attribute actions.
 */
function rules_forms_attribute_get_validate(RulesAbstractPlugin $element) {
  $element->settings += array('data:select' => NULL);
  $info = $element->applyDataSelector($element->settings['data:select'])->info();
  if (strpos($element->settings['data:select'], ':') !== FALSE) {
    if (empty($info['getter callback'])) {
      throw new RulesIntegrityException(t('The selected data property does not support reading.'));
    }
    elseif (!isset($info['attribute info'])) {
      throw new RulesIntegrityException(t('The selected data property is not a form element attribute.'));
    }
  }
}
