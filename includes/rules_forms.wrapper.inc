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

  public function __isset($name) {
    return isset($this->data[$name]);
  }

  public function __unset($name) {
    unset($this->data[$name]);
  }

  /**
   * Returns the form element type.
   */
  public function getElementType() {
    return isset($this->info['element info']['type']) ? $this->info['element info']['type'] : NULL;
  }

  /**
   * Returns the form element name for use in form_set_error().
   */
  public function getElementName() {
    return isset($this->info['element info']['name']) ? $this->info['element info']['name'] : '';
  }

  /**
   * Returns the form element key used in the form array.
   */
  public function getElementKey() {
    if (isset($this->info['element info']['name'])) {
      if (strpos($this->info['element info']['name'], '][') === FALSE) {
        return $this->info['element info']['name'];
      }
      $name = array_pop(explode('][', $this->info['element info']['name']));
      return $name;
    }
    return NULL;
  }

}

/**
 * Wrapper class for form element attributes.
 */
class RulesFormsAttributeWrapper extends EntityValueWrapper {

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
