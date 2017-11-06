<?php

/**
 * @file
 * Main page handler function for Islandora Daily Stats Summary.
 */

/**
 * This will control the variables that will make up the stats summary report.
 *
 * @return string
 *   Page markup html.
 */
function islandora_daily_stats_summary_page() {
  module_load_include('inc', 'islandora_daily_stats', 'includes/utilities');

  $filters = array();
  $filter_links = islandora_daily_stats_make_filter_links($filters);
  $summary_markup = islandora_daily_stats_get_stats_summary();
  return theme('islandora_daily_stats_summary_page', array(
      'summary' => $summary_markup,
      'filter_links' => $filter_links,
  ));
}
