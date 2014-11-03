<?php

/**
 * @file
 * Various non-administration page callbacks for the module.
 */


/**
 * Query for node feeds.
 */
function naver_syndication_node_feed(stdClass $node) {
  if (!node_access('view', $node)) {
    return drupal_access_denied();
  }

  $nodes = array($node->nid);

  $feed_info = array();
  $feed_info['html_url'] = url('node/' . $node->nid, array('absolute' => TRUE));
  $feed_info['atom_url'] = url('node/' . $node->nid . '/syndication.naver', array('absolute' => TRUE));
  _nsyndi_print_feed($nodes, $feed_info);
}

/**
 * Print document feeds.
 */
function _nsyndi_print_feed(array $nids, array $feed_info) {
  $output = '';
  $last_mod = 0;

  $nodes = node_load_multiple($nids);
  foreach ($nodes as $node) {
    $item_text = '';

    $node->link = url("node/$node->nid", array('absolute' => TRUE));

    node_build_content($node, 'rss');

    if (!empty($node->content)) {
      // We render node contents and force links to be last.
      $links = drupal_render($node->content['links']);
      $item_text .= drupal_render($node->content) . $links;
    }

    $item = array(
      'published' => $node->created,
      'updated' => $node->changed,
      'author' => $node->name ? $node->name : variable_get('anonymous', 'Anonymous'),
      'content' => $item_text,
      // @todo How to have summary and/or full body?
    );

    $output .= theme('naver_syndication_feed_item', array('title' => $node->title, 'link' => $node->link, 'item' => $item, 'stitle' => variable_get('site_name', 'Drupal')));
    
  }

  // Merge some default values.
  $feed_info += array(
    'title' => variable_get('site_name', 'Drupal'),
    'subtitle' => variable_get('site_slogan', ''),
  );

  $output = theme('naver_syndication_feed', array('feed_info' => $feed_info, 'items' => $output,'author' => $node->name ? $node->name : variable_get('anonymous', 'Anonymous')));

  drupal_add_http_header('Content-Type', 'application/xml; charset=utf-8');
  print $output;
}

/**
 * Overall themed output.
 */
function theme_naver_syndication_feed($variables) {
  $output = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  $output .= '<feed xmlns="http://webmastertool.naver.com">' . "\n";
  $output .= '  <id>' . check_plain($variables['feed_info']['atom_url']) . "</id>\n";
  $output .= '  <title>' . check_plain($variables['feed_info']['title']) . "</title>\n";
  $output .= "    <author><name>" . check_plain($variables['author']) . "</name></author>\n";
  $output .= '  <updated>' . gmdate(DATE_ATOM, REQUEST_TIME) . "</updated>\n";
  $output .= '  <link rel="site" href="http://' . $_SERVER['HTTP_HOST'] . '" title="' . check_plain($variables['feed_info']['title']) . '" />' . "\n";
  $output .= $variables['items'];
  $output .= "</feed>\n";
  return $output;
}

/**
 * Theming for a single entry in the feed.
 */
function theme_naver_syndication_feed_item($variables) {
	//print_r($variables);
  $variables['item'] += array(
    'id' => $variables['link'],
    'summary' => '',
    'content' => '',
    'author' => '',
  );

  $output = "  <entry>\n";
  $output .= "    <id>" . check_plain($variables['item']['id']) . "</id>\n";
  //$output .= "    <link rel=\"alternate\" type=\"text/html\" href=\"" . check_plain($variables['link']) . "\" />\n";
  $output .= "    <title><![CDATA[" . check_plain($variables['title']) . "]]></title>\n";
  if ($variables['item']['author']) {
    $output .= "    <author><name>" . check_plain($variables['item']['author']) . "</name></author>\n";
  }

  $output .= "    <updated>" . gmdate(DATE_ATOM, $variables['item']['updated']) . "</updated>\n";
  $output .= "    <published>" . gmdate(DATE_ATOM, $variables['item']['published']) . "</published>\n";
  $output .= '    <link rel="via" href="http://' . $_SERVER['HTTP_HOST'] . '" title="' . check_plain($variables['stitle']) . '" />' . "\n";

  if ($variables['item']['summary']) {
    $output .= "    <summary type=\"html\"><![CDATA[<div xmlns=\"http://www.w3.org/1999/xhtml\">" . $variables['item']['summary'] . "</div>]]></summary>\n";
  }
  if ($variables['item']['content']) {
    $output .= "    <content type=\"html\"><![CDATA[<div xmlns=\"http://www.w3.org/1999/xhtml\">" . $variables['item']['content'] . "</div>]]></content>\n";
  }
  

  $output .= "  </entry>\n";
  return $output;
}

