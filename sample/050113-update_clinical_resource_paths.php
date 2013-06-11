<?php

/**
 * @file 050113-update_clinical_resource_paths.php
 *
 * Fix Clinical Practice nesting and menu links
 * https://www.pivotaltracker.com/story/show/48113159
 */

$updb_script = function() {
  db_query("UPDATE {url_alias} SET alias='practice-resources/clinical-practice' WHERE source='node/280' LIMIT 1");
  variable_set('pathauto_taxonomy_term_clinical_resources_pattern', 'practice-resources/clinical-practice/[term:name]');

  $vid = db_query("SELECT vid FROM {taxonomy_vocabulary} WHERE machine_name='clinical_resources' LIMIT 1")->fetchField();
  if (!empty($vid)) {
    $tids = db_query("SELECT tid FROM {taxonomy_term_data} WHERE vid=:vid", array(':vid' => $vid))->fetchAllKeyed(0,0);
    if (!is_array($tids)) {
      drupal_set_message(t('Problems finding Clinical Resources terms.'));
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

  return 1;
};
