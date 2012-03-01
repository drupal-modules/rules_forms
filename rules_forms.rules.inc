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
 * Implements hook_rules_condition_info().
 */
function rules_forms_rules_condition_info() {
  return RulesFormsRulesController::conditionInfo();
}

/**
 * Implements hook_rules_action_info().
 */
function rules_forms_rules_action_info() {
  return RulesFormsRulesController::actionInfo();
}

/**
 * Implements hook_rules_data_info().
 */
function rules_forms_rules_data_info() {
  $data_info = array();
  $data_info['form_element'] = array(
    'label' => t('Form element'),
    'group' => t('Rules Forms'),
    'is wrapped' => TRUE,
    'parent' => 'struct',
  );
}

/**
 * Implements hook_rules_data_info_alter().
 */
function rules_forms_rules_data_info_alter(&$data_info) {
  // @TODO: Add UI classes that limit data selection for form or
  // form_state variable types in Rules Forms conditions and actions.
  $data_info['form'] = array(
    'label' => t('form'),
    'group' => t('Rules Forms'),
    'is wrapped' => TRUE,
  );
  $data_info['form_state'] = array(
    'label' => t('form state'),
    'group' => t('Rules Forms'),
    'is wrapped' => TRUE,
  );
}

/**
 * Form alter callback for the data_set action.
 * This function was largely taken from Rules.
 */
function rules_forms_property_value_form_alter(&$form, &$form_state, $options, RulesAbstractPlugin $element) {
  if (!empty($options['init']) && !isset($form_state['rules_element_step'])) {
    $form['negate']['#access'] = FALSE;
    unset($form['parameter']['value']);
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
 * Validation callback for get property actions.
 */
function rules_forms_property_get_validate(RulesAbstractPlugin $element) {
  rules_forms_property_validate($element, 'get');
}

/**
 * Validation callback for set property actions.
 */
function rules_forms_property_set_validate(RulesAbstractPlugin $element) {
  rules_forms_property_validate($element, 'set');
}

/**
 * Validation callback for determining whether selected data
 * is a form element property.
 */
function rules_forms_property_validate(RulesAbstractPlugin $element, $action) {
  $element->settings += array('data:select' => NULL);
  $info = $element->applyDataSelector($element->settings['data:select'])->info();
  if (strpos($element->settings['data:select'], ':') !== FALSE) {
    // 'getter' or 'setter' callback.
    if (empty($info[$action .'ter callback'])) {
      throw new RulesIntegrityException(t('The selected data property doesn\'t support '. ($action == 'set' ? 'writing' : 'reading') .'.'), array($element, 'parameter', 'data'));
    }
    // Form element properties should have the 'form property' set.
    // This is automatically set on the return values of
    // hook_rules_forms_element_property_info() implementations.
    elseif (empty($info['form property'])) {
      throw new RulesIntegrityException(t('The selected data property is not a form element property.', array($element, 'parameter', 'data')));
    }
  }
}

/**
 * Validation callback for determining whether selected data
 * is a form element.
 */
function rules_forms_element_validate(RulesAbstractPlugin $element) {
  $element->settings += array('data:select' => NULL);
  $info = $element->applyDataSelector($element->settings['data:select'])->info();
  if (strpos($element->settings['data:select'], ':') !== FALSE) {
    if (empty($info['getter callback'])) {
      throw new RulesIntegrityException(t('The selected data property doesn\'t support reading.'), array($element, 'parameter', 'data'));
    }
    // Form element structures will have the 'form element' key set.
    elseif ($info['type'] !== 'struct' || empty($info['form element'])) {
      throw new RulesIntegrityException(t('The selected data property is not a form element.', array($element, 'parameter', 'data')));
    }
  }
}

/**
 * Validation callback for determining whether selected data
 * is a form value.
 */
function rules_forms_value_validate(RulesAbstractPlugin $element) {
  $element->settings += array('data:select' => NULL);
  $info = $element->applyDataSelector($element->settings['data:select'])->info();
  if (strpos($element->settings['data:select'], ':') !== FALSE) {
    if (empty($info['getter callback'])) {
      throw new RulesIntegrityException(t('The selected data property doesn\'t support reading.'), array($element, 'parameter', 'data'));
    }
    // Form value properties will have the 'form value' key set.
    elseif (empty($info['form value'])) {
      throw new RulesIntegrityException(t('The selected data property is not a form value.', array($element, 'parameter', 'data')));
    }
  }
}
