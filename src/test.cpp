#include <cassert>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>

#include "BinarySearchTree.hpp"
#include "LOB_type.hpp"
#include "RedBlackTree.hpp"
using namespace std;

void testSmallest() {
  {
    BinarySearchTree<int> tree;
    tree.insert(50);
    tree.insert(40);
    tree.insert(60);
    tree.insert(35);
    tree.insert(45);
    tree.insert(55);
    tree.insert(65);
    assert(*tree.get_leftmost_node() == 35);

    tree.remove(35);
    assert(*tree.get_leftmost_node() == 40);
    tree.remove(40);
    assert(*tree.get_leftmost_node() == 45);
    tree.remove(45);
    assert(*tree.get_leftmost_node() == 50);
    tree.remove(50);
    assert(*tree.get_leftmost_node() == 55);
    tree.insert(30);
    assert(*tree.get_leftmost_node() == 30);
  }

  {
    BinarySearchTree<int> tree;
    for (int i = 0; i < 100; i++) {
      tree.insert(i);
    }
    assert(*tree.get_leftmost_node() == 0);

    tree.remove(100);
    assert(*tree.get_leftmost_node() == 0);

    for (int i = 0; i < 50; i++) {
      tree.remove(i);
    }
    assert(*tree.get_leftmost_node() == 50);
  }

  {
    BinarySearchTree<int> tree;
    for (int i = 10; i >= 0; i--) {
      tree.insert(i);
    }
    assert(*tree.get_leftmost_node() == 0);

    for (int i = 0; i < 4; i++) {
      tree.remove(i);
    }
    assert(*tree.get_leftmost_node() == 4);

    cout << "testSmallest passed!" << endl;
  }
}

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
    tree.remove(2);
    assert(tree.inorder() == "1 3 4 5 6 7 8 9");
    std::cout << "test integer passed!\n";
  }
}

void testString() {
  {
    BinarySearchTree<string> tree;
    tree.insert("aaa");
    tree.insert("bbb");
    tree.insert("ccc");
    assert(tree.inorder() == "aaa bbb ccc");

    cout << "test string passed!\n";
  }
}

void testRedBlackTrees() {
  {
    RedBlackTree<int> tree;
    int* ptr = tree.insert(1);
    assert(*ptr == 1);
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
    std::cout << "testRedBlackTrees 1 passed!\n";
  }

  {
    RedBlackTree<string> tree;
    tree.insert("aaa");
    assert(tree.inorder() == "aaa");
    tree.insert("bbb");
    assert(tree.inorder() == "aaa bbb");
    tree.insert("ccc");
    assert(tree.inorder() == "aaa bbb ccc");
    std::cout << "testRedBlackTrees 2 passed!\n";
  }
}

void testLOB() {
  {
    LOB book;
    Order order1(1U, Side::BUY, 50.5, 111, 10);
    book.place_order(order1);
  }

  {
    LOB lob;

    Order s1{1U, Side::SELL, 100.0, 1000ULL, 50U};
    lob.place_order(s1);

    Order s2{2U, Side::SELL, 100.0, 1001ULL, 20U};
    lob.place_order(s2);

    Order s3{3U, Side::SELL, 101.0, 1002ULL, 30U};
    lob.place_order(s3);

    Order b1{4U, Side::BUY, 99.0, 1003ULL, 100U};
    lob.place_order(b1);

    Order b2{5U, Side::BUY, 100.0, 1004ULL, 30U};
    lob.place_order(b2);

    Order b3{6U, Side::BUY, 102.0, 1005ULL, 80U};
    lob.place_order(b3);
  }

  {
    LOB lob;

    Order s1{1U, Side::SELL, 100.0, 1000ULL, 50U};
    lob.place_order(s1);

    Order s2{2U, Side::SELL, 100.0, 1001ULL, 20U};
    lob.place_order(s2);

    Order s3{3U, Side::SELL, 101.0, 1002ULL, 30U};
    lob.place_order(s3);

    lob.cancel_order(s1.order_id);
    lob.cancel_order(s2.order_id);
    lob.cancel_order(s3.order_id);
  }
}

int main() {
  testInt();
  testString();
  testSmallest();
  testRedBlackTrees();
  cout << "\ntestLOB  ==========================\n";
  testLOB();

  return 0;
}