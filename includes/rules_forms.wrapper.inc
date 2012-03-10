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
        $this->cache[$name] = rules_wrap_data(NULL, $info, TRUE);
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
    $info = $this->info();
    return isset($info['element info']['type']) ? $info['element info']['type'] : NULL;
  }

  /**
   * Returns the form element name for use in form_set_error().
   */
  public function getElementName() {
    $info = $this->info();
    return isset($info['element info']['name']) ? $info['element info']['name'] : '';
  }

  /**
   * Returns the form element key used in the form array.
   */
  public function getElementKey() {
    $info = $this->info();
    if (isset($info['element info']['name'])) {
      if (strpos($info['element info']['name'], '][') === FALSE) {
        return $info['element info']['name'];
      }
      $name = array_pop(explode('][', $info['element info']['name']));
      return $name;
    }
    return NULL;
  }

}
