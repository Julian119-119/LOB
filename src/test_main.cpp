#include <iostream>

#include "test_helper.hpp"

int main() {
  /* test BST */
  std::cout << "\n===============  test BST ===============\n";
  testSmallest();
  testInt();
  testString();

  /* test RBT */
  std::cout << "\n===============  test RBT ===============\n";
  testRedBlackTrees();

  /* test LOB */
  std::cout << "\n===============  test lob ===============\n";
  test_query();
  test_order();
  // testspend();

  return 0;
}