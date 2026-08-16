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
  fuzz_test();

  /* test L3_LOB */
  std::cout << "\n=============  test L3_LOB  =============\n";
  test_query();
  std::cout << '\n';
  test_order();

  /* test L2_LOB */
  std::cout << "\n=============  test L2_LOB  =============\n";
  test_apply_snapshot();
  
  return 0;
}