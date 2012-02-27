<?php

class RulesFormsWrapper extends EntityStructureWrapper {
  
  /**
   * Overriding the constructor method ensures property info for the form is included.
   */
  public function __construct($type, $data = NULL, $info = array()) {
    parent::__construct($type, $data, $info);
    $this->info += array('property defaults' => array());
    $property_info = rules_forms_entity_property_info();
    $info += array('property info' => $property_info[$type]['bundles'][$data->form_id]['properties']);
    $this->propertyInfo['properties'] = $info['property info'];
  }

}

class RulesFormsFormWrapper extends RulesFormsWrapper {}

class RulesFormsFormStateWrapper extends RulesFormsWrapper {}
