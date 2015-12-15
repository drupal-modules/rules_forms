<?php

/**
 * @file
 * Hooks provided by the rules_forms module.
 *
 * @ingroup xmlsitemap
 */

/**
 * @addtogroup hooks
 * @{
 */


/**
 * Implements hook_TYPE_alter().
 *
 * Alter the current form_info data of rules_forms.
 */
function hook_rules_forms_form_info_alter(&$form_info, $context) {

  $module = 'foo_module';
  foreach ($context['fields'] as $key => $field) {
    if ($field['widget']['module'] == $module) {
      $form_info['elements'][$module . ':' . $module] = array(
        'type' => $module,
        'label' => $field['label'],
      );
    }
  }

}

/**
 * @} End of "addtogroup hooks".
 */
