<?php

/**
 * @file
 * Evaluation functions for Rules Forms module.
 */

/**
 * Condition: Check a form element value.
 */
function rules_forms_condition_element_value($form, $form_state, $element, $value) {
  $form_element = &_rules_forms_get_element($form, $element);
  if (isset($form_element['#value'])) {
    // Multiple values come in as array
    if (is_array($form_element['#value'])) {
      $lines = explode("\r\n", $value);
      return rules_forms_equal_array_values($lines, $form_element['#value']);
    }
    return $form_element['#value'] === $value;
  }
  if (isset($form_element['#default_value'])) {
    if (is_array($form_element['#default_value'])) {
      $lines = explode("\r\n", $value);
      return rules_forms_equal_array_values($lines, $form_element['#default_value']);
    }
    return $form_element['#default_value'] === $value;
  }
  return FALSE;
}

/**
 * Returns TRUE if both arrays contain the same values, regardless of their keys
 * and value ordering.
 */
function rules_forms_equal_array_values($array1, $array2) {
  $diff1 = array_diff($array1, $array2);
  $diff2 = array_diff($array2, $array1);
  return empty($diff1) && empty($diff2);
}

/**
 * Action: Set the redirect target.
 */
function rules_forms_action_redirect($form_state, $path, $query, $fragment) {
  $form_state['redirect'] = array($path, array('query' => array($query), 'fragment' => $fragment));
}

/**
 * Action: Hide a form element.
 */
function rules_forms_action_hide_element($form, $hide) {
  $form_element = &_rules_forms_get_element($form, $hide);
  $form_element['#access'] = FALSE;
}

/**
 * Action: Adjust weight of a form element.
 */
function rules_forms_action_adjust_weight($form, $element, $weight) {
  $form_element = &_rules_forms_get_element($form, $element);
  $form_element['#weight'] = $weight;
}

/**
 * Action: Insert prefix/suffix HTML code.
 */
function rules_forms_action_prefix_suffix_html($form, $element, $prefix, $suffix) {
  if (empty($element)) {
    if (!empty($prefix)) {
      $form['#prefix'] = $prefix;
    }
    if (!empty($suffix)) {
      $form['#suffix'] = $suffix;
    }
  }
  else {
    $form_element = &_rules_forms_get_element($form, $element);
    if (!empty($prefix)) {
      $form_element['#prefix'] = $prefix;
    }
    if (!empty($suffix)) {
      $form_element['#suffix'] = $suffix;
    }
  }
}

/**
 * Action: Set form error.
 */
function rules_forms_action_set_error($form, $element, $message) {
  if (substr($element, strlen($element)-1) === ']') {
    $form_element = &_rules_forms_get_element($form, $element);
    if (isset($form_element['#parents'])) {
      $element = implode('][', $form_element['#parents']);
    }
    else {
      // Remove trailing ']'.
      $element = substr($element, 0, strlen($element)-1);
      // Insert ']' before the first '['.
      $first_part = substr($element, 0, strpos($element, '['));
      $second_part = substr($element, strpos($element, '['));
      $element = $first_part .']'. $second_part;
    }
  }
  form_set_error($element, $message);
}

/**
 * Action: Set the default value.
 */
function rules_forms_action_set_element($form, $element, $value) {
  $form_element = &_rules_forms_get_element($form, $element);
  if (empty($form_element) || !array_key_exists('#default_value', $form_element)) {
    return;
  }
  $lines = explode("\r\n", $value);
  if (count($lines) == 1) {
    $form_element['#default_value'] = $value;
  }
  else {
    $form_element['#default_value'] = $lines;
  }
}

/**
 * Helper function to extract a reference to a form element by a given name.
 */
function &_rules_forms_get_element(&$form, $name) {
  $names = explode('[', $name);
  $element = &$form;
  foreach ($names as $name) {
    if (substr($name, strlen($name)-1) === ']') {
      // Remove trailing ']'.
      $name = substr($name, 0, strlen($name)-1);
    }
    $element =& $element[$name];
  }
  return $element;
}
