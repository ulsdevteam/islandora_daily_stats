    <?php foreach ($paged_models as $page_model_name => $paged_model): ?>
    <div class="value cell_numeric"><?php print number_format($paged_model['ratio_today'], 1); ?></div><label>pages per <?php print $page_model_name; ?></label><br>
    <?php endforeach; ?>
    <br>
