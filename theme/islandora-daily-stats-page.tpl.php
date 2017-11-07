
<div class="filter_links"><?php print $filter_links; ?></div>
<span id="tip"><span id="2d"></span></span>
    <div id="graph"></div>
    <div id="color_key"><?php print $color_key; ?></div>
<?php print $stats_table; ?>
<script type="text/javascript">
  var vis = d3.select("#graph")
            .append("svg");

     var w = 904,
         h = 404;
     vis.attr("width", w)
        .attr("height", h)
        .attr("id", 'svg');

  var month_pixels = <?php print round($pixels_per_month_span); ?>;
  var cur_line_left = 904 - month_pixels;

  while (cur_line_left >= 0) {
    vis.append("line")          // attach a line
    .style("stroke", "#999")
     .attr("x1", function(d) { return cur_line_left })
     .attr("y1", function(d) { return 0 })
     .attr("x2", function(d) { return cur_line_left })
     .attr("y2", function(d) { return 404 });

    cur_line_left = cur_line_left - month_pixels;
  }

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

  jQuery("#graph svg").mousemove(function(e){handleMouseMove(e);});

  // show tooltip when mouse hovers over dot
  function handleMouseMove(e){
    var canvasOffset = jQuery("#graph").offset();
    var offsetX = canvasOffset.left - jQuery(window).scrollLeft();
    var offsetY = canvasOffset.top - jQuery(window).scrollTop();
    var tipCanvas = document.getElementById("tip");
    var tipCtx = document.getElementById("svg");
    mouseX=parseInt(e.clientX-offsetX);
    mouseY=parseInt(e.clientY-offsetY);

    // Put your mousemove stuff here
    var hit = false;
    var useTitle = '';
    for (var i = 0; i < nodes.length; i++) {
      var dot = nodes[i];
      var dotR = dot.r + 6;
      var dx = mouseX - dot.x;
      var dy = mouseY - dot.y;
      if (dx * dx + dy * dy < dotR) {
        tipCanvas.style.left = (mouseX) + "px";
        if (mouseY < 300) {
          tipCanvas.style.top = (mouseY + 44) + "px";
        }
        else {
          tipCanvas.style.top = (mouseY - 10) + "px";
        }
        useTitle = dot.title;
        hit = true;
      }
    }

    document.getElementById("2d").textContent = useTitle;
    if (!hit) {
      tipCanvas.style.left = "-400px";
    }
  }

</script>