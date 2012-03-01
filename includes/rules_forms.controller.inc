<?php

/**
 * Base class for CRUD operations for Rules Forms metadata.
 */
abstract class RulesFormsMetadataController {

  /**
   * Allows class methods to optionally be accessed statically.
   * @param string $method
   * @param array $args
   */
  public static function __callStatic($method, array $args) {
    $class = get_called_class();
    if (!isset(self::$instance) || get_class(self::$instance) !== $class) {
      self::$instance = new $class;
    }
    if (method_exists(self::$instance, $method)) {
      return call_user_func_array(array(self::$instance, $method), $args);
    }
  }

  /**
   * Create an instance of the data. This method is not required.
   *
   * @return RulesFormsMetadataWrapper
   *   The created data, wrapped.
   */
  public function create(array $info) {}

  /**
   * Loads metadata. This method must be implemented by sub-classes.
   *
   * @param string|int $id
   *   The identifier for use in loading data.
   */
  abstract public function load($id);

  /**
   * Loads all metadata of the given type.
   *
   * @return array
   *   An array of RulesFormsMetadataWrapper objects.
   */
  abstract public function loadAll();

  /**
   * Updates the metadata.
   *
   * @param RulesFormsMetadataWrapper $wrapper
   *   The wrapped metadata to update.
   *
   * @return RulesFormsMetadataController
   *   The called object.
   */
  public function update(RulesFormsMetadataWrapper $wrapper) {}

  /**
   * Deletes stored metadata. This method is not required of sub-classes.
   *
   * @param string|int $id
   *   The identifier of the metadata to delete.
   *
   * @return RulesFormsMetadataController
   *   The called object.
   */
  public function delete($id) {}
}

/**
 * Form controller class.
 */
class RulesFormsFormController extends RulesFormsMetadataController {
  
  protected static $cache, $instance;

  /**
   * Activates a form and stores its form info.
   *
   * @param array $info
   *   An array of form information to store.
   *
   * @return RulesFormsMetadataWrapper
   *   A RulesFormsMetadataWrapper object with the new form info.
   */
  public function create(array $info) {
    $info += array(
      'properties' => array('form' => array(), 'form_state' => array()),
      'rebuild' => TRUE,
    );
    $form_info = new RulesFormsFormWrapper('form', $info);
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
      return new RulesFormsFormWrapper('form', $form_info[$form_id]);
    }

    $form_info = db_select('rules_forms', 'rf')
      ->fields('rf')
      ->condition('form_id', $form_id)
      ->execute()->fetchAssoc();
    if (!empty($form_info)) {
      $form_info['properties'] = unserialize($form_info['properties']);
      $form_info['settings'] = unserialize($form_info['settings']);
      return new RulesFormsFormWrapper('form', $form_info);
    }
    return FALSE;
  }

  /**
   * Loads metadata for all active forms.
   *
   * @return array
   *   An array of RulesFormsFormWrapper objects keyed by form IDs.
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
      $form_info[$info['form_id']] = new RulesFormsFormWrapper('form', $info);
    }
    self::setCache($form_info);
    return $form_info;
  }

  /**
   * Updates form info in the database.
   *
   * @param RulesFormsFormWrapper $form_info
   *   The wrapped form data to update in the database.
   *
   * @return RulesFormsFormController
   *   The object on which the method was called.
   */
  public function update(RulesFormsFormWrapper $form_info) {
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
  public function delete($form_id) {
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
