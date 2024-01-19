<?php

/**
 * @file
 * Block area handlers. Insert a block inside of an area.
 */
class views_handler_area_block extends views_handler_area {

  function option_definition() {

    $options = parent::option_definition();
    $options['block_to_insert'] = array('default' => '');
    $options['title'] = array('default' => '');

    return $options;

  }

  /**
   * Default options form that provides the label widget that all fields
   * should have.
   */
  function options_form(&$form, &$form_state) {

    parent::options_form($form, $form_state);
    $test = config_get('comment.recent.info');
    $all_block_infos = [];
    foreach (module_implements('block_info') as $module) {
      if ($blocks = module_invoke($module, 'block_info')) {
        foreach ($blocks as $delta => $info) {
          $moddelta = $module . ':' . $delta;
          $all_block_infos[$moddelta] = views_block_area_get_block($module, $delta, $info);
        }
      }
    }
    // Allow modules to modify the block list.
    backdrop_alter('block_list', $all_block_infos);

    $options = [];
    foreach ($all_block_infos as $block) {
      $options["{$block->module}.{$block->delta}"] = t('@delta@title', [
        '@delta' => $block->delta,
        '@title' => (!empty($block->info)) ? ' (' . $block->info .')' :
           (($block->title == '<none>' || $block->title == '') ? '' : ' (' . $block->title . ')'),
      ]);
    }

    $form['block_to_insert'] = array(
      '#type' => 'select',
      '#title' => t('Block to insert'),
      '#default_value' => $this->options['block_to_insert'],
      '#description' => t('The block to insert into this area.'),
      '#options' => $options,
    );

    $form['title'] = array(
      '#type' => 'textfield',
      '#title' => t('Block title'),
      '#default_value' => $this->options['title'],
      '#description' => t('Override the title for the block. Use &lt;none&gt; to display no title, or leave blank to use the block title from block settings.'),
    );
  }

  /**
   * Render the area
   */
  function render($empty = false) {

    if (!empty($this->options['block_to_insert'])) {
      list($module, $delta) = explode('.', $this->options['block_to_insert']);
      $block = views_block_area_get_block($module, $delta);
      if (empty($block)) {
        return;
      }
      $render_array = views_block_area_get_renderable_array($block);
      if (!empty($this->options['title'])) {
        $render_array['#content']->title = $this->options['title'];
      }

      return render($render_array);
    }

    return '';
  }

}
