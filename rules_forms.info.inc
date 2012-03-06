<?php

/**
 * @file
 * Implements Rules Forms hooks.
 */

function rules_forms_rules_forms_element_info() {
  $info = array();
  $info['button'] = array(
    'label' => t('Button'),
    'value type' => 'text',
    'properties' => rules_forms_element_properties('button'),
  );
  $info['checkbox'] = array(
    'label' => t('Checkbox'),
    'value type' => 'boolean',
    'properties' => rules_forms_element_properties('checkbox'),
  );
  $info['checkboxes'] = array(
    'label' => t('Checkboxes'),
    'value type' => 'struct',
    'properties' => rules_forms_element_properties('checkbox'),
  );
  $info['fieldset'] = array(
    'label' => t('Fieldset'),
    'value type' => FALSE,
    'properties' => rules_forms_element_properties('fieldset'),
  );
  $info['hidden'] = array(
    'label' => t('Hidden element'),
    'value type' => 'unknown',
    'properties' => rules_forms_element_properties('hidden'),
  );
  $info['item'] = array(
    'label' => t('Item'),
    'value type' => FALSE,
    'properties' => rules_forms_element_properties('item'),
  );
  $info['markup'] = array(
    'label' => t('Markup'),
    'value key' => '#markup',
    'value type' => 'text',
    'properties' => rules_forms_element_properties('markup'),
  );
  $info['radio'] = array(
    'label' => t('Radio'),
    'value type' => 'boolean',
    'properties' => rules_forms_element_properties('radio'),
  );
  $info['radios'] = array(
    'label' => t('Radios'),
    'value type' => 'text',
    'properties' => rules_forms_element_properties('radios'),
  );
  $info['select'] = array(
    'label' => t('Select list'),
    'value type' => 'text',
    'properties' => rules_forms_element_properties('select'),
  );
  $info['submit'] = array(
    'label' => t('Submit'),
    'properties' => rules_forms_element_properties('submit'),
  );
  $info['tableselect'] = array(
    'label' => t('Tableselect'),
    'properties' => rules_forms_element_properties('tableselect'),
  );
  $info['text_format'] = array(
    'label' => t('Text format'),
    'properties' => rules_forms_element_properties('text_format'),
  );
  $info['textarea'] = array(
    'label' => t('Textarea'),
    'value type' => 'text',
    'properties' => rules_forms_element_properties('textarea'),
  );
  $info['textfield'] = array(
    'label' => t('Textfield'),
    'value type' => 'text',
    'properties' => rules_forms_element_properties('textfield'),
  );
  $info['weight'] = array(
    'label' => t('Weight'),
    'value type' => 'integer',
    'properties' => rules_forms_element_properties('weight'),
  );
  $info['value'] = array(
    'label' => t('Value'),
    'type' => 'unknown',
    'properties' => rules_forms_element_properties('value'),
  );
  return $info;
}

function rules_forms_element_properties($element_type) {
  $attributes = array();
  foreach (rules_forms_attribute_info() as $attribute => $info) {
    if ($info['elements'] == 'all' || in_array($element_type, $info['elements'])) {
      $info += array(
        'getter callback' => 'rules_forms_get_attribute',
        'read only' => FALSE,
        'computed' => TRUE,
      );
      if (empty($info['read only'])) {
        $info += array('setter callback' => 'rules_forms_set_attribute');
      }
      $attributes[$attribute] = $info;
    }
  }
  return $attributes;
}

function rules_forms_attribute_info() {
  return array(
    '#access' => array(
      'type' => 'boolean',
      'label' => t('Visible'),
      'description' => t('Whether the element is accessible or not; when FALSE, the element is not rendered and the user submitted value is not taken into consideration.'),
      'elements' => 'all',
    ),
    '#collapsed' => array(
      'type' => 'boolean',
      'label' => t('Collapsed'),
      'description' => t('Indicates whether or not the fieldset is collapsed by default.'),
      'elements' => array('fieldset'),
    ),
    '#collapsible' => array(
      'type' => 'boolean',
      'label' => t('Collapsible'),
      'description' => t('Indicates whether or not the fieldset can be collapsed with JavaScript.'),
      'elements' => array('fieldset'),
    ),
    '#cols' => array(
      'type' => 'int',
      'label' => t('Columns'),
      'description' => t('How many columns wide the textarea should be.'),
      'elements' => array('text_format', 'textarea'),
    ),
    '#default_value' => array(
      'type' => 'text',
      'label' => t('Default value'),
      'description' => t('The value of the form element that will be displayed or selected initially if the form has not been submitted yet.'),
      'elements' => array('checkbox', 'checkboxes', 'hidden', 'radio', 'radios', 'select', 'tableselect', 'text_format', 'textarea', 'textfield', 'weight'),
    ),
    '#description' => array(
      'type' => 'text',
      'label' => t('Description'),
      'description' => t('The description of the form element.'),
      'elements' => array('checkbox', 'checkboxes', 'hidden', 'item', 'radio', 'radios', 'select', 'tableselect', 'text_format', 'textarea', 'textfield', 'weight'),
    ),
    '#disabled' => array(
      'type' => 'boolean',
      'label' => t('Disabled'),
      'description' => t('Disables (greys out) a form input element.'),
      'elements' => array('button', 'checkbox', 'checkboxes', 'radio', 'radios', 'select', 'submit', 'text_format', 'textarea', 'textfield', 'weight', 'text_format'),
    ),
    '#field_prefix' => array(
      'type' => 'text',
      'label' => t('Field prefix text or HTML'),
      'description' => t('Text or code that is placed directly in front of the textfield.'),
      'elements' => array('checkbox', 'radio', 'select', 'textarea', 'textfield'),
    ),
    '#field_suffix' => array(
      'type' => 'text',
      'label' => t('Field suffix text or HTML'),
      'description' => t('Text or code that is placed directly after the textfield.'),
      'elements' => array('checkbox', 'radio', 'select', 'textarea', 'textfield'),
    ),
    '#markup' => array(
      'type' => 'text',
      'label' => t('HTML'),
      'description' => t('Used to set HTML that will be output on the form.'),
      'elements' => array('item', 'markup'),
    ),
    '#multiple' => array(
      'type' => 'boolean',
      'label' => t('Allow multiple'),
      'description' => t('Indicates whether the user may select more than one item.'),
      'elements' => array('select'),
      'read only' => TRUE,
    ),
    '#name' => array(
      'type' => 'text',
      'label' => t('Name'),
      'description' => t('The form element name. This can be used as an identifer.'),
      'elements' => array('button', 'submit'),
    ),
    '#options' => array(
      'type' => 'text',
      'label' => t('Options'),
      'description' => t('Selectable options for a form element that allows multiple choices.'),
      'elements' => array('select', 'checkboxes', 'radios', 'tableselect'),
    ),
    '#prefix' => array(
      'type' => 'text',
      'label' => t('Prefix text or HTML'),
      'description' => t('Text or markup to include before the form element.'),
      'elements' => 'all',
    ),
    '#required' => array(
      'type' => 'boolean',
      'label' => t('Required'),
      'description' => t('Indicates whether or not the element is required. This automatically validates for empty fields, and flags inputs as required.'),
      'elements' => array('checkbox', 'checkboxes', 'radio', 'radios', 'select', 'textarea', 'text_format', 'textfield', 'weight'),
    ),
    '#resizable' => array(
      'type' => 'boolean',
      'label' => t('Resizable'),
      'description' => t('Indicates whether users should be allowed to resize the text area.'),
      'elements' => array('text_format', 'textarea'),
    ),
    '#rows' => array(
      'type' => 'int',
      'label' => t('Rows'),
      'description' => t('How many rows high the textarea should be.'),
      'elements' => array('text_format', 'textarea'),
    ),
    '#size' => array(
      'type' => 'int',
      'label' => t('Size'),
      'description' => t('Width of the textfield (in characters) or size of multiselect box (in lines).'),
      'elements' => array('select', 'textfield'),
    ),
    '#suffix' => array(
      'type' => 'text',
      'label' => t('Suffix text or HTML'),
      'description' => t('Text or markup to include after the form element.'),
      'elements' => 'all',
    ),
    '#title' => array(
      'type' => 'text',
      'label' => t('Title'),
      'description' => t('The title of the form element.'),
      'elements' => array('button', 'checkbox', 'checkboxes', 'radio', 'radios', 'select', 'submit', 'text_format', 'textarea', 'textfield', 'weight', 'text_format', 'item'),
    ),
    '#tree' => array(
      'type' => 'boolean',
      'label' => t('Tree'),
      'description' => t('Used to allow collections of form elements. Normally applied to the "parent" element, as the #tree property cascades to sub-elements.'),
      'read only' => TRUE,
      'elements' => 'all',
    ),
    '#type' => array(
      'type' => 'text',
      'label' => t('Element type'),
      'read only' => TRUE,
      'elements' => 'all',
      'required' => TRUE,
    ),
    '#weight' => array(
      'type' => 'integer',
      'label' => t('Weight'),
      'description' => t('Used to sort the list of form elements before being output; lower numbers appear before higher numbers.'),
      'elements' => 'all',
    ),
    '#value' => array(
      'type' => '*',
      'label' => t('Value'),
      'elements' => array('value', 'hidden'),
      'read only' => TRUE,
    ),
  );
}
