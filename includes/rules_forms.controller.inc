<?php

/**
 * Rules controller for Rules Forms entities.
 */
class RulesFormsRulesController extends EntityDefaultRulesController {

  protected $form_info = array();

  /**
   * Loads stored form information.
   */
  public function __construct($type) {
    $this->form_info = rules_forms_get_form_info();
  }

  /**
   * Returns event info for all active forms.
   */
  public function eventInfo() {
    $events = array();
    foreach ($this->form_info as $form_id => $info) {
      $defaults = array(
        'group' => 'Rules Forms',
        'access callback' => 'rules_forms_integration_access',
        'variables' => array(
          'form' => array(
            'type' => 'form',
            'label' => $info['label'],
            'bundle' => $form_id,
          ),
          'form_state' => array(
            'type' => 'form_state',
            'label' => $info['label'],
            'bundle' => $form_id,
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
   * Defines condition info for all forms.
   */
  public static function conditionInfo() {
    $conditions = array();
    $conditions['rules_forms_element_value'] = array(
      'label' => t('Form element has value'),
      'group' => 'Rules Forms',
      'parameter' => array(
        'form' => array(
          'type' => 'form',
          'label' => t('Form'),
        ),
        'value' => array(
          'type' => 'form_value',
          'label' => t('Value(s)'),
          'optional' => TRUE,
          'description' => t('Value(s) assigned to the form element. If the form element allows multiple values, enter one value per line.'),
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
        'access' => 'rules_forms_integration_access',
        'form_alter' => 'rules_forms_element_form_alter',
      ),
    );
    $conditions['rules_forms_element_changed'] = array(
      'label' => t('Form element value has changed'),
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
      ),
      'base' => 'rules_forms_condition_element_changed',
      'callbacks' => array(
        'access' => 'rules_forms_integration_access',
        'form_alter' => 'rules_forms_element_form_alter',
      ),
    );
    $conditions['rules_forms_button_clicked'] = array(
      'label' => t('Form button was clicked'),
      'group' => 'Rules Forms',
      'parameter' => array(
        'form_state' => array(
          'type' => 'form_state',
          'label' => t('Form state'),
        ),
      ),
      'base' => 'rules_forms_condition_button_clicked',
      'callbacks' => array(
        'access' => 'rules_forms_integration_access',
        'form_alter' => 'rules_forms_element_form_alter',
      ),
    );
  return $conditions;
  }

  /**
   * Defines action info for all forms.
   */
  public static function actionInfo() {
    $actions = array();
    $actions['rules_forms_set_property'] = array(
      'label' => t('Set a property of an element in the form'),
      'group' => 'Rules Forms',
      'parameter' => array(
        'form' => array('type' => 'form', 'label' => t('Form')),
      ),
      'base' => 'rules_forms_action_set_property',
      'callbacks' => array(
        'access' => 'rules_forms_integration_access',
      ),
    );
    $actions['rules_forms_set_error'] = array(
      'label' => t('Set an error on the form'),
      'group' => 'Rules Forms',
      'parameter' =>  array(
        'form' => array('type' => 'form', 'label' => t('Form')),
        'message' => array(
          'type' => 'text',
          'label' => t('Message'),
          'optional' => TRUE,
          'description' => t('The message that should be displayed to the user.'),
        ),
      ),
      'base' => 'rules_forms_action_set_error',
      'access callback' => 'rules_forms_integration_access',
    );
    $actions['rules_forms_redirect'] = array(
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
    return $actions;
  }

}

class RulesFormsFormController extends EntityAPIController {}

class RulesFormsFormStateController extends EntityAPIController {}
