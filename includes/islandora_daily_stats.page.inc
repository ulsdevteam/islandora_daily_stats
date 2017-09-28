<?php

function islandora_daily_stats_page($models, $date_start = '', $date_end = '') {
  module_load_include('inc', 'islandora_daily_stats', 'includes/utilities');

  if ($models <> 'allmodels') {
    $filters['model'] = urldecode($models);
  }

  $filter_links = islandora_daily_stats_make_filter_links($filters);
  $stats_table = islandora_daily_stats_get_stats_table($filters);

  return theme('islandora_daily_stats_page', array('filter_links' => $filter_links,
    'stats_table' => $stats_table,
  ));
}

