#include <cassert>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>

#include "BinarySearchTree.h"
using namespace std;

void testInt() {
  {
    BinarySearchTree<int> tree;
    tree.insert(1);
    assert(tree.inorder() == "1");
    tree.insert(2);
    assert(tree.inorder() == "1 2");
    tree.insert(3);
    assert(tree.inorder() == "1 2 3");
    tree.insert(4);
    assert(tree.inorder() == "1 2 3 4");
    tree.insert(5);
    assert(tree.inorder() == "1 2 3 4 5");
    tree.insert(6);
    assert(tree.inorder() == "1 2 3 4 5 6");
    tree.insert(7);
    assert(tree.inorder() == "1 2 3 4 5 6 7");
    tree.insert(8);
    assert(tree.inorder() == "1 2 3 4 5 6 7 8");
    tree.insert(9);
    assert(tree.inorder() == "1 2 3 4 5 6 7 8 9");
    std::cout << "test integer 1 passed!\n";
  }

  {
    BinarySearchTree<int> tree;
    for (int i = 0; i < 20; i++)
      tree.insert(i);
    assert(tree.inorder() == "0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19");
  }
}

void testString() {
  {
    BinarySearchTree<string> tree;
    tree.insert("aaa");
    tree.insert("bbb");
    tree.insert("ccc");
    assert(tree.inorder() == "aaa bbb ccc");

    cout << "test string 1 passed\n";
  }
}

int main() {
  testInt();
  testString();

  return 0;
}