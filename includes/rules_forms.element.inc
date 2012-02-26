<?php

class RulesFormsElement extends Entity {

}

class RulesFormsElementController extends EntityAPIController {

  public function create(array $values = array()) {
    return new RulesFormsElement($values, 'rules_form_element');
  }

}

class RulesFormsElementRulesController extends EntityDefaultRulesController {

  public function eventInfo() {
    $form_info = rules_forms_get_form_info();
    $defaults = array(
      'group' => 'Rules Forms',
      'access callback' => 'rules_forms_integration_access',
    );

    $events = array();
    foreach ($form_info as $form_id => $info) {
      if ($info['buttons'] && isset($info['submit'])) {
        foreach ($info['submit'] as $element_id => $label) {
          // Add button validate events.
          $events['rules_forms_'. $form_id .'_button_'. str_replace(':', '_', $element_id) .'_validate'] = $defaults + array(
            'label' => t('@form @button button is being validated', array('@form' => $info['label'], '@button' => strtolower($label))),
            'variables' => rules_forms_event_variables() + rules_forms_element_variables($form_id),
          );
          // Add button submit events.
          $events['rules_forms_'. $form_id .'_button_'. str_replace(':', '_', $element_id) .'_submit'] = $defaults + array(
            'label' => t('@form @button button is being submitted', array('@form' => $info['label'], '@button' => strtolower($label))),
            'variables' => rules_forms_event_variables() + rules_forms_element_variables($form_id),
          );
        }
      }
    }
    return $events;
  }

}
