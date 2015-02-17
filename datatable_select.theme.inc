<?php
/**
 * @file
 * datatable_select.theme.inc
 */

/**
 * @param $variables
 * @return string
 */
function theme_datatable_select($variables) {
  $element = $variables['element'];
  $rows = array();
  $header = $element['#header'];

  if (!empty($element['#rows'])) {
    foreach (element_children($element['#rows']) as $key) {
      $row = array('data' => array());
      if (isset($element['#rows'][$key]['#attributes'])) {
        $row += $element['#rows'][$key]['#attributes'];
      }
      $row['data'][] = drupal_render($element[$key]);
      foreach ($element['#header'] as $fieldname => $title) {
        $row['data'][] = $element['#rows'][$key][$fieldname];
      }
      $rows[] = $row;
    }

    if ($element['#js_select']) {
      drupal_add_js('misc/tableselect.js');
      array_unshift($header, array('class' => array('select-all')));
    }
    else {
      array_unshift($header, '');
    }

    if (!isset($element['#attributes']['id'])) {
      $element['#attributes']['id'] = _datatables_get_id();
    }
  }

  return theme('datatable_table', array(
    'header' => $header,
    'rows' => $rows,
    'empty' => $element['#empty'],
    'attributes' => $element['#attributes'],
    'datatable_select' => $element['#datatable_select'],
  ));
}

/**
 * @param $variables
 * @return string
 */
function theme_datatable_table($variables) {
  $header = $variables['header'];
  $rows = $variables['rows'];
  $attributes = $variables['attributes'];
  $caption = $variables['caption'];
  $colgroups = $variables['colgroups'];
  $sticky = $variables['sticky'];
  $empty = $variables['empty'];
  $options = $variables['datatable_select'];

  if (count($header) && $sticky) {
    drupal_add_js('misc/tableheader.js');
    $attributes['class'][] = 'sticky-enabled';
  }

  $output = '<table' . drupal_attributes($attributes) . ">\n";
  if (isset($caption)) {
    $output .= '<caption>' . $caption . "</caption>\n";
  }

  if (count($colgroups)) {
    foreach ($colgroups as $number => $colgroup) {
      $attributes = array();
      if (isset($colgroup['data'])) {
        foreach ($colgroup as $key => $value) {
          if ($key == 'data') {
            $cols = $value;
          }
          else {
            $attributes[$key] = $value;
          }
        }
      }
      else {
        $cols = $colgroup;
      }

      if (is_array($cols) && count($cols)) {
        $output .= ' <colgroup' . drupal_attributes($attributes) . '>';
        $i = 0;
        foreach ($cols as $col) {
          $output .= ' <col' . drupal_attributes($col) . ' />';
        }
        $output .= " </colgroup>\n";
      }
      else {
        $output .= ' <colgroup' . drupal_attributes($attributes) . " />\n";
      }
    }
  }

  if (!count($rows) && $empty) {
    $header_count = 0;
    foreach ($header as $header_cell) {
      if (is_array($header_cell)) {
        $header_count += isset($header_cell['colspan']) ? $header_cell['colspan'] : 1;
      }
      else {
        $header_count++;
      }
    }
    $rows[] = array(array(
      'data' => $empty,
      'colspan' => $header_count,
      'class' => array('empty', 'message'),
    ));
  }

  // Format the table header:
  if (count($header)) {
    $tablesort = isset($options['tablesort']) ? $options['tablesort'] : datatable_select_tablesort_init($header);
    // HTML requires that the thead tag has tr tags in it followed by tbody
    // tags. Using ternary operator to check and see if we have any rows.
    $output .= (count($rows) ? ' <thead><tr>' : ' <tr>');
    foreach ($header as $k => $cell) {
      if (!is_array($cell)) {
        $cell = array('data' => $cell);
      }

      $cell = datatable_select_tablesort_header($cell, $header, $tablesort, $options);
      $output .= _theme_table_cell($cell, TRUE);
    }
    // Using ternary operator to close the tags based on whether or not there are rows
    $output .= (count($rows) ? " </tr></thead>\n" : "</tr>\n");
  }
  else {
    $tablesort = array();
  }

  // Format the table rows:
  if (count($rows)) {
    $output .= "<tbody>\n";
    $flip = array(
      'even' => 'odd',
      'odd' => 'even',
    );
    $class = 'even';
    foreach ($rows as $number => $row) {
      // Check if we're dealing with a simple or complex row
      if (isset($row['data'])) {
        $cells = $row['data'];
        $no_striping = isset($row['no_striping']) ? $row['no_striping'] : FALSE;

        // Set the attributes array and exclude 'data' and 'no_striping'.
        $attributes = $row;
        unset($attributes['data']);
        unset($attributes['no_striping']);
      }
      else {
        $cells = $row;
        $attributes = array();
        $no_striping = FALSE;
      }
      if (count($cells)) {
        // Add odd/even class
        if (!$no_striping) {
          $class = $flip[$class];
          $attributes['class'][] = $class;
        }

        // Build row
        $output .= ' <tr' . drupal_attributes($attributes) . '>';
        $i = 0;
        foreach ($cells as $cell) {
          $cell = tablesort_cell($cell, $header, $tablesort, $i++);
          $output .= _theme_table_cell($cell);
        }
        $output .= " </tr>\n";
      }
    }
    $output .= "</tbody>\n";
  }

  $output .= "</table>\n";
  return $output;
}
