<?php 

/**
 * @file
 * Utilities functions for Islandora Daily Stats module.
 */

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
  $query->orderBy('mc.date_d', 'ASC');
  //  $query->where('WEEKDAY(mc.date_d) = 1');
  $query->where('mc.date_d >= DATE_SUB(NOW(), INTERVAL 6 MONTH)');
  $results = $query->execute()->fetchAll();

  $rows = array();
  $max_y = 0;
  $max_x = 0;
  $dates = $models = $values = array();
  $min_date = 0;
  foreach ($results as $result_obj) {
    if ($result_obj->pitt_count > $max_y) {
      $max_y = $result_obj->pitt_count;
    }
    if (!array_key_exists($result_obj->model_desc, $models)) {
      $models[$result_obj->model_desc] = $result_obj->model_desc;
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
    $date = date('m/d/Y', strtotime($result_obj->date_d));
    $dates[$date] = $date;
    $values[$result_obj->model_desc][$date] = $result_obj->pitt_count;
    $max_x++;
  }

  $model_keys = array_keys($models);

  // need to make an array of line_data
  foreach ($rows as $idx => $row_array) {
    $rows[$idx]['pitt_count'] = array('data' => number_format($rows[$idx]['pitt_count']), 'class'=>'cell_numeric');
    // $rows[$idx]['nonpitt_count'] = array('data' => number_format($rows[$idx]['nonpitt_count']), 'class'=>'cell_numeric');
  }

  $headers = array('Date', 'Model', 'Pitt Count'); // , 'Non-Pitt Count');
  $stats_table = theme('table', array('header' => $headers, 'rows' => $rows));


  return array(
    'stats_table' => $stats_table,
    'models' => $model_keys,
    'values' => $values,
    'dates' => $dates,
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
    $this_pct_over_30_days = ($model_stats['last_30_days'] <> 0) ? 100 * ($model_stats['total_today'] - $model_stats['last_30_days']) / $model_stats['last_30_days'] : 0;
    $this_pct_over_60_days = ($model_stats['prev_30_days'] <> 0) ? 100 * ($model_stats['total_today'] - $model_stats['prev_30_days']) / $model_stats['prev_30_days'] : 0;
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

  $paged_model_ratios = array();
  $paged_models = array(
      'Book' => 'BookPage',
      'Manuscript' => 'ManuscriptPage',
      'NewspaperIssue' => 'NewspaperPage');

  $m = array();
  foreach ($models_summary as $model_summary) {
    $m[$model_summary['name']] = $model_summary;
  }
  foreach ($paged_models as $model_name => $page_model_name) {
    if (array_key_exists($model_name, $m) && array_key_exists($page_model_name, $m)) {
      $paged_model_ratios[$model_name] = array(
         'ratio_today' => round($m[$page_model_name]['total_today'] / $m[$model_name]['total_today'], 1),
         'ratio_last_30_days' => round($m[$page_model_name]['last_30_days'] / $m[$model_name]['last_30_days'], 1),
         'ratio_prev_30_days' => round($m[$page_model_name]['prev_30_days'] / $m[$model_name]['prev_30_days'], 1)
      );
    }

  }

  $page_averages = theme('islandora_daily_stats_page_averages_summary', array('paged_models' => $paged_model_ratios));
  
  $general_summary = islandora_daily_stats_general_summary();
  $overview_summary = array(
      'name' => 'Overview',
      'description' => 'This is an overview of the various content types that ' .
        'were ingested over the last 30 days.</p>' .
      '<p>The largest growth over the last 30 days has been: ' . implode(", ", $big_growth_last_30) . '.</p>' .
      '<p>The largest growth over the previous 30 day period: ' . implode(", ", $big_growth_prev_30) . ".",
      'today' => $general_summary['today'],
      'yesterday' => $general_summary['yesterday'],
      'page_averages' => $page_averages,
    );

  $summary = theme('islandora_daily_stats_overview_summary', $overview_summary) . 
       islandora_daily_stats_js_summary_block($models_summary);

  $themed_model_summaries = array();
  foreach ($models_summary as $model_summary) {   
    $themed_model_summaries[] = theme('islandora_daily_stats_model_summary', $model_summary);
  }

  // Create another overview summary block that displays the Pages/book averages.


  $summary .= implode("", $themed_model_summaries);

  return $summary;
}

function islandora_daily_stats_js_summary_block($models_summary) {  
  $values = array();
  // loop through $models_summary and use  'name' and 'total_today' values.
  foreach ($models_summary as $model_summary) {
    if (isset($model_summary['name']) && isset($model_summary['total_today']) && $model_summary['total_today'] > 0) {
      $values[] = array(
          'name' => $model_summary['name'],
          'total_today' => $model_summary['total_today'],
      );
    }
  }
  $js = theme('islandora_daily_stats_summary_chart_js', array('values' => $values));
  return $js;
}

function islandora_daily_stats_get_last_date($model, $offset = '') {
  $sql = "SELECT " . (($offset) ? 'DATE_SUB(sm.date_d, INTERVAL ' . $offset . ' DAY)' : 'sm.date_d') . " as `date_d` " .
         "FROM islandora_stats_contentmodel_counts sm " .
         "LEFT OUTER JOIN islandora_stats_models m ON m.model_id = sm.model_id " .
         "WHERE (m.model = '" . $model->model . "') ORDER BY sm.date_d DESC LIMIT 1";
  $query = db_query($sql);
  $result = $query->fetchAll();
  return (isset($result[0]) ? $result[0]->date_d : 0);
}

function islandora_daily_stats_get_count_on_date($model, $date) {
  $sql = "SELECT sm.pitt_count AS pitt_count " .
         "FROM islandora_stats_contentmodel_counts sm " .
         "LEFT OUTER JOIN islandora_stats_models m ON m.model_id = sm.model_id " .
         "WHERE (m.model = '" . $model->model . "') AND (sm.date_d = '" . $date . "')";
  $query = db_query($sql);
  $result = $query->fetchAll();
  return (isset($result[0]) ? $result[0]->pitt_count: 0);
}

/**
 * Gets the stats for a given model on various dates.
 *
 * @param type $model
 */
function islandora_daily_stats_get_model_summary($model) {
  if (!is_object($model)) { return array(); }
  $today = islandora_daily_stats_get_last_date($model);
  $yesterday = islandora_daily_stats_get_last_date($model, 1);
  $ago_7_days = islandora_daily_stats_get_last_date($model, 7);
  $ago_14_days = islandora_daily_stats_get_last_date($model, 14);
  $ago_30_days = islandora_daily_stats_get_last_date($model, 30);
  $ago_60_days = islandora_daily_stats_get_last_date($model, 60);

  $today_count = islandora_daily_stats_get_count_on_date($model, $today);
  $yesterday_count = islandora_daily_stats_get_count_on_date($model, $yesterday);
  $new_today = $today_count - $yesterday_count;
  $last_7_count = islandora_daily_stats_get_count_on_date($model, $ago_7_days);
  $prev_7_days = islandora_daily_stats_get_count_on_date($model, $ago_14_days);
  $new_in_last_week = $today_count - $last_7_count;
  $new_in_two_weeks = $today_count - $prev_7_days;
  $last_30_count = islandora_daily_stats_get_count_on_date($model, $ago_30_days);
  $prev_30_days = islandora_daily_stats_get_count_on_date($model, $ago_60_days);
  $new_in_last_month = $today_count - $last_30_count;
  $new_in_two_months = $today_count - $prev_30_days;

  return array(
      'name' => $model->model_desc,
      'total_today' => $today_count,
      'new_today' => $new_today,
      'yesterday_count' => $yesterday_count,
      'new_in_last_week' => $new_in_last_week,
      'last_7_count' => $last_7_count,
      'new_in_two_weeks' => $new_in_two_weeks,
      'prev_7_days' => $prev_7_days,
      'new_in_last_month' => $new_in_last_month,
      'last_30_count' => $last_30_count,
      'new_in_two_months' => $new_in_two_months,
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
