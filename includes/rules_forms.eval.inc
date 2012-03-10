<?php

/**
 * @file
 * Evaluation functions for Rules Forms module.
 */

/**
 * Info alter callback for the element has value condition.
 * Sets the value field according to the type of value the element uses.
 */
function rules_forms_element_value_info_alter(&$element_info, $element) {
  $element->settings += array('data:select' => NULL);
  if ($wrapper = $element->applyDataSelector($element->settings['data:select'])) {
    if ($wrapper instanceof RulesFormsElementWrapper) {
    $info = $wrapper->info();
    $element_info['parameter']['value']['type'] = $wrapper->valueType();
    $element_info['parameter']['value']['options list']  = !empty($info['options list']) ? 'rules_data_selector_options_list' : FALSE;
  }}
}

/**
 * Condition: Form element has value.
 */
function rules_forms_condition_element_value(EntityMetadataWrapper $wrapper, $value, $regex, $settings, $state, $element) {
  if ($wrapper instanceof RulesFormsElementWrapper) {
    $attribute = isset($wrapper->value) ? $wrapper->value->value() : $wrapper->default_value->value();
    // Perform the comparison with a regular expression if necessary.
    if ($regex) {
      // Allow multiple regular expressions to be run, one on each line.
      // Return FALSE immediately if an expression fails.
      $lines = explode("\r\n", $value);
      foreach ($lines as $line) {
        $result = preg_match($value, $attribute_value) == 1;
        if ($result === FALSE) {
          return FALSE;
        }
      }
      return TRUE;
    }

    // Multiple values come in as array.
    if (is_array($attribute_value)) {
      $lines = explode("\r\n", $value);
      return rules_forms_equal_array_values($lines, $attribute_value);
    }
    return $attribute_value === $value;
  }
  else {
    return $wrapper === $value;
  }
}

/**
 * Condition: Form element attribute has value.
 */
function rules_forms_condition_attribute_value(EntityMetadataWrapper $wrapper, $value, $regex, $settings, $state, $element) {
  if ($wrapper instanceof RulesFormsAttributeWrapper) {
    $attribute_value = $wrapper->value();
    // Perform the comparison with a regular expression if necessary.
    if ($regex) {
      // Allow multiple regular expressions to be run, one on each line.
      // Return FALSE immediately if an expression fails.
      $lines = explode("\r\n", $value);
      foreach ($lines as $line) {
        $result = preg_match($value, $attribute_value) == 1;
        if ($result === FALSE) {
          return FALSE;
        }
      }
      return TRUE;
    }
  
    // Multiple values come in as array.
    if (is_array($attribute_value)) {
      $lines = explode("\r\n", $value);
      return rules_forms_equal_array_values($lines, $attribute_value);
    }
    return $attribute_value === $value;
  }
  else {
    return $wrapper === $value;
  }
}

/**
 * Condition: Form element value has changed.
 *
 * @param ArrayObject $form
 *   A reference to the form array of the form for which the event
 *   was triggered.
 * @param RulesFormsFormStateWrapper $form_state
 *   A reference to the form state array of the form for which the
 *   event was triggered.
 * @param string $element
 *   The element ID of the element being checked in the form of
 *   element_type:element_id.
 *
 * @return bool
 *   Indicates whether the element's value has changed since the
 *   form was built.
 */
function rules_forms_condition_element_changed(EntityStructureWrapper $form_element, $form_state, $settings, $state, $element) {
  $form_id = $form['form_id']['#value'];
  $form_values = $form_state->value();
  $form_values = $form_values['values'];

  if (isset($_SESSION['rules_forms_form_values'][$form_id]) && strpos($settings['data:select'], ':') !== FALSE) {
    // Get the build form values and the element name being validated.
    $element_name = substr($settings['data:select'], strpos($settings['data:select'], ':') + 1);
    $build_values = $_SESSION['rules_forms_form_values'][$form_id];

    // Ensure that form values are set. This will prevent the condition from being evaluated during build.
    if (isset($build_values[$element_name]) && isset($form_state['values']) && isset($form_state['values'][$element_name])) {
      return $build_values[$element_name] !== $form_state['values'][$element_name];
    }
  }
  return FALSE;
}

/**
 * Condition: Form button was clicked.
 *
 * @param RulesFormsFormStateWrapper $form_state
 *   A reference to the form state array of the form for which the
 *   event was triggered.
 * @param EntityStructureWrapper $form_element
 *   The element ID of the element being checked in the form of
 *   element_type:element_id.
 *
 * @return bool
 *   Indicates whether the indicated button was clicked.
 */
function rules_forms_condition_button_clicked($form_state, EntityStructureWrapper $form_element, $settings, $state, $element) {
  $form_state = $form_state->value();
  $form_element = $form_element->value();
  if (isset($form_state['triggering_element'])) {
    $button = $form_state['triggering_element'];
    if (isset($form_element['#name']) && isset($button['#name'])) {
      return $form_element['#name'] === $button['#name'];
    }
    elseif (isset($form_element['#value']) && isset($button['#value'])) {
      return $form_element['#value'] === $button['#value'];
    }
  }
  return FALSE;
}

/**
 * Returns TRUE if both arrays contain the same values, regardless
 * of their keys and value ordering.
 *
 * @param array $array1
 *   An array to compare against $array2.
 * @param array $array2
 *   An array to compare against $array1.
 *
 * @return bool
 *   Indicates whether the input arrays are equal.
 */
function rules_forms_equal_array_values($array1, $array2) {
  $diff1 = array_diff($array1, $array2);
  $diff2 = array_diff($array2, $array1);
  return empty($diff1) && empty($diff2);
}

/**
 * Action: Set the value of an element attribute.
 */
function rules_forms_action_attribute_set(EntityMetadataWrapper $wrapper, $value, $settings, $state, $element) {
  if ($wrapper instanceof RulesFormsAttributeWrapper) {
    try {
      $wrapper->set($value);
    }
    catch (EntityMetadataWrapperException $e) {
      throw new RulesEvaluationException('Unable to modify data "@selector": ' . $e->getMessage(), array('@selector' => $settings['data:select']));
    }
  }
  else {
    return array('data' => $value);
  }
}

/**
 * Info alter callback for the attribute_set action.
 * Load the Rules callback.
 */
function rules_forms_action_attribute_set_info_alter(&$element_info, $element) {
  module_load_include('inc', 'rules', 'modules/data.eval');
  rules_action_data_set_info_alter($element_info, $element);
}

/**
 * Action: Set the redirect target.
 *
 * @param RulesFormsFormStateWrapper $form_state
 *   A reference to the form state array of the form for which the
 *   event was triggered.
 * @param string $path
 *   The path to which to redirect the user.
 * @param string $query
 *   A query string for use in drupal_goto().
 * @param string $fragment
 *   A fragement string for use in drupal_goto().
 */
function rules_forms_action_redirect($form_state, $path, $query, $fragment) {
  $form_state = $form_state->value();
  // We manually check the form to see if redirect is okay.
  // Setting $form_state['redirect'] has proven to be unreliable.
  if (!empty($form_state['programmed']) || !empty($form_state['rebuild']) || !empty($form_state['no_redirect'])) {
    return;
  }

  // Check if a query was defined and build the options array.
  $options = $query != '' ? array('query' => array($query)) : array();
  $options['fragment'] = $fragment;

  // Redirect the user.
  drupal_goto($path, $options);
}

/**
 * Action: Set form error.
 *
 * @param EntityStructureWrapper
 *   The wrapped form element on which to set the error, if any.
 * @param string $message
 *   The error message to set on the form element.
 */
function rules_forms_action_set_error(EntityMetadataWrapper $wrapper, $message, $settings, $state, $element) {
  // Use the data selector structure to target the form element.
  if ($wrapper instanceof RulesFormsElementWrapper) {
    $element = $wrapper->getElementName();
  }
  else {
    $element = '';
  }
  form_set_error($element, $message);
}

/**
 * Validation callback for set options action.
 */
function rules_forms_action_set_options_validate($element) {
  // Check for duplicate key values to prevent unexpected data loss. Require
  // all options to include a safe_key.
  $lines = explode("\n", trim($element->settings['options']));
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

/**
 * Action: Set multiple value options of a form element.
 * Note: For multiple option values each key value pair is on its own line
 * and formatted key|value.
 *
 * @param ArrayObject $form
 *   A reference to the form array of the form for which the event
 *   was triggered.
 * @param string $element_id
 *   The element ID of the element being checked in the form of
 *   element_type:element_id.
 * @param string $value
 *   A value to assign to the element's #options attribute. This value
 *   is split into an array.
 */
function rules_forms_action_set_options($form, $element_id, $value) {
  $form_element = &_rules_forms_get_element($form, $element_id);
  $lines = explode("\r\n", trim($value));

  $processed_options = array();
  foreach ($lines as $line) {
    $line = trim($line);
    if (preg_match('/^([^|]+)\|(.*)$/', $line, $matches)) {
      $processed_options[$matches[1]] = $matches[2];
    }
  }
  if (isset($form_element['#type'])) {
    $form_element['#options'] = $processed_options;
  }
}

