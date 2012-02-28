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
    'ui class' => 'RulesFormsFormDataUI',
  );
  $data_info['form_state'] = array(
    'label' => t('form state'),
    'group' => t('Rules Forms'),
    'is wrapped' => TRUE,
    'ui class' => 'RulesFormsFormStateDataUI',
  );
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
