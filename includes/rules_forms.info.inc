<?php

/**
 * @file
 * Implements Rules Forms hooks.
 */

/**
 * Implements hook_rules_forms_element_info().
 */
function _rules_forms_element_info() {
  return array(
    'button' => t('Button'),
    'checkbox' => t('Checkbox'),
    'checkboxes' => t('Checkboxes'),
    'fieldset' => t('Fieldset'),
    'hidden' => t('Hidden'),
    'item' => t('Item'),
    'markup' => t('Markup'),
    'radio' => t('Radio'),
    'radios' => t('Radios'),
    'select' => t('Select list'),
    'submit' => t('Submit'),
    'tableselect' => t('Table select list'),
    'text_format' => t('Text format'),
    'textarea' => t('Text area'),
    'textfield' => t('Text field'),
    'weight' => t('Weight'),
    'value' => t('Value'),
  );
}

/**
 * Returns an array of attribute property info for a specific form element type.
 *
 * @param string $element_type
 *   The form element type. This is an element type defined in
 *   hook_rules_forms_element_info().
 */
function _rules_forms_element_properties($element_type) {
  // Allow modules to alter rules forms element attribute info.
  $attribute_info = &drupal_static(__FUNCTION__);
  if (!isset($attribute_info)) {
    $attribute_info = _rules_forms_attribute_info();
    drupal_alter('rules_forms_attribute_info', $attribute_info);
  }

  $attributes = array();
  foreach ($attribute_info as $attribute => $info) {
    if ($info['attribute info']['elements'] == 'all' || in_array($element_type, $info['attribute info']['elements'])) {
      $info += array(
        'getter callback' => 'entity_property_verbatim_get',
        'computed' => TRUE,
        'attribute info' => array(),
      );
      $info['attribute info'] += array('read only' => FALSE);
      if (empty($info['attribute info']['read only'])) {
        $info += array('setter callback' => 'entity_property_verbatim_set');
      }
      $attributes[$attribute] = $info;
    }
  }
  return $attributes;
}

/**
 * Returns an array of attribute property info and data indicating
 * which element types each attribute is supported on.
 */
function _rules_forms_attribute_info() {
  return array(
    '#access' => array(
      'type' => 'boolean',
      'label' => t('Visible'),
      'description' => t('Whether the element is accessible or not; when FALSE, the element is not rendered and the user submitted value is not taken into consideration.'),
      'attribute info' => array('elements' => 'all'),
    ),
    '#collapsed' => array(
      'type' => 'boolean',
      'label' => t('Collapsed'),
      'description' => t('Indicates whether or not the fieldset is collapsed by default.'),
      'attribute info' => array('elements' => array('fieldset')),
    ),
    '#collapsible' => array(
      'type' => 'boolean',
      'label' => t('Collapsible'),
      'description' => t('Indicates whether or not the fieldset can be collapsed with JavaScript.'),
      'attribute info' => array('elements' => array('fieldset')),
    ),
    '#cols' => array(
      'type' => 'int',
      'label' => t('Columns'),
      'description' => t('How many columns wide the textarea should be.'),
      'attribute info' => array('elements' => array('text_format', 'textarea')),
    ),
    '#default_value' => array(
      'type' => 'text',
      'label' => t('Default value'),
      'description' => t('The value of the form element that will be displayed or selected initially if the form has not been submitted yet.'),
      'attribute info' => array(
        'elements' => array('checkbox', 'checkboxes', 'hidden', 'radio', 'radios', 'select', 'tableselect', 'text_format', 'textarea', 'textfield', 'weight'),
      ),
    ),
    '#description' => array(
      'type' => 'text',
      'label' => t('Description'),
      'description' => t('The description of the form element.'),
      'attribute info' => array(
        'elements' => array('checkbox', 'checkboxes', 'hidden', 'item', 'radio', 'radios', 'select', 'tableselect', 'text_format', 'textarea', 'textfield', 'weight'),
      ),
    ),
    '#disabled' => array(
      'type' => 'boolean',
      'label' => t('Disabled'),
      'description' => t('Disables (greys out) a form input element.'),
      'attribute info' => array(
        'elements' => array('button', 'checkbox', 'checkboxes', 'radio', 'radios', 'select', 'submit', 'text_format', 'textarea', 'textfield', 'weight', 'text_format'),
      ),
    ),
    '#field_prefix' => array(
      'type' => 'text',
      'label' => t('Field prefix text or HTML'),
      'sanitize' => 'check_markup',
      'description' => t('Text or code that is placed directly in front of the textfield.'),
      'attribute info' => array('elements' => array('checkbox', 'radio', 'select', 'textarea', 'textfield')),
    ),
    '#field_suffix' => array(
      'type' => 'text',
      'label' => t('Field suffix text or HTML'),
      'sanitize' => 'check_markup',
      'description' => t('Text or code that is placed directly after the textfield.'),
      'attribute info' => array('elements' => array('checkbox', 'radio', 'select', 'textarea', 'textfield')),
    ),
    '#markup' => array(
      'type' => 'text',
      'label' => t('HTML'),
      'sanitize' => 'check_markup',
      'description' => t('Used to set HTML that will be output on the form.'),
      'attribute info' => array('elements' => array('item', 'markup')),
    ),
    '#multiple' => array(
      'type' => 'boolean',
      'label' => t('Allow multiple'),
      'description' => t('Indicates whether the user may select more than one item.'),
      'attribute info' => array(
        'elements' => array('select'),
        'read only' => TRUE,
      ),
    ),
    '#name' => array(
      'type' => 'text',
      'label' => t('Name'),
      'description' => t('The form element name. This can be used as an identifer.'),
      'attribute info' => array('elements' => array('button', 'submit')),
    ),
    '#options' => array(
      'type' => 'text',
      'label' => t('Options'),
      'description' => t('Selectable options for a form element that allows multiple choices.'),
      'attribute info' => array(
        'elements' => array('select', 'checkboxes', 'radios', 'tableselect'),
        'validate callback' => 'rules_forms_options_set_validate',
        'setter callback' => 'rules_forms_options_set',
      ),
    ),
    '#prefix' => array(
      'type' => 'text',
      'label' => t('Prefix text or HTML'),
      'description' => t('Text or markup to include before the form element.'),
      'attribute info' => array('elements' => 'all'),
    ),
    '#required' => array(
      'type' => 'boolean',
      'label' => t('Required'),
      'description' => t('Indicates whether or not the element is required. This automatically validates for empty fields, and flags inputs as required.'),
      'attribute info' => array(
        'elements' => array('checkbox', 'checkboxes', 'radio', 'radios', 'select', 'textarea', 'text_format', 'textfield', 'weight'),
      ),
    ),
    '#resizable' => array(
      'type' => 'boolean',
      'label' => t('Resizable'),
      'description' => t('Indicates whether users should be allowed to resize the text area.'),
      'attribute info' => array('elements' => array('text_format', 'textarea')),
    ),
    '#rows' => array(
      'type' => 'int',
      'label' => t('Rows'),
      'description' => t('How many rows high the textarea should be.'),
      'attribute info' => array('elements' => array('text_format', 'textarea')),
    ),
    '#size' => array(
      'type' => 'int',
      'label' => t('Size'),
      'description' => t('Width of the textfield (in characters) or size of multiselect box (in lines).'),
      'attribute info' => array('elements' => array('select', 'textfield')),
    ),
    '#suffix' => array(
      'type' => 'text',
      'label' => t('Suffix text or HTML'),
      'description' => t('Text or markup to include after the form element.'),
      'attribute info' => array('elements' => 'all'),
    ),
    '#title' => array(
      'type' => 'text',
      'label' => t('Title'),
      'description' => t('The title of the form element.'),
      'attribute info' => array(
        'elements' => array('button', 'checkbox', 'checkboxes', 'radio', 'radios', 'select', 'submit', 'text_format', 'textarea', 'textfield', 'weight', 'text_format', 'item'),
      ),
    ),
    '#tree' => array(
      'type' => 'boolean',
      'label' => t('Tree'),
      'description' => t('Used to allow collections of form elements. Normally applied to the "parent" element, as the #tree property cascades to sub-elements.'),
      'attribute info' => array('elements' => 'all', 'read only' => TRUE),
    ),
    '#type' => array(
      'type' => 'text',
      'label' => t('Element type'),
      'attribute info' => array('elements' => 'all', 'read only' => TRUE),
      'required' => TRUE,
    ),
    '#weight' => array(
      'type' => 'integer',
      'label' => t('Weight'),
      'description' => t('Used to sort the list of form elements before being output; lower numbers appear before higher numbers.'),
      'attribute info' => array('elements' => 'all'),
    ),
    '#value' => array(
      'type' => '*',
      'label' => t('Value'),
      'attribute info' => array(
        'read only' => TRUE,
        'elements' => array('value', 'hidden'),
      ),
    ),
  );
}
