<?php
/**
 * @file
 * Block area handlers. Insert a block inside of an area.
 */
class views_handler_field_block extends views_handler_field {

  function query() {
    // do nothing -- to override the parent query.
  }

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
    //this will only get custom blocks, "system" blocks don't all share a config name in Backdrop
    $config_names = config_get_names_with_prefix('block');
    $block_info = [];
    foreach ($config_names as $config_name) {
      $block_info[] = config_get($config_name);
    }
    // Allow modules to modify the block list.
    drupal_alter('block_list', $block_info);

    $options = [];
    foreach ($block_info as $block) {
      $options["{$block['delta']}"] = t('@delta@title', [
        '@delta' => $block['delta'],
        '@title' => (!empty($block['info'])) ? ' (' . $block['info'] .')' :
           (($block['title'] == '<none>' || $block['title'] == '') ? '' : ' (' . $block['title'] . ')'),
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
  function render($values) {

    if (!empty($this->options['block_to_insert'])) {

      $block = block_custom_block_load($this->options['block_to_insert']);
      if (empty($block)) {
        return;
      }
      $render_array = module_invoke('block', 'block_view', $block['delta']);
      if (!empty($this->options['title'])) {
        $render_array['subject'] = $this->options['title'];
      }

      return render($render_array['subject']) . render($render_array['content']);

    }

    return '';
  }
}
