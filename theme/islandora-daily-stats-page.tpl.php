<div class="filter_links"><?php print $filter_links; ?></div>
<div id="chart_dailystats_div" style="width: 1000px; height: 500px"></div>
    <script type="text/javascript">
      google.charts.load('current', {'packages':['line']});
      google.charts.setOnLoadCallback(drawChart);

    function drawChart() {
<?php $i = 0; ?>

  var data = new google.visualization.DataTable();
      data.addColumn('string', 'Date');
<?php foreach ($models as $k => $model) : ?>
      data.addColumn('number', '<?php print $model; ?>');
<?php $last_model = $model; ?>
<?php endforeach; ?>

      data.addRows([
<?php foreach ($dates as $date): ?>
<?php print ($i > 0) ? ',' : ''; ?>
          [<?php print "'" . $date . "'"; ?><?php foreach ($models as $k => $model): ?>, <?php print $values[$model][$date]; ?><?php endforeach; ?>]

<?php $i++; ?>
<?php endforeach; ?>
      ]);

      var options = {
        chart: {
          title: 'Number of objects over time',
          subtitle: 'by object model'
        },
        width: 1000,
        height: 500,
        vAxis: {
          format: 'decimal'
        },
        axes: {
          x: {
            0: {side: 'bottom'}
          }
        }
      };

      var chart = new google.charts.Line(document.getElementById('chart_dailystats_div'));

      chart.draw(data, google.charts.Line.convertOptions(options));
    }
  </script>

<?php print $stats_table; ?>
