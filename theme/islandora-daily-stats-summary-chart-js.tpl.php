<script type="text/javascript">
      // Load the Visualization API and the corechart package.
      google.charts.load('current', {'packages':['corechart']});

      // Set a callback to run when the Google Visualization API is loaded.
      google.charts.setOnLoadCallback(drawChart);

      // Callback that creates and populates a data table,
      // instantiates the pie chart, passes in the data and
      // draws it.
      function drawChart() {

        // Create the data table.
        var data = new google.visualization.DataTable();
        data.addColumn('string', 'Model');
        data.addColumn('number', 'Objects');
  <?php $last_value_name = (is_array($values) && count($values) > 0) ? $values[count($values)-1]['name'] : ''; ?>
        data.addRows([
  <?php foreach ($values as $value): ?>
          ['<?php print $value['name'] . "', " . $value['total_today']; ?>
           <?php print ($value['name'] == $last_value_name) ? ']' : '],'; ?>
  <?php endforeach; ?>
]);

        // Set chart options
        var options = {'title':'How many objects of each model',
                       'width':400,
                       'height':300};

        // Instantiate and draw our chart, passing in some options.
        var chart = new google.visualization.PieChart(document.getElementById('chart_summary_div'));
        chart.draw(data, options);
      }
</script>
