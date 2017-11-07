<?php

/**
 * @file
 * Main page handler function for Islandora Daily Stats module.
 */

/**
 * This will control the variables that will make up any stats report.
 *
 * @param string $models
 *   CSV of models for the report data -- uses "allmodels" when not filtered.
 * @param string $date_start
 *   (optional) Start date for report data.
 * @param string $date_end
 *   (optional) Ending date for report data.
 * 
 * @return string
 *   Page markup html.
 */
function islandora_daily_stats_page($models, $date_start = '', $date_end = '') {
  module_load_include('inc', 'islandora_daily_stats', 'includes/utilities');
  $filters = array();
  if ($models <> 'allmodels') {
    $filters['model'] = urldecode($models);
  }

  $filter_links = islandora_daily_stats_make_filter_links($filters);
  $result =  islandora_daily_stats_get_stats($filters);
  $models_colors = $result['color_key_models'];
  ksort($models_colors);
  return theme('islandora_daily_stats_page', array(
      'filter_links' => $filter_links,
      'stats_table' => $result['stats_table'],
      'node_data' => $result['node_data'],
      'line_data' => $result['line_data'],
      'pixels_per_month_span' => $result['pixels_per_month_span'],
      'color_key' => theme('islandora_daily_stats_color_key', array('model_colors' => $models_colors)),
  ));
}
