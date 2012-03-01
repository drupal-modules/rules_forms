<?php

/**
 * Defines an interface for returning a reference to the internal
 * form data array.
 */
interface RulesFormsArrayAccess extends ArrayAccess {

  /**
   * Returns a reference to the internal form data array.
   */
  public function &getArray();

}

/**
 * Defines an entity class for wrapping form data.
 */
class RulesFormsArrayEntity extends Entity implements RulesFormsArrayAccess {

  public $formId, $formData;

  /**
   * Constructor.
   * @param string $form_id
   *   The form ID of the form entity.
   * @param array $form_data
   *   The form or form_state array.
   */
  public function __construct($type, $form_id, &$form_data) {
    $this->entityType = $type;
    $this->formId = $form_id;
    $this->formData = &$form_data;
    $this->setUp();
  }

  /**
   * @see ArrayAccess::offsetSet()
   */
  public function offsetSet($offset, $value) {
    if (is_null($offset)) {
      $this->formData[] = $value;
    }
    else {
      $this->formData[$offset] = $value;
    }
  }

  /**
   * @see ArrayAccess::offsetExists()
   */
  public function offsetExists($offset) {
    return isset($this->formData[$offset]);
  }

  /**
   * @see ArrayAccess::offsetUnset()
   */
  public function offsetUnset($offset) {
    unset($this->formData[$offset]);
  }

  /**
   * @see ArrayAccess::offsetGet()
   */
  public function offsetGet($offset) {
    return isset($this->formData[$offset]) ? $this->formData[$offset] : NULL;
  }

  /**
   * Returns a reference to the form data array.
   */
  public function &getArray() {
    return $this->formData;
  }

}
