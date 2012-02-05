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
    // Multiple values come in as array.
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
  $form_element = &_rules_forms_get_element($form, $element);
  if (!empty($prefix)) {
    $form_element['#prefix'] = $prefix;
  }
  if (!empty($suffix)) {
    $form_element['#suffix'] = $suffix;
  }
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
 * Action: Set the default value.
 */
function rules_forms_action_set_element($form, $element, $value) {
  $form_element = &_rules_forms_get_element($form, $element);
  if (empty($form_element)) {
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
 * Action: Set element title.
 */
function rules_forms_action_set_title($form, $element, $title) {
  $form_element = &_rules_forms_get_element($form, $element);
  $form_element['#title'] = $title;
}

/**
 * Action: Set element description.
 */
function rules_forms_action_set_description($form, $element, $description) {
  $form_element = &_rules_forms_get_element($form, $element);
  $form_element['#description'] = $description;
}

/**
 * Action: Hide a form element.
 */
function rules_forms_action_set_access($form, $element, $access) {
  $form_element = &_rules_forms_get_element($form, $element);
  $form_element['#access'] = $access == 0;
}

/**
 * Action: Disable a form element.
 */
function rules_forms_action_set_disabled($form, $element, $disabled) {
  $form_element = &_rules_forms_get_element($form, $element);
  $form_element['#disabled'] = $disabled == 1;
}

/**
 * Action: Require a form element.
 */
function rules_forms_action_set_required($form, $element, $required) {
  $form_element = &_rules_forms_get_element($form, $element);
  $form_element['#required'] = $required == 1;
}

/**
 * Validation callback for set options action.
 */
function rules_forms_action_set_options_validate($element) {
  // Check for duplicate key values to prevent unexpected data loss. Require
  // all options to include a safe_key.
  $lines = explode("\n", trim($element->settings['access']));
  $existing_keys = array();
  $duplicate_keys = array();
  $missing_keys = array();
  $long_keys = array();
  $group = '';
  
  foreach ($lines as $line) {
    $matches = array();
    $line = trim($line);
    if (preg_match('/^([^|]*)\|(.*)$/', $line, $matches)) {
      $key = $matches[1];
      if (strlen($key) > 128) {
        $long_keys[] = $key;
      }
    }
    else {
      $missing_keys[] = $line;
    }

    if (isset($key)) {
      if (isset($existing_keys[$group][$key])) {
        $duplicate_keys[$key] = $key;
      }
      else {
        $existing_keys[$group][$key] = $key;
      }
    }
  }

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

/**
 * Action: Set multiple value element options.
 */
function rules_forms_action_set_options($form, $element, $options) {
  $form_element = &_rules_forms_get_element($form, $element);
  $processed_options = array();
  $lines = explode("\r\n", trim($options));
  foreach ($lines as $line) {
    $line = trim($line);
    if (preg_match('/^([^|]+)\|(.*)$/', $line, $matches)) {
      $processed_options[$matches[1]] = $matches[2];
    }
  }
  $form_element['#options'] = $processed_options;
}

/**
 * Helper function to extract a reference to a form element by a given name.
 */
function &_rules_forms_get_element(&$form, $name) {
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
