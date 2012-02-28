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
 */
function rules_action_property_value_form_alter(&$form, &$form_state, $options, RulesAbstractPlugin $element) {
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
    // TODO: improve display
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
 * Custom validation callback for the data set action.
 */
function rules_forms_action_property_set_validate(RulesAbstractPlugin $element) {
  $element->settings += array('data:select' => NULL);
  $info = $element->applyDataSelector($element->settings['data:select'])->info();
  if (strpos($element->settings['data:select'], ':') !== FALSE) {
    if (empty($info['setter callback'])) {
      throw new RulesIntegrityException(t('The selected data property doesn\'t support writing.'), array($element, 'parameter', 'data'));
    }
    // @TODO Add the key 'rules forms element property' to property info.
    elseif (!empty($info['form element property'])) {
      throw new RulesIntegrityException(t('The selected data property is not a form element property.', array($element, 'parameter', 'data')));
    }
  }
}
