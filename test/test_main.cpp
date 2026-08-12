#include <iostream>

#include "test_helper.hpp"

int main() {
  /* test RBT */
  std::cout << "\n===============  test RBT ===============\n";
  test_insert_search();
  test_remove();
  test_insert_emplace();
  test_heterogeneous_remove();
  test_invariant();
  test_get_successor();

  /* test LOB
  std::cout << "\n===============  test lob ===============\n";
  test_query();
  std::cout << '\n';
  test_order();
  */

  return 0;
}