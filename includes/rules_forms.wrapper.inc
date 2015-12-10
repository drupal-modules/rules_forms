<?php

/**
 * Base metadata wrapper for Rules Forms data structures.
 */
class RulesFormsStructureWrapper extends EntityStructureWrapper {

  /**
   * Overrides EntityStructureWrapper::get() to ensure data is properly wrapped.
   */
  public function get($name) {
    // Look it up in the cache if possible.
    if (!array_key_exists($name, $this->cache)) {
      // Try to get the property info without the hash. If this fails,
      // add the hash mark to the $name. This allows us to support chaining
      // with element attribute #names which are otherwise invalid PHP code.
      try {
        $info = $this->getPropertyInfo($name);
      }
      catch (EntityMetadataWrapperException $e) {
        $name = '#' . $name;
      }

      if (!empty($info) || $info = $this->getPropertyInfo($name)) {
        $info += array(
          'parent' => $this,
          'name' => $name,
          'langcode' => $this->langcode,
          'property defaults' => array(),
        );
        $info['property defaults'] += $this->info['property defaults'];

        // Check if this is a form element attribute and wrap it if necessary.
        if (!empty($info['attribute info'])) {
          $this->cache[$name] = new RulesFormsAttributeWrapper($info['type'], NULL, $info);
        }
        // Default to allowing Rules to wrap the data using data type info.
        else {
          $this->cache[$name] = rules_wrap_data(NULL, $info, TRUE);
        }
      }
      else {
        throw new EntityMetadataWrapperException('There is no property ' . check_plain($name) . " for this entity.");
      }
    }
    return $this->cache[$name];
  }

}

/**
 * Wrapper class for form arrays. For data is wrapped as an EntityMetadataArrayObject.
 */
class RulesFormsFormWrapper extends RulesFormsStructureWrapper {

  /**
   * Returns the form ID of the form.
   */
  public function getFormId() {
    $data = $this->value();
    if (isset($data['form_id'])) {
      return $data['form_id']['#value'];
    }
    return NULL;
  }

}
/**
 * Wrapper class for form state arrays. For data is wrapped as an EntityMetadataArrayObject.
 */
class RulesFormsFormStateWrapper extends RulesFormsStructureWrapper {}

/**
 * Wrapper class for form element arrays.
 */
class RulesFormsElementWrapper extends RulesFormsStructureWrapper {

  public function __construct($type, $data = NULL, $info = array()) {
    $info += array('element info' => array());
    $info['element info'] += array('data type' => 'text');
    parent::__construct($type, $data, $info);
  }

  /**
   * Magic method: Determine if a form element attribute is set.
   */
  public function __isset($name) {
    return isset($this->data[$name]);
  }

  /**
   * Magic method: Unset a form element attribute.
   */
  public function __unset($name) {
    unset($this->data[$name]);
  }

  /**
   * Returns element info as defined in element property info.
   *
   * @param string $name
   *   An optional name of a key to return.
   *
   * @return mixed
   *   An array of element info, or a specific property if $name is defined.
   */
  public function getElementInfo($name = NULL) {
    $info = $this->info();
    if (isset($info['element info'])) {
      if (isset($name)) {
        return isset($info['element info'][$name]) ? $info['element info'][$name] : NULL;
      }
      return $info['element info'];
    }
    return array();
  }

  /**
   * Returns the data type of the form element.
   */
  public function getElementDataType() {
    return $this->info['element info']['data type'];
  }

  /**
   * Returns the form element name for use in form_set_error().
   *
   * @return string
   *   The form element name as required by form_set_error().
   *
   * @throws EntityMetadataWrapperException
   *   The form element 'name' was not found.
   */
  public function getElementName() {
    if (!isset($this->info['element info']['name'])) {
      throw new EntityMetadataWrapperException('Unknown form element name for ' . check_plain($this->info['name']) . '.');
    }
    return $this->info['element info']['name'];
  }

  /**
   * Returns an element key.
   *
   * @param string $name
   *   The name of the element key to return.
   *
   * @return string|false
   *   The indicated key or FALSE if none was found.
   */
  public function getElementKey($name) {
    if (isset($this->info['element info']['keys'][$name])) {
      return $this->info['element info']['keys'][$name];
    }
    return FALSE;
  }

  /**
   * Returns the current value of the form element either from #value or #default_value.
   * If neither of these is set we use the 'empty value' key.
   *
   * @return mixed
   *   The element value retrieved from defined keys. If neither the value or default value
   *   is defined, the empty value will be returned.
   *
   * @throws EntityMetadataWrapperException
   *   The form element does not contain a value.
   */
  public function getElementValue() {
    if ($this->info['element info']['data type'] === FALSE) {
      throw new EntityMetadataWrapperException('Form element ' . check_plain($this->info['name']) . ' does not contain a value.');
    }
    $data = $this->value();
    $value_key = $this->getElementKey('value');
    if ($value_key && isset($data[$value_key])) {
      return $data[$value_key];
    }
    elseif ($default_value_key = $this->getElementKey('default_value') && isset($data[$default_value_key])) {
      return $data[$default_value_key];
    }
    return $this->getElementEmptyValue();
  }

  /**
   * Sets the value of the form element.
   *
   * @param mixed $value
   *   The value to set.
   *
   * @return RulesFormsElementWrapper
   *   The called object.
   *
   * @throws EntityMetadataWrapperException
   *   The form element does not support a value.
   */
  public function setElementValue($value) {
    if ($this->info['element info']['data type'] == FALSE || $value_key = $this->getElementKey('value') === FALSE) {
      throw new EntityMetadataWrapperException('Form element ' . check_plain($this->info['name']) . ' does not support a value.');
    }
    else {
      $this->get($value_key)->set($value);
    }
    return $this;
  }

  /**
   * Sets the default value of the form element.
   *
   * @param mixed $value
   *   The value to set.
   *
   * @return RulesFormsElementWrapper
   *   The called object.
   *
   * @throws EntityMetadataWrapperException
   *   The form element does not support a default value.
   */
  public function setElementDefaultValue($value) {
    if ($this->info['element info']['data type'] == FALSE || $default_value_key = $this->getElementKey('default_value') === FALSE) {
      throw new EntityMetadataWrapperException('Form element ' . check_plain($this->info['name']) . ' does not support a default value.');
    }
    else {
      $this->get($default_value_key)->set($value);
    }
    return $this;
  }

  /**
   * Returns the value of the form element when #value and #default_value are not set.
   */
  public function getElementEmptyValue() {
    return $this->info['element info']['empty'];
  }

}

/**
 * Wrapper class for form element attributes.
 */
class RulesFormsAttributeWrapper extends EntityValueWrapper {

  /**
   * Magic method: Retrieve data from parent form element by calling
   * getElement* methods directly. Setter methods are not supported here.
   */
  public function __call($method, array $args) {
    if (substr($method, 0, 10) === 'getElement' && method_exists($this->info['parent'], $method)) {
      return call_user_func_array(array($this->info['parent'], $method), $args);
    }
  }

  /**
   * Overrides EntityMetadataWrapper::set() to handle special circumstances
   * of form attributes.
   */
  public function set($value) {
    // The #options attribute is identified in property info as a 'text' property
    // in order to provide the proper interface. However, that text is converted
    // to an array, so if this is an #options attribute we just check to ensure that
    // we're either receiving text or an array.
    if ($this->info['name'] == '#options') {
      if (!is_string($value) && !is_array($value)) {
        throw new EntityMetadataWrapperException('Invalid data value given. Be sure it matches the required data type and format.');
      }
    }
    elseif (!$this->validate($value)) {
      throw new EntityMetadataWrapperException('Invalid data value given. Be sure it matches the required data type and format.');
    }
    $this->clear();
    $this->data = $value;
    $this->updateParent($value);
    return $this;
  }

}
