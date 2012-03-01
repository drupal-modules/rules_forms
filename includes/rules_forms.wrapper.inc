<?php

class RulesFormsMetadataWrapper extends EntityStructureWrapper {

  /**
   * Overrides EntityStructureWrapper::get() to ensure that Rules Forms
   * wrappers are loaded in Rules Forms events.
   *
   * @return
   *   An instance of EntityMetadataWrapper.
   */
  public function get($name) {
    // Look it up in the cache if possible.
    if (!array_key_exists($name, $this->cache)) {
      if ($info = $this->getPropertyInfo($name)) {
        $info += array(
          'form type' => '',
          'parent' => $this,
          'name' => $name,
          'langcode' => $this->langcode,
          'property defaults' => array(),
        );
        $info['property defaults'] += $this->info['property defaults'];
        $this->cache[$name] = rules_forms_metadata_wrapper($info['type'], NULL, $info);
      }
      else {
        throw new EntityMetadataWrapperException('There is no property ' . check_plain($name) . " for this entity.");
      }
    }
    return $this->cache[$name];
  }

}

/**
 * Provides a wrapper for form array entities.
 */
class RulesFormsFormDataWrapper extends RulesFormsMetadataWrapper {

  protected $formId;

  /**
   * Overrides EntityStructureWrapper::__construct(). Form and form state data
   * types require an object that implements RulesFormsArrayAccess interface.
   */
  public function __construct($type, $data, $info = array()) {
    if (!is_object($data) || !in_array('RulesFormsArrayAccess', class_implements($data))) {
      throw new EntityMetadataWrapperException('Form and form state data must be passed as an instance of a class that implements the RulesFormsArrayAccess interface.');
    }
    $this->formId = $info['form_id'];
    $property_info = entity_get_property_info();
    $info += array(
      'form_id' => $this->formId,
      'property info' => $property_info['form']['bundles'][$this->formId]['properties'],
    );
    parent::__construct($type, $data, $info);
  }

}

/**
 * Provides a wrapper for form elements.
 */
class RulesFormsElementWrapper extends RulesFormsMetadataWrapper {}

/**
 * Provides a wrapper for element attributes.
 */
class RulesFormsAttributeWrapper extends RulesFormsMetadataWrapper {}

/**
 * Provides a wrapper for form values.
 */
class RulesFormsValueWrapper extends EntityValueWrapper {}

/**
 * Metadata wrapper for form and form_state entity data.
 */
class RulesFormsFormWrapper {

  protected $type, $info, $formId;

  protected static $propertyInfo;

  /**
   * Constructor adds default values to the $info array.
   *
   * @param array $info
   *   An array of information related to an active Rules Forms form.
   */
  public function __construct($type, array $info = array()) {
    $this->type = $type;
    $info += array(
      'form_id' => '',
      'label' => '',
      'properties' => array('form' => array(), 'form_state' => array()),
      'settings' => array(),
      'rebuild' => TRUE,
    );
    $this->formId = $info['form_id'];
    $this->info = $info;
  }

  /**
   * Returns the form ID.
   */
  public function getIdentifier() {
    return $this->info['form_id'];
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
   * Returns property info for the form.
   *
   * @param string $key
   *   An optional string identifying the specific property info to return.
   *
   * @return array
   *   An array of property info to be used in hook_entity_property_info().
   *   Arrays are keyed by form and form_state keys, each containing properties
   *   of the form and form_state bundle that is keyed by the form ID.
   */
  public function getPropertyInfo($key = NULL) {
    if ($key !== NULL) {
      return isset($this->info['properties'][$key]) ? $this->info['properties'][$key] : array();
    }
    return $this->info['properties'];
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
    $this->info['properties'] = array(
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
    self::$propertyInfo = rules_forms_element_property_info();
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
    foreach (element_children($form) as $key) {
      if (isset($form[$key]['#type']) && array_key_exists($form[$key]['#type'], self::$propertyInfo)) {
        $defaults = array(
          'label' => isset($form[$key]['#title']) ? self::$propertyInfo[$form[$key]['#type']]['label'] .': '. $form[$key]['#title'] : self::$propertyInfo[$form[$key]['#type']]['label'],
          'computed' => TRUE,
          'getter callback' => 'rules_forms_get_attribute',
        );
  
        $form_properties[$key] = $defaults + array(
          'type' => 'struct',
          'setter callback' => 'rules_forms_set_attribute',
          'auto creation' => 'rules_forms_create_structure',
          'form type' => 'element',
          'property info' => self::$propertyInfo[$form[$key]['#type']]['properties'],
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
        self::buildPropertyInfo($form[$key], $form_properties[$key]['property info'], $form_state_arg, $is_tree);
      }
    }
  }
}
