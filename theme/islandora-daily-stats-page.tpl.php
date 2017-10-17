
<div class="filter_links"><?php print $filter_links; ?></div>
<span id="tip"><span id="2d"></span></span>
<div id="graph"></div>
<?php print $stats_table; ?>
<script type="text/javascript">
  var vis = d3.select("#graph")
            .append("svg");

     var w = 904,
         h = 404;
     vis.attr("width", w)
        .attr("height", h)
        .attr("id", 'svg');

  var nodes = <?php print $node_data; ?>;
            
  vis.selectAll("circle .nodes")
     .data(nodes)
     .enter()
     .append("svg:circle")
//     .attr("title", function(d) { return d.title; })
     .attr("class", function(d) { return d.class; })
     .attr("cx", function(d) { return d.x; })
     .attr("cy", function(d) { return d.y; })
     .attr("r", function(d) { return d.r + "px"; })
     .attr("fill", function(d) { return d.color; });

  var links = <?php print $line_data; ?>

  vis.selectAll(".line")
     .data(links)
     .enter()
     .append("line")
     .attr("x1", function(d) { return d.source.x })
     .attr("y1", function(d) { return d.source.y })
     .attr("x2", function(d) { return d.target.x })
     .attr("y2", function(d) { return d.target.y })
     .style("stroke", "rgb(6,120,155)");

  jQuery("#graph svg").mousemove(function(e){handleMouseMove(e);});

  // show tooltip when mouse hovers over dot
  function handleMouseMove(e){
    var canvasOffset = jQuery("#graph").offset();
    var offsetX = canvasOffset.left;
    // Why does subtract 300 work?
    var offsetY = canvasOffset.top;

    var tipCanvas = document.getElementById("tip");
    var tipCtx = document.getElementById("svg");

    mouseX=parseInt(e.clientX-offsetX);
    mouseY=parseInt(e.clientY-offsetY);

    // Put your mousemove stuff here
    var hit = false;
    var useTitle = '';
    for (var i = 0; i < nodes.length; i++) {
      var dot = nodes[i];
      var dotR = dot.r + 2;
      var dx = mouseX - dot.x;
      var dy = mouseY - dot.y;
      if (dx * dx + dy * dy < dotR) {
        tipCanvas.style.left = (dot.x) + "px";
        tipCanvas.style.top = (dot.y - 40) + "px";
        useTitle = dot.title;
        hit = true;
      }
    }
    document.getElementById("2d").textContent = useTitle;
    if (!hit) {
      tipCanvas.style.left = "-200px"; //display = 'none';
    }
    else {
      // jQuery("#tip").show();
    }
  }

</script>