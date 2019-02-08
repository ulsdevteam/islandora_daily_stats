  <div class="model_summary">
      <h3><?php print $name; ?> - <span class="today_value"><?php print number_format($total_today); ?> total objects</span></h3>
    <label>today</label>
      <span class="value cell_numeric<?php print ($new_today < 0 ? " negative" : ""); ?>"><?php print ($new_today < 0  ? '(' : '') . number_format(abs($new_today)) . ($new_today < 0 ? ')' : ''); ?></span>
      <span class="cell_numeric sub_value"><?php print number_format($yesterday_count); ?></span><br>
    <label>last week</label>
      <span class="value cell_numeric<?php print ($new_in_last_week < 0 ? " negative" : ""); ?>"><?php print ($new_in_last_week < 0  ? '(' : '') . number_format(abs($new_in_last_week)) . ($new_in_last_week < 0  ? ')' : ''); ?></span>
      <span class="cell_numeric sub_value"><?php print number_format($last_7_count); ?></span><br>
    <label>last two weeks</label>
      <span class="value cell_numeric<?php print ($new_in_two_weeks < 0 ? " negative" : ""); ?>"><?php print ($new_in_two_weeks < 0  ? '(' : '') . number_format(abs($new_in_two_weeks)) . ($new_in_two_weeks < 0  ? ')' : ''); ?></span>
      <span class="cell_numeric sub_value"><?php print number_format($prev_7_days); ?></span><br>
    <label>last 30 days</label>
      <span class="value cell_numeric<?php print ($new_in_last_month < 0 ? " negative" : ""); ?>"><?php print ($new_in_last_month < 0  ? '(' : '') . number_format(abs($new_in_last_month)) . ($new_in_last_month < 0  ? ')' : ''); ?></span>
      <span class="cell_numeric sub_value"><?php print number_format($last_30_count); ?></span><br>
    <label>last 60 days</label>
      <span class="value cell_numeric<?php print ($new_in_two_months < 0 ? " negative" : ""); ?>"><?php print ($new_in_two_months < 0  ? '(' : '') . number_format(abs($new_in_two_months)) . ($new_in_two_months < 0  ? ')' : ''); ?></span>
      <span class="cell_numeric sub_value"><?php print number_format($prev_30_days); ?></span><br>
    <label>last year (365 days)</label>
      <span class="value cell_numeric<?php print ($new_last_year < 0 ? " negative" : ""); ?>"><?php print ($new_last_year < 0  ? '(' : '') . number_format(abs($new_last_year)) . ($new_last_year < 0  ? ')' : ''); ?></span>
      <span class="cell_numeric sub_value"><?php print number_format($prev_365_days); ?></span><br>
  </div>
