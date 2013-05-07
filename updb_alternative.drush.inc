<?php

/**
 * @file updb_alternative.drush.inc
 */


// The directory where update drush scripts will be kept.
define('UPDB_SCRIPTS_PATH', 'updb');

/**
 * Implements hook_drush_command().
 *
 * @return
 *   An associative array describing your command(s).
 *
 * @see drush_parse_command()
 */
function updb_alternative_drush_command() {
  $items = array();

  $items['updb-alternative'] = array(
    'description' => "Run database update scripts.",
    'aliases' => array('upda'),
  );

  return $items;
}

/**
 * Implements hook_drush_help().
 */
function updb_alternative_drush_help($section) {
  switch ($section) {
  case 'drush:updb-alternative':
    return dt("Run database update scripts.");
  }
}

/**
 * Run database update scripts.
 */
function drush_updb_alternative() {

  $scriptpath = DRUPAL_ROOT . '/' . UPDB_SCRIPTS_PATH;

  $filenames = scandir($scriptpath);

  $history = variable_get('updb_history', array());

  foreach ($filenames as $filename) {
    if (empty($history[$filename]) && $filename != 'updb.php' && preg_match('/\.php$/i', $filename)) {
      $filepath = "$scriptpath/$filename";
      include($filepath);
      if (isset($updb_script)) {
        $status = $updb_script();
        unset($updb_script);

        if (!empty($status)) {
          $history[$filename] = 1;
          variable_set('updb_history', $history);
        }
      }
    }
  }
}

/**
 * The scripts need to be in the following format, defining $updb_script as an anonymous function
 *
 * $updb_script = function() {
 *   $example_var = 'Hello World';
 *   calling_another_function();
 *
 *   return empty($success) ? TRUE : FALSE;
 * };
 */

/**
* Revert specified features.
*
* @TODO Check that it really is Forced features revert.  Currently an exact
* copy of the function initially placed in feature_projects.install.
*
* Code mostly taken from drush.
*/
function updb_alternative_features_revert($modules) {
  module_load_include('inc', 'features', 'features.export');
  features_include();
  foreach ($modules as $module) {
    if (($feature = feature_load($module, TRUE)) && module_exists($module)) {
      $components = array();
      // Forcefully revert all components of a feature.
      foreach (array_keys($feature->info['features']) as $component) {
        if (features_hook($component, 'features_revert')) {
          $components[] = $component;
        }
      }
    }
    foreach ($components as $component) {
      features_revert(array($module => array($component)));
    }
  }
}
function updb_features_revert($modules) {
  updb_alternative_features_revert($modules);
}

/**
 * Helper for changing a term Pathauto pattern and updating all terms with aliases
 * under that vocabulary.
 */
function updb_alternative_pathauto_update_term($pathauto_variable_name, $new_pattern, $vocab_machine_name, $rebuild = TRUE) {
  variable_set($pathauto_variable_name, $new_pattern);

  $vid = db_query("SELECT vid FROM {taxonomy_vocabulary} WHERE machine_name=:machine_name LIMIT 1", array(':machine_name' => $vocab_machine_name))->fetchField();
  if (!empty($vid)) {
    $tids = db_query("SELECT tid FROM {taxonomy_term_data} WHERE vid=:vid", array(':vid' => $vid))->fetchAllKeyed(0,0);
    if (!is_array($tids)) {
      drupal_set_message(t('Problems finding !machine_name terms.', array('!machine_name' => $vocab_machine_name)));
    }
    foreach ($tids as $tid) {
      $like = "taxonomy/term/$tid";
      db_delete('url_alias')
        ->condition('source', db_like($like) . '%', 'LIKE')
        ->execute();
    }

    db_delete('pathauto_persist')
      ->condition('entity_type', 'taxonomy_term')
      ->condition('entity_id', $tids, 'IN')
      ->execute();

    // Update path aliases for these term ids
    pathauto_taxonomy_term_update_alias_multiple($tids, 'bulkupdate');
  }
}

