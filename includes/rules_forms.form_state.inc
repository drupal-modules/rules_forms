<?php

class RulesFormsFormState extends Entity {

}

class RulesFormsFormStateController extends EntityAPIController {

  public function create(array $values = array()) {
    return new RulesFormsFormState($values, 'rules_form_state');
  }

}

class RulesFormsFormStateRulesController extends EntityDefaultRulesController {

  public function eventInfo() {
    return array();
  }

}