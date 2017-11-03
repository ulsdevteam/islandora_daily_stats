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
    'Beige' => '#fffac8',
    'Maroon' => '#800000',
    'Mint' => '#aaffc3',
    'Olive' => '#808000',
    'Coral' => '#ffd8b1',
    'Navy' => '#000080',
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
    // Add a filter for last year...
    $query->where('mc.date >= DATE_SUB(NOW(), INTERVAL 1 YEAR)');
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
  $query->addField('mc', 'date');
  $query->addField('m', 'model_desc');
  $query->addField('mc', 'pitt_count');
  $query->addField('mc', 'nonpitt_count');
  $query->orderBy('mc.date', 'ASC');
  $query->range(0, 1500);
  $results = $query->execute()->fetchAll();
  $rows = array();
  $max_y = 0;
  $max_x = 0;
  $model_colors = array();
  $models = array();
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
    $rows[] = array(
      'date' => date('m/d/Y', strtotime($result_obj->date)),
      'model' => $result_obj->model_desc,
      'pitt_count' => $result_obj->pitt_count,
      'nonpitt_count' => $result_obj->nonpitt_count,
    );
    $max_x++;
  }

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
        'title' => $row_array['pitt_count'] . " " . $row_array['model'] . " objects (" . $row_array['date'] . ")",
        'r' => $r, 'color' => $models[$row_array['model']]);
  }

  $line_array_models = $line_array = array();
  // need to make an array of line_data
  foreach ($rows as $idx => $row_array) {
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
    $rows[$idx]['pitt_count'] = array('data' => number_format($rows[$idx]['pitt_count']), 'class'=>'cell_numeric');
    $rows[$idx]['nonpitt_count'] = array('data' => number_format($rows[$idx]['nonpitt_count']), 'class'=>'cell_numeric');
  }

  $headers = array('Date', 'Model', 'Pitt Count', 'Non-Pitt Count');
  $stats_table = theme('table', array('header' => $headers, 'rows' => $rows));

  foreach ($line_array_models as $model => $line_array_model) {
    $line_array = array_merge($line_array, $line_array_models[$model]);
  }
  $node_data = json_encode($node_array);
  $line_data = json_encode($line_array);

  return array(
    'stats_table' => $stats_table,
    'node_data' => $node_data,
    'line_data' => $line_data,
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

  $filter_links = array();
  $filter_links[] = (count($selected_models_arr) < 1) ? '<span class="active">All models</span>' :
        l('ALL MODELS', 'admin/reports/islandora/dailystats/allmodels');
  foreach ($results as $result_obj) {
    $filter_links[] = (!(array_search($result_obj->model, $selected_models_arr) === FALSE)) ?
        '<span class="active">' . $result_obj->model_desc . '</span>' :
        l($result_obj->model_desc, 'admin/reports/islandora/dailystats/' . $result_obj->model);
  }
  return implode(" | ", $filter_links);
}
