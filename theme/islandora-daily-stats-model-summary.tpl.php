  <div class="model_summary">
      <h3><?php print $name; ?> - <span class="today_value"><?php print number_format($total_today); ?> total objects</span></h3>
    <div class="value cell_numeric"><?php print number_format($new_today); ?></div><label>New today</label><br>
    <div class="value cell_numeric"><?php print number_format($new_in_last_week); ?></div><label>Added in last week</label><br>
    <div class="value cell_numeric"><?php print number_format($new_in_two_weeks); ?></div><label>Added in last two weeks</label><br>
    <div class="value cell_numeric"><?php print number_format($new_in_last_month); ?></div><label>Added in last 30 days</label><br>
    <div class="value cell_numeric"><?php print number_format($new_in_two_months); ?></div><label>Added in last 60 days</label><br>
  </div>
