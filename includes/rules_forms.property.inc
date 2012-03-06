<?php

class RulesFormsFormEvaluator {

  protected $elementInfo, $attributeInfo;

  public function __construct() {
    $this->elementInfo = rules_forms_get_element_info();
  }

  public function evaluate($form) {
    $info = array();
    $info['properties'] = array(
      'form' => array(),
      'form_state' => array(
        'values' => array(
          'type' => 'struct',
          'label' => 'values',
          'computed' => TRUE,
          'getter callback' => 'rules_forms_get_attribute',
          'auto creation' => 'rules_forms_create_structure',
          'property info' => array(),
        ),
      ),
    );
    $this->buildPropertyInfo($form, $info['properties']['form'], $info['properties']['form_state']['values']['property info']);
    return $info;
  }

  /**
   * Builds form element property info.
   *
   * This data is used in hook_entity_property_info() to identify the
   * form structure in the form and form_state entity types. This method
   * does a recursive call on form element children to map the entire
   * form array structure in the form of property info. Each form element
   * is identified as a 'struct' type, and property info for the form
   * element is retrieved from hook_rules_forms_element_property_info().
   * That way, each *possible* form element attribute is represented as
   * a property of the element structure. This allows specific properties
   * to be selected, read, and written in Rules.
   *
   * @param array $form
   *   The form array.
   * @param array $form_properties
   *   The form properties array. This argument is passed by reference.
   * @param array $form_state_properties
   *   The form_state properties array. This argument is passed by reference.
   * @param bool $tree
   *   A boolean value indicating whether the current position of the form
   *   array is a #tree. This is determined by setting this value to TRUE
   *   once an element has #tree = TRUE. This value will be passed recursively
   *   to identify all children as #tree in order to properly structure the
   *   $form_state array.
   */
  protected function buildPropertyInfo($form, &$form_properties, &$form_state_properties, $tree = FALSE) {
    foreach (element_children($form) as $key) {
      if (isset($form[$key]['#type']) && array_key_exists($form[$key]['#type'], $this->elementInfo)) {
        $defaults = array(
          'label' => isset($form[$key]['#title']) ? $this->elementInfo[$form[$key]['#type']]['label'] .': '. $form[$key]['#title'] : $this->elementInfo[$form[$key]['#type']]['label'],
          'computed' => TRUE,
          'getter callback' => 'rules_forms_get_attribute',
        );
  
        $form_properties[$key] = $defaults + array(
          'type' => 'struct',
          'setter callback' => 'rules_forms_set_attribute',
          'auto creation' => 'rules_forms_create_structure',
          'form type' => 'element',
          'property info' => $this->elementInfo[$form[$key]['#type']]['properties'],
        );
  
        $is_tree = ($tree == TRUE || !empty($form[$key]['#tree']));
        $form_state_properties[$key] = $defaults;
  
        // If this is a tree then property info needs to be stored as such.
        $children = element_children($form[$key]);
        if ($is_tree && !empty($children)) {
          $form_state_properties[$key]['type'] = 'struct';
          $form_state_properties[$key]['property info'] = array();
          $form_state_arg = &$form_state_properties[$key]['property info'];
        }
        else {
          $form_state_properties[$key]['type'] = 'text';
          $form_state_properties[$key]['form type'] = 'value';
          $form_state_arg = &$form_state_properties;
        }
        $this->buildPropertyInfo($form[$key], $form_properties[$key]['property info'], $form_state_arg, $is_tree);
      }
    }
  }

}
