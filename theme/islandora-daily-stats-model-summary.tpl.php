  <div class="model_summary">
    <h3><?php print $name; ?></h3>
    <div class="value cell_numeric"><?php print number_format($today); ?></div><label>Objects today</label><br>
    <div class="value cell_numeric"><?php print number_format($last_7_days); ?></div><label>Last 7 days</label><br>
    <div class="value cell_numeric"><?php print number_format($prev_7_days); ?></div><label>Prev 7 days</label><br>
    <div class="value cell_numeric"><?php print number_format($last_30_days); ?></div><label>Last 30 days</label><br>
    <div class="value cell_numeric"><?php print number_format($prev_30_days); ?></div><label>Prev 30 days</label><br>
  </div>
