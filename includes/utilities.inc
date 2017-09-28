<?php 

/**
 * This will take the filters that are passed and query the stats tables -
 * converting the rows a themed table.
 *
 * @param type $filters
 */
function islandora_daily_stats_get_stats_table($filters) {
  $query = db_select('islandora_stats_contentmodel_counts', 'mc');
    // based on filters:
    //   ->condition('ur.rid', array(2,4,),'IN') // set to an array of roles and pass. In this case I knew ahead of time what rids I wanted to filter by.
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
  $query->range(0,1000);
  $results = $query->execute()->fetchAll();
  $rows = array();
  foreach ($results as $result_obj) {
    $rows[] = array('date' => $result_obj->date, 'model' => $result_obj->model_desc,
      'pitt_count' => $result_obj->pitt_count, 'nonpitt_count' => $result_obj->nonpitt_count);
  }
  $headers = array('Date', 'Model', 'Pitt Count', 'Non-Pitt Count');
  return theme('table', array('header' => $headers, 'rows' => $rows));
}

/**
 * Based on the currently active filters, make quick links to each of the models
 * and a form to post using GET to combine models or set date range.
 *
 * @param type $filters
 */
function islandora_daily_stats_make_filter_links($filters) {
  $query = db_select('islandora_stats_models', 'm');
  $query->addField('m', 'model');
  $query->addField('m', 'model_desc');
  $query->orderBy('model_desc');
  $results = $query->execute()->fetchAll();
  
  $selected_models_arr = array();
  if (isset($filters['models'])) {
    $selected_models_arr = explode(",", $filters['models']);
  }

  $filter_links = array();
  foreach ($results as $result_obj) {
    $filter_links[] = (isset($selected_models_arr[$result_obj->model])) ?
        '<span class="active">' . $result_obj->model_desc . '</span>' :
        l($result_obj->model_desc, 'islandora/dailystats/' . $result_obj->model);
  }
  return implode(" | ", $filter_links);
}