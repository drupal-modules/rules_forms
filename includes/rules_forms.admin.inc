<?php

/**
 * @file
 * Administrative forms for Rules Forms module.
 */

/**
 * Defines the forms events settings form.
 */
function rules_forms_admin_events($form, &$form_state) {
  $form = array();
  $form['general_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('General settings'),
    '#collapsible' => TRUE,
  );
  $form['general_settings']['enable_form_activation_message'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable event activation messages on forms'),
    '#default_value' => isset($_SESSION['rules_forms_message']) ? $_SESSION['rules_forms_message'] : FALSE,
    '#description' => t('If checked, there will be a message on each form containing a link to activate events '.
                        'for the form. Only visible for your currently logged in user account.'),
  );
  $form['general_settings']['settings_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
    '#submit' => array('rules_forms_settings_submit'),
  );

  $form_info = rules_forms_get_form_info();
  if (!empty($form_info)) {
    $options = array();
    foreach ($form_info as $form_id => $info) {
      $options[$form_id] = $info['label'];
    }
    $form['active_forms'] = array(
      '#type' => 'fieldset',
      '#title' => t('Active forms'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['active_forms']['form_events'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Forms where events are activated'),
      '#options' => $options,
      '#description' => t('Forms that currently invoke events. Select forms to deactivate related events.'),
    );
    $form['active_forms']['events_deactivate'] = array(
      '#type' => 'submit',
      '#value' => t('Deactivate events'),
      '#submit' => array('rules_forms_events_deactivate_submit'),
    );
  }
  else {
    drupal_set_message(t('To begin create form rules, enable the event activation messages below and navigate '.
                         'to the form you would like to activate events on.'), 'status', FALSE);
  }
  return $form;
}

/**
 * Submit handler to save settings.
 */
function rules_forms_settings_submit($form_id, $form_state) {
  $_SESSION['rules_forms_message'] = (bool) $form_state['values']['enable_form_activation_message'];
  drupal_set_message(t('The settings have been saved.'));
}

/**
 * Submit handler to deactivate form events.
 */
function rules_forms_events_deactivate_submit($form_id, $form_state) {
  $deactivate_events = array_filter($form_state['values']['form_events']);
  foreach ($deactivate_events as $key => $value) {
    rules_forms_deactivate_form($key);
  }
  rules_forms_clear_cache();
  drupal_set_message(t('The event settings have been saved.'));
}

/**
 * Activation page for a form ID.
 *
 * @param $form_id_activate
 *   The form ID of the form to be activated.
 * @param $form_page
 *   The encoded URI of the page on which the form was activated. This is used for
 *   redirecting the user back to that page once activation is completed.
 */
function rules_forms_activate($form_id_activate, $form_page) {
  if ($form_info = rules_forms_get_form_info($form_id_activate)) {
    return t('Events for %form_id have already been activated.', array('%form_id' => $form_id_activate));
  }
  return drupal_get_form('rules_forms_activate_confirm_form', $form_id_activate, $form_page);
}

/**
 * Confirmation form to activate events on a form.
 */
function rules_forms_activate_confirm_form($form, &$form_state, $form_id_activate, $form_page) {
  $default_form_label = drupal_ucfirst(str_replace('_', ' ', $form_id_activate));
  $form = array();
  $form['form_id_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom form label'),
    '#default_value' => $default_form_label,
    '#required' => TRUE,
    '#description' => t('Enter a custom label for use in the administration user interface.'),
  );
  $form_state['form_id_activate'] = $form_id_activate;
  $form_state['form_page'] = $form_page;

  $path = array();
  $path['path'] = isset($_GET['destination']) ? $_GET['destination'] : RULES_FORMS_ADMIN_PATH;

  $title = t('Are you sure you want to activate events for %form?', array('%form' => $form_id_activate));
  $msg = t('Once the activation is confirmed, events on this form can be used to trigger rules.');
  return confirm_form($form, $title, $path, $msg, t('Activate'), t('Cancel'));
}

/**
 * Submit handler for activation of a form.
 */
function rules_forms_activate_confirm_form_submit($form, &$form_state) {
  $path = urldecode($form_state['form_page']);
  $form_info = array(
    'form_id' => $form_state['form_id_activate'],
    'label' => $form_state['values']['form_id_label'],
  );

  // Save the form information but prevent cache rebuild until the form is visited.
  rules_forms_activate_form($form_info);
  rules_forms_clear_cache();
  drupal_set_message(t("%form has been activated.", array('%form' => $form_state['form_id_activate'])));

  // Redirect the user back to the form that was activated.
  $form_state['redirect'] = $path;
}
