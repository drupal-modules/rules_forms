<?php

/**
 * Handles construction and CRUD operations for form info.
 */
class RulesFormsMetadataController {

  private static $cache, $instance;

  /**
   * Allows class methods to optionally be accessed statically.
   * @param string $method
   * @param array $args
   */
  public static function __callStatic($method, array $args) {
    if (!isset(self::$instance)) {
      self::$instance = new RulesFormsMetadataController;
    }
    if (method_exists(self::$instance, $method)) {
      return call_user_func_array(array(self::$instance, $method), $args);
    }
  }

  /**
   * Activates a form and stores its form info.
   *
   * @param array $info
   *   An array of form information to store.
   *
   * @return RulesFormsMetadataWrapper
   *   A RulesFormsMetadataWrapper object with the new form info.
   */
  public function activate(array $info) {
    $info += array(
      'properties' => array('form' => array(), 'form_state' => array()),
      'rebuild' => TRUE,
    );
    $form_info = new RulesFormsMetadataWrapper($info);
    $fields = array(
      'form_id' => $form_info->getIdentifier(),
      'label' => $form_info->label(),
      'properties' => serialize($form_info->getPropertyInfo()),
      'settings' => serialize($form_info->getSettings()),
      'rebuild' => $form_info->needsRebuild() == TRUE ? 1 : 0,
    );
    $result = db_insert('rules_forms')
      ->fields($fields)
      ->execute();
    return $form_info;
  }

  /**
   * Loads form info from the database.
   *
   * @param string $form_id
   *   The form ID of the form whose information is to be loaded.
   *
   * @return RulesFormsMetadataWrapper|false
   *   The wrapped form data. FALSE if the data could not be found.
   */
  public function load($form_id) {
    if ($form_info = self::getCache() && isset($form_info[$form_id])) {
      return new RulesFormsMetadataWrapper($form_info[$form_id]);
    }

    $form_info = db_select('rules_forms', 'rf')
      ->fields('rf')
      ->condition('form_id', $form_id)
      ->execute()->fetchAssoc();
    if (!empty($form_info)) {
      $form_info['properties'] = unserialize($form_info['properties']);
      $form_info['settings'] = unserialize($form_info['settings']);
      return new RulesFormsMetadataWrapper($form_info);
    }
    return FALSE;
  }

  /**
   * Loads metadata for all active forms.
   *
   * @return array
   *   An array of RulesFormsMetadataWrapper objects keyed by form IDs.
   */
  public function loadAll() {
    if ($form_info = self::getCache()) {
      return $form_info;
    }

    $form_info = array();
    $results = db_select('rules_forms', 'rf')
      ->fields('rf')
      ->execute();

    foreach ($results as $info) {
      $info = (array) $info;
      $info['properties'] = unserialize($info['properties']);
      $info['settings'] = unserialize($info['settings']);
      $form_info[$info['form_id']] = new RulesFormsMetadataWrapper($info);
    }
    self::setCache($form_info);
    return $form_info;
  }

  /**
   * Updates form info in the database.
   *
   * @param RulesFormsMetadataWrapper $form_info
   *   The wrapped form data to update in the database.
   *
   * @return RulesFormsMetadataController
   *   The object on which the method was called.
   */
  public function update(RulesFormsMetadataWrapper $form_info) {
    $fields = array(
      'form_id' => $form_info->getIdentifier(),
      'label' => $form_info->label(),
      'properties' => serialize($form_info->getPropertyInfo()),
      'settings' => serialize($form_info->getSettings()),
      'rebuild' => $form_info->needsRebuild() == TRUE ? 1 : 0,
    );
    $result = db_update('rules_forms')
      ->fields($fields)
      ->condition('form_id', $fields['form_id'])
      ->execute();
    return $this;
  }

  /**
   * Disables the form.
   *
   * @return RulesFormsMetadataController
   *   The object on which the method was called.
   */
  public function deactivate($form_id) {
    $num = db_delete('rules_forms')
      ->condition('form_id', $form_id)
      ->execute();
    return $this;
  }

  /**
   * Returns a boolean value indicating whether a form is active.
   *
   * @param string $form_id
   *   The form ID of the form to check.
   *
   * @return bool
   *   A boolean value indicating whether the form has been activated by the user.
   */
  public function isActive($form_id) {
    if ($form_info = self::getCache()) {
      return isset($form_info[$form_id]);
    }
    $result = db_select('rules_forms', 'rf')
      ->fields('rf', array('form_id'))
      ->condition('form_id', $form_id)
      ->execute()->fetchObject();
    return !empty($result);
  }

  /**
   * Clears the form info cache.
   *
   * @return RulesFormsMetadataController
   *   The object on which the method was called.
   *
   * @see RulesFormsMetadataController::getCache()
   * @see RulesFormsMetadataController::setCache()
   */
  public function clearCache() {
    entity_info_cache_clear();
    rules_clear_cache();
    cache_clear_all('form_info', 'cache_rules_forms');
    return $this;
  }

  /**
   * Returns cached form info if it exists.
   *
   * @return array|false
   *   The cached form info or FALSE if it hasn't been cached.
   *
   * @see RulesFormsMetadataController::clearCache()
   * @see RulesFormsMetadataController::setCache()
   */
  protected static function getCache() {
    if (isset(self::$cache)) {
      return self::$cache;
    }
    elseif ($cache = cache_get('form_info', 'cache_rules_forms')) {
      self::$cache = $cache->data;
    }
    return isset(self::$cache) ? self::$cache : FALSE;
  }

  /**
   * Caches form info.
   *
   * @return RulesFormsMetadataController
   *   The called object.
   *
   * @see RulesFormsMetadataController::clearCache()
   * @see RulesFormsMetadataController::getCache()
   */
  protected static function setCache(array $form_info) {
    cache_set('form_info', $form_info, 'cache_rules_forms');
    self::$cache = $form_info;
    return TRUE;
  }

}

/**
 * Rules controller for Rules Forms entities.
 */
class RulesFormsRulesController extends EntityDefaultRulesController {

  protected $form_info = array();

  /**
   * Loads stored form information.
   */
  public function __construct($type) {
    $this->form_info = RulesFormsMetadataController::loadAll();
  }

  /**
   * Returns event info for all active forms.
   */
  public function eventInfo() {
    $events = array();
    foreach ($this->form_info as $form_id => $info) {
      $defaults = array(
        'group' => 'Rules Forms',
        'access callback' => 'rules_forms_integration_access',
        'variables' => array(
          'form' => array(
            'type' => 'form',
            'label' => $info->label(),
            'bundle' => $form_id,
          ),
          'form_state' => array(
            'type' => 'form_state',
            'label' => $info->label(),
            'bundle' => $form_id,
          ),
        ),
      );
      $events['rules_forms_' . $form_id . '_form_built'] = $defaults + array(
        'label' => t('@form is being built', array('@form' => $info->label())),
      );
      $events['rules_forms_' . $form_id . '_form_submit'] = $defaults + array(
        'label' => t('@form is submitted', array('@form' => $info->label())),
      );
      $events['rules_forms_' . $form_id . '_form_validate'] = $defaults + array(
        'label' => t('@form is being validated', array('@form' => $info->label())),
      );
    }
    return $events;
  }

  /**
   * Defines condition info for all forms.
   */
  public static function conditionInfo() {
    $conditions = array();
    $conditions['rules_forms_element_has_value'] = array(
      'label' => t('Form element property has value'),
      'group' => 'Rules Forms',
      'parameter' => array(
        'data' => array(
          'type' => '*',
          'label' => t('Form element property'),
          'description' => t('Select the form element property to evaluate. You can traverse the form structure using the data selector. Element properties are prefixed by a hash mark (#).'),
          'restriction' => 'selector',
          'wrapped' => TRUE,
          'allow null' => TRUE,
        ),
        'value' => array(
          'type' => '*',
          'label' => t('Value'),
          'description' => t('Value(s) assigned to the form element. If the form element allows multiple values, enter one value per line.'),
          'allow null' => TRUE,
          'optional' => TRUE,
        ),
        'regex' => array(
          'type' => 'boolean',
          'label' => t('Evaluate as regular expression'),
          'optional' => TRUE,
          'description' => t('Perform the data comparison using the provided value as a regular expression. '.
                             'To execute multiple regular expressions, enter one expression per line.'),
        ),
      ),
      'base' => 'rules_forms_condition_element_value',
      'callbacks' => array(
        'validate' => 'rules_forms_property_get_validate',
        'form_alter' => 'rules_forms_property_value_form_alter',
      ),
    );
    $conditions['rules_forms_element_changed'] = array(
      'label' => t('Form element value has changed'),
      'group' => 'Rules Forms',
      'parameter' => array(
        'form_state' => array(
          'type' => 'form_state',
          'label' => t('Form state'),
        ),
        'data' => array(
          'type' => '*',
          'label' => t('Form element'),
          'description' => t('Select the form element to evaluate. You can traverse the form structure using the data selector. Element properties are prefixed by a hash mark (#).'),
          'restriction' => 'selector',
          'wrapped' => TRUE,
          'allow null' => TRUE,
        ),
      ),
      'base' => 'rules_forms_condition_element_changed',
      'callbacks' => array(
        'validate' => 'rules_forms_element_validate',
        'form_alter' => 'rules_forms_property_value_form_alter',
      ),
    );
    $conditions['rules_forms_button_clicked'] = array(
      'label' => t('Form button was clicked'),
      'group' => 'Rules Forms',
      'parameter' => array(
        'form_state' => array(
          'type' => 'form_state',
          'label' => t('Form state'),
        ),
        'data' => array(
          'type' => 'struct',
          'label' => t('Form button'),
          'description' => t('Select the form element to evaluate. You can traverse the form structure using the data selector. Element properties are prefixed by a hash mark (#).'),
          'restriction' => 'selector',
          'wrapped' => TRUE,
          'allow null' => TRUE,
        ),
      ),
      'base' => 'rules_forms_condition_button_clicked',
    );
    return $conditions;
  }

  /**
   * Defines action info for all forms.
   */
  public static function actionInfo() {
    $actions = array();
    $actions['rules_forms_property_set'] = array(
      'label' => t('Set a property of an element in the form'),
      'group' => 'Rules Forms',
      'parameter' => array(
        'data' => array(
          'type' => '*',
          'label' => t('Form'),
          'restriction' => 'selector',
          'description' => t('Select the form element property to modify. You can traverse the form structure using the data selector. Element properties are prefixed by a hash mark (#).'),
          'wrapped' => TRUE,
          'allow null' => TRUE,
        ),
        'value' => array(
          'type' => '*',
          'label' => t('Value'),
          'description' => t('The new value to set for the specified data.'),
          'allow null' => TRUE,
          'optional' => TRUE,
        ),
      ),
      'base' => 'rules_action_data_set',
      'callbacks' => array(
        'validate' => 'rules_forms_property_set_validate',
        'info_alter' => 'rules_forms_action_property_set_info_alter',
      ),
    );
    $actions['rules_forms_set_error'] = array(
      'label' => t('Set an error on the form'),
      'group' => 'Rules Forms',
      'parameter' =>  array(
        'data' => array(
          'type' => 'struct',
          'label' => t('Targeted element'),
          'restriction' => 'selector',
          'description' => t('Select the form element to target (if any).'),
          'wrapped' => TRUE,
          'allow null' => TRUE,
        ),
        'message' => array(
          'type' => 'text',
          'label' => t('Message'),
          'optional' => TRUE,
          'description' => t('The message that should be displayed to the user.'),
        ),
      ),
      'base' => 'rules_forms_action_set_error',
      'callbacks' => array(
        'validate' => 'rules_forms_element_validate',
      ),
    );
    $actions['rules_forms_redirect'] = array(
      'label' => t('Set the redirect target of the form'),
      'group' => 'Rules Forms',
      'parameter' => array(
        'form_state' => array(
          'type' => 'form_state',
          'label' => t('Form state'),
        ),
        'path' => array(
          'type' => 'text',
          'label' => t('Path'),
        ),
        'query' => array(
          'type' => 'text',
          'label' => t('Query'),
          'optional' => TRUE,
        ),
        'fragment' => array(
          'type' => 'text',
          'label' => t('Fragment'),
          'optional' => TRUE,
        ),
      ),
      'base' => 'rules_forms_action_redirect',
      'help' => t('Enter a Drupal path, path alias, or external URL to redirect to. Enter (optional) queries after "?" and (optional) anchor after "#".'),
    );
    return $actions;
  }

}

class RulesFormsFormController extends EntityAPIController {}

class RulesFormsFormStateController extends EntityAPIController {}
