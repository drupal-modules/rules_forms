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

  public $form_id, $form_data;

  /**
   * Constructor.
   * @param string $form_id
   *   The form ID of the form entity.
   * @param array $form_data
   *   The form or form_state array.
   */
  public function __construct($type, $form_id, &$form_data) {
    $this->entityType = $type;
    $this->form_id = $form_id;
    $this->form_data = &$form_data;
    $this->setUp();
  }

  /**
   * @see ArrayAccess::offsetSet()
   */
  public function offsetSet($offset, $value) {
    if (is_null($offset)) {
      $this->form_data[] = $value;
    }
    else {
      $this->form_data[$offset] = $value;
    }
  }

  /**
   * @see ArrayAccess::offsetExists()
   */
  public function offsetExists($offset) {
    return isset($this->form_data[$offset]);
  }

  /**
   * @see ArrayAccess::offsetUnset()
   */
  public function offsetUnset($offset) {
    unset($this->form_data[$offset]);
  }

  /**
   * @see ArrayAccess::offsetGet()
   */
  public function offsetGet($offset) {
    return isset($this->form_data[$offset]) ? $this->form_data[$offset] : NULL;
  }

  /**
   * Returns a reference to the form data array.
   */
  public function &getArray() {
    return $this->form_data;
  }

}
