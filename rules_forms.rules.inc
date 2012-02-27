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
  $data_info['form'] = array(
    'label' => t('form'),
    'group' => t('Rules Forms'),
  );
  $data_info['form_state'] = array(
    'label' => t('form state'),
    'group' => t('Rules Forms'),
  );
}

/**
 * Alters the form for action: Set element property. Taken from Rules module.
 */
function rules_forms_element_form_alter(&$form, &$form_state, $options, RulesAbstractPlugin $element) {
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
  $form['parameter']['element']['settings']['element']['#ajax'] = $form['reload']['#ajax'] + array(
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
    unset($form['negate']);
  }
  else {
    // For the Condition: Element has value form, unset the regex option
    // if it doesn't make sense for the type of field selected.
    if ($element->getElementName() == 'rules_forms_element_value') {
      $type = substr($element->settings['element'], 0, strpos($element->settings['element'], ':'));
      if ($type != 'textfield' && $type != 'textarea') {
        $form['parameter']['regex']['#access'] = FALSE;
        $form['parameter']['regex']['settings']['regex']['#type'] = 'value';
        $form['parameter']['regex']['settings']['regex']['#value'] = 0;
      }
    }
    // Hide the reload button in case js is enabled and it's not the first step.
    $form['reload']['#attributes'] = array('class' => array('rules-hide-js'));
  }
}

/**
 * Rules Forms integration access callback.
 */
function rules_forms_integration_access($type, $name) {
  return user_access('administer rules forms rules');
}
