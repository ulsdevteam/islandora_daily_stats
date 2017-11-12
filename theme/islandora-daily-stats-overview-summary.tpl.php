  <div id="chart_summary_div"></div>

  <div class="overview_summary">
    <h3><?php print $name; ?></h3>
    <p><?php print $description; ?></p>
    <div class="value cell_numeric"><?php print number_format($today)  ; ?></div><label>Total Objects Today</label><br>
    <div class="value cell_numeric"><?php print number_format($yesterday)  ; ?></div><label>New Objects yesterday</label>
    <br>
  </div>
<br style="clear:both">
