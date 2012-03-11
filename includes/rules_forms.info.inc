<?php

/**
 * @file
 * Implements Rules Forms hooks.
 */

/**
 * Implements hook_rules_forms_element_info().
 *
 * Possible keys:
 * - label: A human-readable label of the form element.
 * - description (optional): A human-readable translated description.
 * - properties: An array of property info as required by EntityMetadataWrapper.
 *   This info should directly correspond to the element's possible attributes.
 *   Additional keys in each individual property:
 *   - attribute info: An array of information about the form element attribute.
 *     - validate callback (optional): A callback for validating form data in the
 *       set element attribute action.
 *     - setter callback: A callback for setting data in the set element attribute action.
 * - element info: An array of information describing the form element to Rules Forms.
 *   - data type: The type of data the form element's value requires.
 *   - keys: An array of key|value pairs describing keys to important attributes.
 *     For example:
 *     - value: The key to the value of the form element (if any).
 *     - default_value: The key to the default value of the form element (if any).
 *   - empty: An empty form element value.
 */
function rules_forms_rules_forms_element_info() {
  $info['button'] = array(
    'label' => t('Button'),
    'properties' => rules_forms_element_properties('button'),
    'element info' => array(
      'data type' => 'text',
      'keys' => array('value' => '#value'),
      'empty' => '',
    ),
  );
  $info['checkbox'] = array(
    'label' => t('Checkbox'),
    'properties' => rules_forms_element_properties('checkbox'),
    'element info' => array(
      'data type' => 'boolean',
      'keys' => array('value' => '#value', 'default_value' => '#default_value'),
      'empty' => FALSE,
    ),
  );
  $info['checkboxes'] = array(
    'label' => t('Checkboxes'),
    'properties' => rules_forms_element_properties('checkboxes'),
    'element info' => array(
      'data type' => 'struct',
      'keys' => array('value' => '#value', 'default_value' => '#default_value'),
      'empty' => array(),
    ),
  );
  $info['container'] = array(
    'label' => t('Container'),
    'properties' => rules_forms_element_properties('container'),
    // The 'data type' FALSE indicates that this element is not settable.
    'element info' => array('data type' => FALSE),
  );
  $info['fieldset'] = array(
    'label' => t('Fieldset'),
    'properties' => rules_forms_element_properties('fieldset'),
    // The 'data type' FALSE indicates that this element is not settable.
    'element info' => array('data type' => FALSE),
  );
  $info['hidden'] = array(
    'label' => t('Hidden'),
    'properties' => rules_forms_element_properties('hidden'),
    'element info' => array(
      'data type' => '*',
      'keys' => array('value' => '#value'),
      'empty' => NULL,
    ),
  );
  $info['item'] = array(
    'label' => t('Item'),
    'properties' => rules_forms_element_properties('item'),
    'element info' => array(
      'data type' => 'text',
      'keys' => array('value' => '#markup'),
      'empty' => '',
    ),
  );
  $info['markup'] = array(
    'label' => t('Markup'),
    'properties' => rules_forms_element_properties('markup'),
    'element info' => array(
      'data type' => 'text',
      'keys' => array('value' => '#markup'),
      'empty' => '',
    ),
  );
  $info['radio'] = array(
    'label' => t('Radio'),
    'properties' => rules_forms_element_properties('radio'),
    'element info' => array(
      'data type' => 'text',
      'keys' => array('value' => '#value', 'default_value' => '#default_value'),
      'empty' => NULL,
    ),
  );
  $info['radios'] = array(
    'label' => t('Radios'),
    'properties' => rules_forms_element_properties('radios'),
    'element info' => array(
      'data type' => 'text',
      'keys' => array('value' => '#value', 'default_value' => '#default_value'),
      'empty' => '',
    ),
  );
  $info['select'] = array(
    'label' => t('Select list'),
    'properties' => rules_forms_element_properties('select'),
    'element info' => array(
      'data type' => 'text|struct',
      'keys' => array('value' => '#value', 'default_value' => '#default_value'),
      'empty' => '',
    ),
  );
  $info['submit'] = array(
    'label' => t('Submit'),
    'properties' => rules_forms_element_properties('submit'),
    'element info' => array(
      'data type' => 'text',
      'keys' => array('value' => '#value'),
      'empty' => '',
    ),
  );
  $info['tableselect'] = array(
    'label' => t('Table select list'),
    'properties' => rules_forms_element_properties('tableselect'),
    'element info' => array(
      'data type' => 'struct',
      'keys' => array('value' => '#value', 'default_value' => '#default_value'),
      'empty' => array(),
    ),
  );
  $info['text_format'] = array(
    'label' => t('Text format'),
    'properties' => rules_forms_element_properties('text_format'),
    'element info' => array(
      'data type' => 'text',
      'keys' => array('value' => '#value', 'default_value' => '#default_value'),
      'empty' => '',
    ),
  );
  $info['textarea'] = array(
    'label' => t('Text area'),
    'properties' => rules_forms_element_properties('textarea'),
    'element info' => array(
      'data type' => 'text',
      'keys' => array('value' => '#value', 'default_value' => '#default_value'),
      'empty' => '',
    ),
  );
  $info['textfield'] = array(
    'label' => t('Text field'),
    'properties' => rules_forms_element_properties('textfield'),
    'element info' => array(
      'data type' => 'text',
      'keys' => array('value' => '#value', 'default_value' => '#default_value'),
      'empty' => '',
    ),
  );
  $info['weight'] = array(
    'label' => t('Weight'),
    'properties' => rules_forms_element_properties('weight'),
    'element info' => array(
      'data type' => 'integer',
      'keys' => array('value' => '#value', 'default_value' => '#default_value'),
      'empty' => 0,
    ),
  );
  $info['value'] = array(
    'label' => t('Value'),
    'properties' => rules_forms_element_properties('value'),
    'element info' => array(
      'data type' => '*',
      'keys' => array('value' => '#value'),
      'empty' => '',
    ),
  );
  return $info;
}

/**
 * Returns an array of attribute property info for a specific form element type.
 *
 * @param string $element_type
 *   The form element type. This is an element type defined in
 *   hook_rules_forms_element_info().
 */
function rules_forms_element_properties($element_type) {
  $attribute_info = _rules_forms_attribute_info();
  $attributes = array();
  foreach ($attribute_info as $attribute => $info) {
    if ($info['attribute info']['elements'] == 'all' || in_array($element_type, $info['attribute info']['elements'])) {
      $attributes[$attribute] = _rules_forms_build_element_property($info);
    }
  }
  return $attributes;
}

/**
 * Returns an array of element properties based on existing attributes.
 */
function rules_forms_filter_element_properties($element) {
  $attribute_info = _rules_forms_attribute_info();
  $attributes = array();
  foreach (element_properties($element) as $attribute => $info) {
    if (isset($attribute_info[$attribute])) {
      $attributes[$attribute] = _rules_forms_build_element_property($info);
    }
  }
  return $attributes;
}

/**
 * Builds the property info of an individual property from an array of info.
 */
function _rules_forms_build_element_property(array $info) {
  $info += array(
    'getter callback' => 'entity_property_verbatim_get',
    'computed' => TRUE,
    'attribute info' => array(),
  );
  $info['attribute info'] += array('read only' => FALSE);
  if (empty($info['attribute info']['read only'])) {
    $info += array('setter callback' => 'entity_property_verbatim_set');
  }
  // Unset the 'elements' and 'read only' portions as those are not used in wrappers.
  unset($info['attribute info']['elements'], $info['attribute info']['read only']);
  return $info;
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