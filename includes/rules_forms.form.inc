<?php

class RulesFormsForm extends Entity {

}

class RulesFormsFormController extends EntityAPIController {

  public function create(array $values = array()) {
    return new RulesFormsForm($values, 'rules_form');
  }

}

class RulesFormsFormRulesController extends EntityDefaultRulesController {

  public function eventInfo() {
    $form_info = rules_forms_get_form_info();
    $defaults = array(
      'group' => 'Rules Forms',
      'access callback' => 'rules_forms_integration_access',
    );

    $events = array();
    foreach ($form_info as $form_id => $info) {
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

}
