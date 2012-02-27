<?php

class RulesFormsElement {

  public static function getProperty($element, array $options, $name, $type) {
    return isset($element[$name]) ? $element[$name] : NULL;
  }

  public static function setProperty($element, $name, $value) {
    $element[$name] = $value;
  }

}
