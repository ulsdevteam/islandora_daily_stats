  <div class="overview_summary">
    <h3><?php print $name; ?></h3>
    <p><?php print $description; ?></p>
    <div class="value cell_numeric"><?php print number_format($yesterday)  ; ?></div><label>Objects yesterday</label><br>
    <div class="value cell_numeric"><?php print number_format($last_7_days); ?></div><label>Last 7 days</label><br>
    <div class="value cell_numeric"><?php print number_format($prev_7_days); ?></div><label>Prev 7 days</label><br>
    <div class="value cell_numeric"><?php print number_format($last_30_days); ?></div><label>Last 30 days</label><br>
    <div class="value cell_numeric"><?php print number_format($prev_30_days); ?></div><label>Prev 30 days</label><br>
  </div>
