<?php

/**
 * @file updb/050313-revert_modules_for_pr100.php
 */

$updb_script = function() {
  updb_features_revert(array(
    'ons_chemo_card_landing_page',
    'ons_chemo_card',
    'ons_position_statement',
    'ons_pep_topics',
  ));

  return 1;
};
