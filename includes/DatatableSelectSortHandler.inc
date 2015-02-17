<?php
/**
 * @file
 * Contains class DatatableSelectSortHandler.
 */

/**
 * Handler class for performing sort comparisons.
 */
class DatatableSelectSortHandler {
  /**
   * The table sort information.
   *
   * @var array
   */
  protected $tablesort;

  /**
   * The return value for when $a less than $b.
   *
   * @var int
   */
  protected $value1 = -1;

  /**
   * The return value for when $a is not less than $b.
   *
   * @var int
   */
  protected $value2 = 1;

  /**
   * Creates a new instance of DatatableSelectSortHandler.
   *
   * @param array $tablesort
   */
  public function __construct(array &$tablesort) {
    $this->tablesort = &$tablesort;
    // If the sort direction is descending, we need to flip the returned values
    // for the $a < $b comparison. Therefore, lets only do this once by
    // reassigning the $this->value1 and $this->value2 property values.
    if ('desc' == $this->tablesort['sort']) {
      $this->value1 = 1;
      $this->value2 = -1;
    }
  }

  /**
   * Performs sort comparison operation between two rows within a table sort.
   *
   * @param array $a
   *   The element for the left hand side of the comparison.
   * @param array $b
   *   The element for the right hand side of the compaison.
   *
   * @return int
   *   Returns an integer representing the sort direction.
   */
  public function sort($a, $b) {
    $a_value = isset($a[$this->tablesort['field']]) ? $a[$this->tablesort['field']] : $a;
    $b_value = isset($b[$this->tablesort['field']]) ? $b[$this->tablesort['field']] : $b;

    if ($a_value == $b_value) {
      return 0;
    }

    return $a_value < $b_value ? $this->value1 : $this->value2;
  }
}
