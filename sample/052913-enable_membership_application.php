<?php

/**
 * @file 052913-enable_membership_application.php
 *
 * Enable the ons_membership_application module.
 */

$updb_script = function() {
  return module_enable(array('ons_membership_application'));
};

