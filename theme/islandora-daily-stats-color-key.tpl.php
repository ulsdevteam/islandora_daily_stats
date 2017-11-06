<?php foreach ($model_colors as $model => $color_RGB): ?>
    <div class="model">
        <span><?php print $model; ?></span> <div class="color_sample" style="background-color:<?php print $color_RGB; ?>">&nbsp;</div>
    </div>
<?php endforeach; ?>
