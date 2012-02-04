<?php

/**
 * @file
 * Defines data type classes for forms and form elements.
 */

/**
 * Defines a base class for Rules forms input classes.
 */
abstract class RulesFormsData extends RulesDataUI implements RulesDataDirectInputFormInterface {

  /**
   * Provides the default mode.
   */
  public static function getDefaultMode() {
    return 'input';
  }

  /**
   * Provides the default form.
   */
  public static function defaultForm($name, $info, $settings, $title, $type = 'select') {
    $form[$name] = array(
      '#type' => $type,
      '#title' => $title,
      '#default_value' => isset($settings[$name]) ? $settings[$name] : NULL,
      '#required' => empty($info['optional']),
    );
    return $form;
  }

  /**
   * Renders a parameter value.
   */
  public static function render($value) {
    return array(
      'content' => array('#markup' => check_plain($value)),
      '#attributes' => array('class' => array('rules-forms-parameter')),
    );
  }

}

/**
 * Defines the form element data type UI.
 */
class RulesFormsDataElement extends RulesFormsData {

  /**
   * Returns a select list with options including only available elements.
   */
  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $options = array();
    foreach ($element->availableVariables() as $key => $value) {
      // Populate the options list with only form elements.
      if (!in_array($key, array('site', 'form', 'form_state', 'form_id', 'user'))) {
        $options[$key] = $value['label'];
      }
    }
    $form = parent::defaultForm($name, $info, $settings, 'Form element');
    $form[$name]['#options'] = $options;
    return $form;
  }

}
