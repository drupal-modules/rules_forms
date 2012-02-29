<?php

/**
 * Provides a wrapper for form array entities.
 */
class RulesFormsDataWrapper extends EntityStructureWrapper {}

/**
 * Provides a wrapper for active form information.
 */
class RulesFormsMetadataWrapper {

  protected $form_id, $info;

  /**
   * Constructor adds default values to the $info array.
   *
   * @param array $info
   *   An array of information related to an active Rules Forms form.
   */
  public function __construct(array $info) {
    $info += array(
      'form_id' => '',
      'label' => '',
      'properties' => array('form' => array(), 'form_state' => array()),
      'settings' => array(),
      'rebuild' => TRUE,
    );
    $this->form_id = $info['form_id'];
    $this->info = $info;
  }

  /**
   * Returns the form ID.
   */
  public function getIdentifier() {
    return $this->form_id;
  }

  /**
   * Returns the entire $info array.
   */
  public function info() {
    return $this->info;
  }

  /**
   * Returns the form label.
   */
  public function label() {
    return $this->info['label'];
  }

  /**
   * Returns additional settings.
   */
  public function getSettings() {
    return $this->info['settings'];
  }

  /**
   * Returns a specific additional setting.
   *
   * @param string $key
   *   The key of the setting to return.
   *
   * @return mixed
   *   The requested setting.
   */
  public function get($key) {
    return isset($this->info['settings'][$key]) ? $this->info['settings'][$key] : NULL;
  }

  /**
   * Magic method: Returns a specific additional setting.
   *
   * @see RulesFormsMetadataWrapper::get()
   */
  public function __get($key) {
    return $this->get($key);
  }

  /**
   * Sets a specific additional setting.
   *
   * @param string $key
   *   The key of the setting to set.
   * @param mixed $value
   *   The settin value.
   *
   * @return RulesFormsMetadataWrapper
   *   The called object.
   */
  public function set($key, $value) {
    $this->info['settings'][$key] = $value;
    return $this;
  }

 /**
  * Magic method: Sets a specific additional setting.
  *
  * @see RulesFormsMetadataWrapper::get()
  */
  public function __set($key, $value) {
    $this->set($key, $value);
  }

  /**
   * Returns a boolean value indicating whether property info needs to be rebuilt.
   */
  public function needsRebuild() {
    return !empty($this->info['rebuild']);
  }

  /**
   * Rebuilds property info for the form.
   *
   * Property info that is built here is used in hook_entity_property_info()
   * to identify the structure of the form for use in Rules data selectors.
   *
   * @param array $form
   *   The form array.
   *
   * @see RulesFormsMetadataWrapper::buildPropertyInfo()
   */
  public function rebuildPropertyInfo($form) {
    // Note: This call to retrieve element property info is a hacky fix to
    // that function's return. When calling the function right after the cache
    // is cleared, a small and invalid portion of the property info array is
    // returned. We have to call it first here to ensure the proper array is
    // returned in rules_forms_build_element_property_info().
    rules_forms_element_property_info();

    $this->info['properties'] = array(
      'form' => array(),
      'form_state' => array(
        'values' => array(
          'type' => 'struct',
          'label' => 'values',
          'computed' => TRUE,
          'getter callback' => 'rules_forms_get_property',
          'auto creation' => 'rules_forms_create_structure',
          'property info' => array(),
        ),
      ),
    );
    self::buildPropertyInfo($form, $this->info['properties']['form'], $this->info['properties']['form_state']['values']['property info']);
    $this->info['rebuild'] = FALSE;
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
  protected static function buildPropertyInfo($form, &$form_properties, &$form_state_properties, $tree = FALSE) {
    $element_properties = rules_forms_element_property_info();
    foreach (element_children($form) as $key) {
      if (isset($form[$key]['#type']) && array_key_exists($form[$key]['#type'], $element_properties)) {
        $defaults = array(
          'label' => isset($form[$key]['#title']) ? "{$form[$key]['#type']}: {$form[$key]['#title']}" : $form[$key]['#type'],
          'computed' => TRUE,
          'getter callback' => 'rules_forms_get_property',
        );

        $form_properties[$key] = $defaults + array(
          'type' => 'struct',
          'setter callback' => 'rules_forms_set_property',
          'auto creation' => 'rules_forms_create_structure',
          'form element' => TRUE,
          'property info' => $element_properties[$form[$key]['#type']],
        );
    
        $is_tree = ($tree == TRUE || !empty($form[$key]['#tree']));
        $form_state_properties[$key] = $defaults;
    
        // If this is a tree then property info needs to be stored as such.
        $children = element_children($form[$key]);
        if ($is_tree && !empty($children)) {
          $form_state_properties[$key]['type'] = 'struct';
          $form_state_properties['property info'] = array();
          $form_state_arg = &$form_state_properties['property info'];
        }
        else {
          $form_state_properties[$key]['type'] = 'text';
          $form_state_properties[$key]['form value'] = TRUE;
          $form_state_arg = &$form_state_properties;
        }
        self::buildPropertyInfo($form[$key], $form_properties[$key]['property info'], $form_state_arg, $is_tree);
      }
    }
  }

  /**
   * Returns property info for the form.
   *
   * @param string $form
   *   An optional string identifying the specific property info to return.
   *   Property info is stored for form and form_state
   */
  public function getPropertyInfo($form = NULL) {
    if ($form !== NULL) {
      return isset($this->info['properties'][$form]) ? $this->info['properties'][$form] : array();
    }
    return $this->info['properties'];
  }

}
