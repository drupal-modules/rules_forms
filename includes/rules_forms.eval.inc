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

  // First check if a value exists.
  if (isset($form_element['#value'])) {
    // Multiple values come in as array.
    if (is_array($form_element['#value'])) {
      $lines = explode("\r\n", $value);
      return rules_forms_equal_array_values($lines, $form_element['#value']);
    }
    return $form_element['#value'] === $value;
  }

  // If no #value existed, check against the default value.
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
 * Action: Adjust weight of a form element.
 */
function rules_forms_action_adjust_weight($form, $element, $weight) {
  $form_element = &_rules_forms_get_element($form, $element);
  $form_element['#weight'] = $weight;
}

/**
 * Action: Set form error.
 */
function rules_forms_action_set_error($form, $element, $message) {
  if (substr($element, strlen($element) - 1) === ']') {
    $form_element = &_rules_forms_get_element($form, $element);
    if (isset($form_element['#parents'])) {
      $element = implode('][', $form_element['#parents']);
    }
    else {
      // Remove trailing ']'.
      $element = substr($element, 0, strlen($element) - 1);
      // Insert ']' before the first '['.
      $first_part = substr($element, 0, strpos($element, '['));
      $second_part = substr($element, strpos($element, '['));
      $element = $first_part . ']' . $second_part;
    }
  }
  form_set_error($element, $message);
}

/**
 * Validation callback for set options action.
 */
function rules_forms_action_set_attribute_validate($element) {
  if (isset($element->settings['attribute']) && $element->settings['attribute'] == '#options') {
    // Check for duplicate key values to prevent unexpected data loss. Require
    // all options to include a safe_key.
    $lines = explode("\n", trim($element->settings['access']));
    $existing_keys = array();
    $duplicate_keys = array();
    $missing_keys = array();
    $long_keys = array();
    $group = '';

    // Check each option for validity - length, existence, and duplication.
    foreach ($lines as $line) {
      $matches = array();
      $line = trim($line);
      if (preg_match('/^([^|]*)\|(.*)$/', $line, $matches)) {
        $key = $matches[1];

        // Validate the length of the key.
        if (strlen($key) > 128) {
          $long_keys[] = $key;
        }
      }
      // The key was not found.
      else {
        $missing_keys[] = $line;
      }

      // Ensure this is not a duplicate key.
      if (isset($key)) {
        if (isset($existing_keys[$group][$key])) {
          $duplicate_keys[$key] = $key;
        }
        else {
          $existing_keys[$group][$key] = $key;
        }
      }
    }
  
    // Throw exceptions for validation errors.
    if (!empty($missing_keys)) {
      throw new RulesIntegrityException(t('Every option must have a key specified. Specify each option as "safe_key|Some readable option'), $element);
    }
  
    if (!empty($long_keys)) {
      throw new RulesIntegrityException(t('Option keys must be less than 128 characters. The following keys exceed this limit:') . theme('item_list', $long_keys), $element);
    }
  
    if (!empty($duplicate_keys)) {
      throw new RulesIntegrityException(t('Options within the select list must be unique. The following keys have been used multiple times:') . theme('item_list', array('items' => $duplicate_keys)), $element);
    }
  }
}

/**
 * Action: Set form element attribute.
 * Note: For multiple option values each key value pair is on its own line
 * and formatted key|value.
 */
function rules_forms_action_set_attribute($form, $element_id, $attribute, $value) {
  $form_element = &_rules_forms_get_element($form, $element_id);
  $lines = explode("\r\n", trim($value));

  if ($attribute == '#options') {
    $processed_options = array();
    foreach ($lines as $line) {
      $line = trim($line);
      if (preg_match('/^([^|]+)\|(.*)$/', $line, $matches)) {
        $processed_options[$matches[1]] = $matches[2];
      }
    }
    $form_element['#options'] = $processed_options;
  }
  else {
    // This value should only be one line.
    $value = array_shift($lines);

    // For boolean values, 0 or 1 may have been indicated. Convert to boolean.
    switch ($value) {
      case '0':
        $value = FALSE;
        break;
      case '1':
        $value = TRUE;
        break;
    }

    $form_element[$attribute] = $value;
  }
}

/**
 * Helper function to extract a reference to a form element by a given name.
 */
function &_rules_forms_get_element(&$form, $name) {
  // Get the element key after the first colon.
  $name = substr($name, strpos($name, ':') + 1);
  $names = explode('[', $name);
  $element = &$form;
  foreach ($names as $name) {
    if (substr($name, strlen($name) - 1) === ']') {
      // Remove trailing ']'.
      $name = substr($name, 0, strlen($name) - 1);
    }
    $element =& $element[$name];
  }
  return $element;
}
