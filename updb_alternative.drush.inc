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
 * }
 */

