<?php

class RulesFormsFormEvaluator {

  protected $elementInfo;

  public function __construct() {
    module_load_include('inc', 'rules_forms', 'includes/rules_forms.info');
    $this->elementInfo = rules_forms_get_element_info();
  }

  /**
   * Evaluates the elements of a form to identify its structure in property info.
   *
   * @param array $form
   *   The form array to evaluate.
   *
   * @return array
   *   An array of property info for use in hook_rules_data_info().
   */
  public function evaluate($form) {
    $info = array();
    $info['properties'] = array(
      'form' => array(),
      'form_state' => array(
        'values' => array(
          'type' => 'struct',
          'label' => 'values',
          'computed' => TRUE,
          'getter callback' => 'entity_property_verbatim_get',
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
        $info = $this->elementInfo[$form[$key]['#type']];
        $defaults = array(
          'label' => isset($form[$key]['#title']) ? $info['label'] .': '. $form[$key]['#title'] : $info['label'],
          'computed' => TRUE,
          'auto creation' => 'rules_forms_create_structure',
          'getter callback' => 'entity_property_verbatim_get',
        );
  
        // All form elements are structures since they have attribute property info.
        $form_properties[$key] = $defaults + array(
          'type' => 'form_' . $form[$key]['#type'],
          'setter callback' => 'entity_property_verbatim_set',
          'element info' => $info['element info'] + array(
            'type' => $form[$key]['#type'],
            'name' => isset($form[$key]['#parents']) ? implode('][', $form[$key]['#parents']) : $key,
          ),
          'property info' => $info['properties'],
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
          $form_state_arg = &$form_state_properties;
        }
        $this->buildPropertyInfo($form[$key], $form_properties[$key]['property info'], $form_state_arg, $is_tree);
      }
      // Handle cases where an array is not necessarily a form element with a #type.
      // We just identify it as a 'struct' with no properties except the sub arrays.
      elseif (!isset($form[$key]['#type'])) {
        $defaults = array(
          'type' => 'form_element',
          'label' => $key,
          'computed' => TRUE,
          'auto creation' => 'rules_forms_create_structure',
          'getter callback' => 'entity_property_verbatim_get',
        );
        // In this case, properties are filtered based on the attributes that are available.
        $form_properties[$key] = $defaults + array(
          'setter callback' => 'entity_property_verbatim_set',
          'element info' => array(
            'type' => NULL,
            'name' => isset($form[$key]['#parents']) ? implode('][', $form[$key]['#parents']) : $key,
            'data type' => FALSE,
          ),
          'property info' => rules_forms_filter_element_properties($form[$key]),
        );
        if ($tree) {
          $form_state_properties[$key] = $defaults + array('property info' => array());
          $form_state_arg = &$form_state_properties[$key]['property info'];
        }
        else {
          $form_state_arg = &$form_state_properties;
        }
        $this->buildPropertyInfo($form[$key], $form_properties[$key]['property info'], $form_state_arg, $tree);
      }
    }
  }

}
