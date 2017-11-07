<?php 

/**
 * @file
 * Utilities functions for Islandora Daily Stats module.
 */

/**
 * This returns an array of simple and distinct color values that should be 
 * easy to distinguish between for any chart.
 * 
 * @return array
 *   Array with the key being the color string and value is hex value.
 */
function islandora_daily_stats_get_default_chart_colors() {
  return array(
    'Red' => '#e6194b',
    'Green' => '#3cb44b',
    'Yellow' => '#ffe119',
    'Blue' => '#0082c8',
    'Orange' => '#f58231',
    'Purple' => '#911eb4',
    'Cyan' => '#46f0f0',
    'Magenta' => '#f032e6',
    'Lime' => '#d2f53c',
    'Pink' => '#fabebe',
    'Teal' => '#008080',
    'Lavender' => '#e6beff',
    'Brown' => '#aa6e28',
    'Navy' => '#000080',
    'Maroon' => '#800000',
    'Mint' => '#aaffc3',
    'Olive' => '#808000',
    'Coral' => '#ffd8b1',
    'Beige' => '#fffac8',
    'Grey' => '#808080',
    'White' => '#FFFFFF',
    'Black' => '#000000',
  );
}

/**
 * Create the stats  based on filters.
 *
 * @param array $filters
 *   Array of filters that uses the field alias as the key.
 *
 * @return array
 *   'stats_table': Drupal themed table for stats report in HTML markup.
 *   'json_data': json encoded values
 */
function islandora_daily_stats_get_stats($filters) {
  $chart_colors = islandora_daily_stats_get_default_chart_colors();
  $pop_these_colors = $chart_colors;
  $query = db_select('islandora_stats_contentmodel_counts', 'mc');
  if (count($filters) < 1) {
    // Add a filter for last 6 months...
//    $query->where('mc.date_d >= DATE_SUB(NOW(), INTERVAL 6 MONTH)');
  }
  else {
    foreach ($filters as $filter_field => $filter_value) {
      if (strstr($filter_value, ',')) {
        $filter_value = str_replace(",", "','", addslashes($filter_value));
        $query->where($filter_field . " IN ('" . $filter_value . "')");
      }
      else {
        $query->where($filter_field . ' = ' . "'" . $filter_value . "'");
      }
    }
  }
  $query->innerJoin('islandora_stats_models', 'm', 'm.model_id = mc.model_id');
  $query->addField('mc', 'date_d');
  $query->addField('m', 'model_desc');
  $query->addField('mc', 'pitt_count');
//  $query->addField('mc', 'nonpitt_count');
  $query->orderBy('mc.date_d', 'ASC');
  $query->where('mc.date_d >= DATE_SUB(NOW(), INTERVAL 6 MONTH)');
//  $query->range(0, 10);
  $results = $query->execute()->fetchAll();
  $rows = array();
  $max_y = 0;
  $max_x = 0;
  $model_colors = array();
  $models = array();
  $min_date = 0;
  foreach ($results as $result_obj) {
    if ($result_obj->pitt_count > $max_y) {
      $max_y = $result_obj->pitt_count;
    }
    if (!array_key_exists($result_obj->model_desc, $models)) {
      if (count($pop_these_colors) < 1) {
        $pop_these_colors = $chart_colors;
      }
      $color_key = array_shift($pop_these_colors);
      $models[$result_obj->model_desc] = $color_key;
    }
    if (!$min_date) {
      $min_date = strtotime($result_obj->date_d);
    }
    $rows[] = array(
      'date' => date('m/d/Y', strtotime($result_obj->date_d)),
      'model' => $result_obj->model_desc,
      'pitt_count' => $result_obj->pitt_count,
      // 'nonpitt_count' => 0, // $result_obj->nonpitt_count,
    );
    $max_x++;
  }

  $max_date = strtotime($result_obj->date_d);
  $days_spanned = ($max_date - $min_date) / 60 / 60 / 24;
  $pixels_per_month_span = 30 * 904 / $days_spanned;

  // The json array must be normalized to fit in the chart canvas which is
  // 900 x 400 pixels;
  $node_array = array();
  $model_to_color_index = array();

  $last_date = '';
  $date_ticks = 0;
  foreach ($rows as $idx => $row_array) {
    $x = round(900 * $idx / $max_x, 0);
    $y = 404 - round(400 * $row_array['pitt_count'] / $max_y, 0);
    $class = 'nodes';
    if ($last_date <> $row_array['date']) {
      $date_ticks++;
      $last_date = $row_array['date'];
    }
    $r = (($date_ticks % 5) == 0) ? ((($date_ticks % 10) == 0) ? 5 : 3) : 3;
    $node_array[] = array('x' => $x, 'y' => $y, 
        'title' => number_format($row_array['pitt_count']) . " " . $row_array['model'] . " objects (" . $row_array['date'] . ")",
        'r' => $r, 'color' => $models[$row_array['model']]);
  }

  $line_array_models = $line_array = array();
  // need to make an array of line_data
  foreach ($rows as $idx => $row_array) {
    /*
    if ($idx > 0) {
      // Will use the x and y from previous calculation iteration.
      $line_array_models[$row_array['model']]['source'] = array('x' => $x, 'y' => $y);
    }
    else {
      $line_array_models[$row_array['model']]['source'] = array('x' => 0, 'y' => 0);
    }
    $x = 904 - round(900 * $idx / $max_x, 0);
    $y = 404 - round(400 * $row_array['pitt_count'] / $max_y, 0);
    $line_array_models[$row_array['model']]['target'] = array('x' => $x, 'y' => $y);
    */
    $rows[$idx]['pitt_count'] = array('data' => number_format($rows[$idx]['pitt_count']), 'class'=>'cell_numeric');
    // $rows[$idx]['nonpitt_count'] = array('data' => number_format($rows[$idx]['nonpitt_count']), 'class'=>'cell_numeric');
  }

  $headers = array('Date', 'Model', 'Pitt Count'); // , 'Non-Pitt Count');
  $stats_table = theme('table', array('header' => $headers, 'rows' => $rows));

/*  foreach ($line_array_models as $model => $line_array_model) {
    $line_array = array_merge($line_array, $line_array_models[$model]);
  }
 */
  $node_data = json_encode($node_array);
  $line_data = json_encode($line_array);

  return array(
    'stats_table' => $stats_table,
    'node_data' => $node_data,
    'pixels_per_month_span' => $pixels_per_month_span,
    'line_data' => $line_data,
    'color_key_models' => $models,
  );
}

/**
 * Based on the currently active filters.
 * 
 * TODO: add a form to post using GET to combine models or set date range.
 * 
 * This will make links to each of the models.  This will render each filter
 * that is applied as "active" and all other possible model filters will be
 * links to a report that is filtered on that model value.
 *
 * Multiple filters can be combined using CSV.
 *
 * @param array $filters
 *   Each array element represents a filter that is applied to the report.
 */
function islandora_daily_stats_make_filter_links($filters) {
  $query = db_select('islandora_stats_models', 'm');
  $query->addField('m', 'model');
  $query->addField('m', 'model_desc');
  $query->orderBy('model_desc');
  $results = $query->execute()->fetchAll();

  $selected_models_arr = array();
  if (isset($filters['model'])) {
    $selected_models_arr = explode(",", $filters['model']);
  }

  $top_report_links = $grouped_links = $filter_links = array();

  $top_report_links[] = l('Islandora Daily Stats Summary', 'admin/reports/islandora/dailystats/summary');
  $top_report_links[] = l('Islandora Daily Stats', 'admin/reports/islandora/dailystats/stats/allmodels');

  $grouped_links[] = l('ALL MODELS', 'admin/reports/islandora/dailystats/stats/allmodels');
  $grouped_links[] = l('All Pages', 'admin/reports/islandora/dailystats/stats/pageCModel,manuscriptPageCModel,newspaperPageCModel');
  $grouped_links[] = l('All Parent Objects', 'admin/reports/islandora/dailystats/stats/bookCModel,sp-audioCModel,collectionCModel,findingAidCModel,sp_large_image_cmodel,manuscriptCModel,newspaperIssueCModel,sp_pdf,sp_videoCModel');
  foreach ($results as $result_obj) {
    $filter_links[] = (!(array_search($result_obj->model, $selected_models_arr) === FALSE)) ?
        '<span class="active">' . $result_obj->model_desc . '</span>' :
        l($result_obj->model_desc, 'admin/reports/islandora/dailystats/stats/' . $result_obj->model);
  }
  return implode(" | ", $top_report_links) . "<br>" .
        implode(" | ", $grouped_links) . "<br>" . implode(" | ", $filter_links);
}

function islandora_daily_stats_get_stats_summary() {
  $query = db_select('islandora_stats_models', 'm');
  $query->addField('m', 'model');
  $query->addField('m', 'model_desc');
  $query->orderBy('model_desc');
  $results = $query->execute()->fetchAll();

  $models_summary = array();
  $max_today_comp_last_30 = array();
  $max_today_comp_prev_30 = array();
  $cur_max_over_60_days = $cur_max_over_30_days = 0;

  foreach ($results as $model) {
    $model_stats = islandora_daily_stats_get_model_summary($model);
    $models_summary[] = $model_stats;
    $this_pct_over_30_days = $model_stats['last_30_days'] ? 100 * ($model_stats['today'] - $model_stats['last_30_days']) / $model_stats['last_30_days'] : 0;
    $this_pct_over_60_days = $model_stats['prev_30_days'] ? 100 * ($model_stats['today'] - $model_stats['prev_30_days']) / $model_stats['prev_30_days'] : 0;
    $max_today_comp_last_30[$model->model_desc] = $this_pct_over_30_days;
    $max_today_comp_prev_30[$model->model_desc] = $this_pct_over_60_days;
  }
  asort($max_today_comp_last_30);
  asort($max_today_comp_prev_30);
  $big_growth_last_30 = $big_growth_prev_30 = array();

  $i = 0;
  $start_at = count($max_today_comp_last_30) - 3;
  foreach ($max_today_comp_last_30 as $model_desc => $v) {
    if ($i >= $start_at) {
      $big_growth_last_30[] = $model_desc . " up by " . round($v, 2) . "%";
    }
    $i++;
  }
  $i = 0;
  $start_at = count($max_today_comp_prev_30) - 3;
  foreach ($max_today_comp_prev_30 as $model_desc => $v) {
    if ($i >= $start_at) {
      $big_growth_prev_30[] = $model_desc . " up by " . round($v, 2) . "%";
    }
    $i++;
  }

  $big_growth_last_30 = array_reverse($big_growth_last_30);
  $big_growth_prev_30 = array_reverse($big_growth_prev_30);
  
  $general_summary = islandora_daily_stats_general_summary();
  $overview_summary = array(
      'name' => 'Overview',
      'description' => 'This is an overview of the various content types that ' .
        'were ingested over the last 30 days.</p>' .
      '<p>The largest growth over the last 30 days has been: ' . implode(", ", $big_growth_last_30) . '.</p>' .
      '<p>The largest growth over the previous 30 day period: ' . implode(", ", $big_growth_prev_30) . ".",
      'today' => $general_summary['today'],
      'yesterday' => $general_summary['yesterday'],
    );

  $summary = theme('islandora_daily_stats_overview_summary', $overview_summary);


  foreach ($models_summary as $model_summary) {
    $summary .= theme('islandora_daily_stats_model_summary', $model_summary);
  }

  return $summary;
}

/**
 * Gets the stats for a given model on various dates.
 *
 * @param type $model
 */
function islandora_daily_stats_get_model_summary($model) {
  if (!is_object($model)) { return array(); }
  $today = date("Y/m/d", time());
  $sql = "SELECT sm.pitt_count AS pitt_count " .
         "FROM islandora_stats_contentmodel_counts sm " .
         "LEFT OUTER JOIN islandora_stats_models m ON m.model_id = sm.model_id " .
         "WHERE (m.model = '" . $model->model . "') AND (sm.date_d = '" . $today . "')";
  $query = db_query($sql);
  $result = $query->fetchAll();
  $today_count = (isset($result[0]) ? $result[0]->pitt_count: 0);

  $ago_7_days = date("Y/m/d", time() - 7 * 60 * 60 * 24);
  $sql = "SELECT AVG(sm.pitt_count) AS pitt_count " .
         "FROM islandora_stats_contentmodel_counts sm " .
         "LEFT OUTER JOIN islandora_stats_models m ON m.model_id = sm.model_id " .
         "WHERE (m.model = '" . $model->model . "') AND (sm.date_d >= '" . $ago_7_days . "')";
  $query = db_query($sql);
  $result = $query->fetchAll();
  $last_7_count = (isset($result[0]) ? $result[0]->pitt_count: 0);

  $ago_14_days = date("Y/m/d", time() - 14 * 60 * 60 * 24);
  $sql = "SELECT AVG(sm.pitt_count) AS pitt_count " .
         "FROM islandora_stats_contentmodel_counts sm " .
         "LEFT OUTER JOIN islandora_stats_models m ON m.model_id = sm.model_id " .
         "WHERE (m.model = '" . $model->model . "') AND (sm.date_d >= '" . $ago_14_days . "' AND sm.date_d < '" . $ago_7_days . "')";
  $query = db_query($sql);
  $result = $query->fetchAll();
  $prev_7_days = (isset($result[0]) ? $result[0]->pitt_count: 0);

  $ago_30_days = date("Y/m/d", time() - 30 * 60 * 60 * 24);
  $sql = "SELECT AVG(sm.pitt_count) AS pitt_count " .
         "FROM islandora_stats_contentmodel_counts sm " .
         "LEFT OUTER JOIN islandora_stats_models m ON m.model_id = sm.model_id " .
         "WHERE (m.model = '" . $model->model . "') AND (sm.date_d >= '" . $ago_30_days . "')";
  $query = db_query($sql);
  $result = $query->fetchAll();
  $last_30_count = (isset($result[0]) ? $result[0]->pitt_count: 0);

  $ago_60_days = date("Y/m/d", time() - 60 * 60 * 60 * 24);
  $sql = "SELECT AVG(sm.pitt_count) AS pitt_count " .
         "FROM islandora_stats_contentmodel_counts sm " .
         "LEFT OUTER JOIN islandora_stats_models m ON m.model_id = sm.model_id " .
         "WHERE (m.model = '" . $model->model . "') AND (sm.date_d >= '" . $ago_60_days . "' AND sm.date_d < '" . $ago_30_days . "')";
  $query = db_query($sql);
  $result = $query->fetchAll();
  $prev_30_days = (isset($result[0]) ? $result[0]->pitt_count: 0);

  return array(
      'name' => $model->model_desc,
      'today' => $today_count,
      'last_7_days' => $last_7_count,
      'prev_7_days' => $prev_7_days,
      'last_30_days' => $last_30_count,
      'prev_30_days' => $prev_30_days,
    );
}

/**
 * Gets the stats for a given model on various dates.
 *
 * @param type $model
 */
function islandora_daily_stats_general_summary() {
  $today = date("Y/m/d", time());
  $sql = "SELECT SUM(sm.pitt_count) AS pitt_count " .
         "FROM islandora_stats_contentmodel_counts sm " .
         "LEFT OUTER JOIN islandora_stats_models m ON m.model_id = sm.model_id " .
         "WHERE (sm.date_d = '" . $today . "')";
  $query = db_query($sql);
  $result = $query->fetchAll();
  $today_count = (isset($result[0]) ? $result[0]->pitt_count: 0);

  $yesterday = date("Y/m/d", time() - 60 * 60 * 24);
  $sql = "SELECT SUM(sm.pitt_count) AS pitt_count " .
         "FROM islandora_stats_contentmodel_counts sm " .
         "LEFT OUTER JOIN islandora_stats_models m ON m.model_id = sm.model_id " .
         "WHERE (sm.date_d = '" . $yesterday . "')";
  $query = db_query($sql);
  $result = $query->fetchAll();
  $day_before_count = (isset($result[0]) ? $result[0]->pitt_count: 0);

  return array('today' => $today_count, 'yesterday' => $today_count - $day_before_count);
}
