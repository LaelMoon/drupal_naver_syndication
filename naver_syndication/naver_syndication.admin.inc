<?php

/**
 * @file
 * Administrative page callbacks for the module.
 */

/**
 * Administration settings form.
 *
 * @see system_settings_form()
 */
function naver_syndication_settings_form() {

  $form['nsyndi_bearer_token_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Naver Syndication bearer access token'),
    '#default_value' => variable_get('nsyndi_bearer_token_key', ''),
	'#description' => t('Visit <a href="http://webmastertool.naver.com/syndi/status.naver" target="_blank">http://webmastertool.naver.com/syndi/status.naver</a> to obtain access token.'),
    '#required' => TRUE,
    '#size' => 120,
  );

  $form['nsyndi_feed_entries'] = array(
    '#type' => 'select',
    '#title' => t('Number of document to syndicate in each cron job'),
    '#default_value' => variable_get('nsyndi_feed_entries', 20),
    '#options' => drupal_map_assoc(range(1, 100)),
  );
  
  return system_settings_form($form);
}
