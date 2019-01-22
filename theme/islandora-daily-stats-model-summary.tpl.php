  <div class="model_summary">
      <h3><?php print $name; ?> - <span class="today_value"><?php print number_format($total_today); ?> total objects</span></h3>
    <div class="value cell_numeric"><?php print number_format($new_today); ?></div><label>New today</label>
      <span class="cell_numeric sub_value"><?php print number_format($yesterday_count); ?></span><br>
    <div class="value cell_numeric"><?php print number_format($new_in_last_week); ?></div><label>Added in last week</label>
      <span class="cell_numeric sub_value"><?php print number_format($last_7_count); ?></span><br>
    <div class="value cell_numeric"><?php print number_format($new_in_two_weeks); ?></div><label>Added in last two weeks</label>
      <span class="cell_numeric sub_value"><?php print number_format($prev_7_days); ?></span><br>
    <div class="value cell_numeric"><?php print number_format($new_in_last_month); ?></div><label>Added in last 30 days</label>
      <span class="cell_numeric sub_value"><?php print number_format($last_30_count); ?></span><br>
    <div class="value cell_numeric"><?php print number_format($new_in_two_months); ?></div><label>Added in last 60 days</label>
      <span class="cell_numeric sub_value"><?php print number_format($prev_30_days); ?></span><br>
    <div class="value cell_numeric"><?php print number_format($new_last_year); ?></div><label>Added in last year (365 days)</label>
      <span class="cell_numeric sub_value"><?php print number_format($prev_365_days); ?></span><br>
  </div>
