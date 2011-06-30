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
 * Action: Set form attributes.
 */
function rules_forms_action_set_attributes($form, $element, $access, $description, $disabled, $required, $weight) {
  $settings = array(
    '#access' => $access,
    '#description' => $description,
    '#disabled' => $disabled,
    '#required' => $required,
    '#weight' => $weight,
  );

  $form_element = &_rules_forms_get_element($form, $element);
  foreach ($settings as $key => $value) {
    $form_element[$key] = $settings[$key];
  }
}

/**
 * Action: Add a form element.
 *
 * @param $form
 *   The form on which the element will be placed. This argument is passed
 *   as an ArrayObject, so we reference it.
 * @param $type
 *   The type of form element being created.
 * @param $name
 *   The machine-readable name of a form element.
 * @param $title
 *   The '#title' attribute of the created form element.
 * @param $description
 *   The '#description' attribute of the created form element.
 * @param $required
 *   A boolean value representing whether the element should be required.
 * @paam $weight
 *   An integer to set as the weight of the form element.
 * @param $default
 *   A default value to set for the form element. For boolean value
 *   form elements, this should be a 1 or 0. For others it may be
 *   a string.
 * @param $options
 *   A string that will be converted into options, with one array item
 *   per line.
 * @param $multiple
 *   A boolean value representing whether the element allows multiple values.
 *   This generally applies only to 'select' type elements.
 */
function rules_forms_action_add_element($form, $type, $parent, $name, $title, $description, $required, $weight, $default, $options, $multiple) {
  if (!empty($parent)) {
    $parent_element = &_rules_forms_get_element($form, $parent);
    $parent_element[$name] = rules_forms_element_attributes($type, $title, $description, $required, $weight, $default, $options, $multiple);
  }
  else {
    $form[$name] = rules_forms_element_attributes($type, $title, $description, $required, $weight, $default, $options, $multiple);
  }
}

/**
 * Sets a form element's attributes.
 *
 * The attributes set in the action's settings are compared against a
 * definition of allowed attributes for the form element type. If the
 * attribute is allowed and has been set to something other than the
 * default then it will be set in the form element. For elements that
 * require an array of options, the options array is created from the
 * action input.
 *
 * @see rules_forms_action_add_element()
 * @see _rules_forms_element_attributes()
 */
function rules_forms_element_attributes($type, $title, $description, $required, $weight, $default_value, $options, $multiple) {
  $settings = _rules_forms_element_attributes();
  $attributes = array();

  // Create options and default value arrays for multiple option elements.
  if ($settings[$type]['options'] == RULES_FORMS_REQUIRED) {
    $options = explode("\r\n", $options);
    if ($settings[$type]['multiple'] == RULES_FORMS_OPTIONAL && $multiple == TRUE) {
      $default_value = explode("\r\n", $default_value);
    }
  }

  // Loop through each attribute and set if necessary.
  foreach ($settings[$type] as $key => $required) {
    // Check if the attribute is allowed.
    if ($required == RULES_FORMS_DISABLED) {
      continue;
    }
    // If it is required then force it to be set.
    elseif ($required == RULES_FORMS_REQUIRED) {
      $attributes['#'. $key] = ${$key};
    }
    // If the attribute wasn't set in settings then don't set it here.
    elseif (empty(${$key})) {
      continue;
    }
    // The attribute was set in action settings.
    else {
      $attributes['#'. $key] = ${$key};
    }
  }

  return $attributes;
}

/**
 * Defines attributes for specific form elements.
 *
 * @see rules_forms_element_attributes()
 */
function _rules_forms_element_attributes() {
  $defaults = array(
    'type' => RULES_FORMS_REQUIRED,
    'title' => RULES_FORMS_REQUIRED,
    'default_value' => RULES_FORMS_OPTIONAL,
    'description' => RULES_FORMS_OPTIONAL,
    'required' => RULES_FORMS_OPTIONAL,
    'weight' => RULES_FORMS_OPTIONAL,
  );

  return array(
    'textfield' => $defaults + array(
      'options' => RULES_FORMS_DISABLED,
      'multiple' => RULES_FORMS_DISABLED,
    ),
    'textarea' => $defaults + array(
      'options' => RULES_FORMS_DISABLED,
      'multiple' => RULES_FORMS_DISABLED,
    ),
    'checkbox' => $defaults + array(
      'options' => RULES_FORMS_DISABLED,
      'multiple' => RULES_FORMS_DISABLED,
    ),
    'checkboxes' => $defaults + array(
      'options' => RULES_FORMS_REQUIRED,
      'multiple' => RULES_FORMS_DISABLED,
    ),
    'select' => $defaults + array(
      'options' => RULES_FORMS_REQUIRED,
      'multiple' => RULES_FORMS_OPTIONAL,
    ),
    'radios' => $defaults + array(
      'options' => RULES_FORMS_REQUIRED,
      'multiple' => RULES_FORMS_DISABLED,
    ),
  );
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
