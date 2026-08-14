#include <algorithm>
#include <cassert>
#include <iostream>
#include <set>
#include <vector>

#include "RedBlackTree.hpp"
#include "test_helper.hpp"
using namespace std;

void test_insert_search() {
  /* 大小交錯插入 */
  {
    RedBlackTree<int> tree;

    assert(tree.find_node(50) == tree.end());  // 測試空的 find
    assert(tree.empty() == true);              // 測試 empty()

    auto int_ptr = tree.insert(7);
    assert(*tree.find_node(7) == 7);
    assert(*tree.begin() == 7);
    assert(*(--tree.end()) == 7);
    assert(tree.empty() == false);
    assert(*int_ptr == 7);  // 測試回傳值

    int_ptr = tree.insert(12);
    assert(*tree.find_node(12) == 12);
    assert(*tree.begin() == 7);
    assert(*(--tree.end()) == 12);
    assert(tree.empty() == false);
    assert(*int_ptr == 12);

    int_ptr = tree.insert(3);
    assert(*tree.find_node(3) == 3);
    assert(*tree.begin() == 3);
    assert(*(--tree.end()) == 12);
    assert(tree.empty() == false);
    assert(*int_ptr == 3);

    tree.insert(6);
    assert(*tree.find_node(6) == 6);
    assert(*tree.begin() == 3);
    assert(*(--tree.end()) == 12);

    tree.insert(11);
    assert(*tree.find_node(11) == 11);
    assert(*tree.begin() == 3);
    assert(*(--tree.end()) == 12);

    tree.insert(10);
    assert(*tree.find_node(10) == 10);
    assert(*tree.begin() == 3);
    assert(*(--tree.end()) == 12);

    tree.insert(1);
    assert(*tree.find_node(1) == 1);
    assert(*tree.begin() == 1);
    assert(*(--tree.end()) == 12);

    tree.insert(4);
    assert(*tree.find_node(4) == 4);
    assert(*tree.begin() == 1);
    assert(*(--tree.end()) == 12);

    tree.insert(4);  // 放入重複的數字
    auto it = tree.begin();
    int node_num = 0;
    while (it != tree.end()) {
      node_num++;
      ++it;
    }
    assert(node_num == 8);  // 測試重複的數字是否有被放進去
  }

  /* 由小到大插入 */
  {
    RedBlackTree<int> tree;
    auto int_ptr = tree.insert(10);
    assert(*tree.find_node(10) == 10);
    assert(*tree.begin() == 10);
    assert(*(--tree.end()) == 10);
    assert(tree.empty() == false);
    assert(*int_ptr == 10);

    int_ptr = tree.insert(23);
    assert(*tree.find_node(23) == 23);
    assert(*tree.begin() == 10);
    assert(*(--tree.end()) == 23);
    assert(tree.empty() == false);
    assert(*int_ptr == 23);

    int_ptr = tree.insert(35);
    assert(*tree.find_node(35) == 35);
    assert(*tree.begin() == 10);
    assert(*(--tree.end()) == 35);
    assert(tree.empty() == false);
    assert(*int_ptr == 35);

    tree.insert(42);
    assert(*tree.find_node(42) == 42);
    assert(*tree.begin() == 10);
    assert(*(--tree.end()) == 42);

    tree.insert(53);
    assert(*tree.find_node(53) == 53);
    assert(*tree.begin() == 10);
    assert(*(--tree.end()) == 53);

    tree.insert(64);
    assert(*tree.find_node(64) == 64);
    assert(*tree.begin() == 10);
    assert(*(--tree.end()) == 64);

    tree.insert(75);
    assert(*tree.find_node(75) == 75);
    assert(*tree.begin() == 10);
    assert(*(--tree.end()) == 75);

    tree.insert(86);
    assert(*tree.find_node(86) == 86);
    assert(*tree.begin() == 10);
    assert(*(--tree.end()) == 86);

    tree.insert(94);
    assert(*tree.find_node(94) == 94);
    assert(*tree.begin() == 10);
    assert(*(--tree.end()) == 94);

    tree.insert(103);
    assert(*tree.find_node(103) == 103);
    assert(*tree.begin() == 10);
    assert(*(--tree.end()) == 103);
  }

  /* 由大到小插入 */
  {
    RedBlackTree<int> tree;
    auto int_ptr = tree.insert(103);
    assert(*tree.find_node(103) == 103);
    assert(*tree.begin() == 103);
    assert(*(--tree.end()) == 103);
    assert(tree.empty() == false);
    assert(*int_ptr == 103);

    int_ptr = tree.insert(94);
    assert(*tree.find_node(94) == 94);
    assert(*tree.begin() == 94);
    assert(*(--tree.end()) == 103);
    assert(tree.empty() == false);
    assert(*int_ptr == 94);

    int_ptr = tree.insert(86);
    assert(*tree.find_node(86) == 86);
    assert(*tree.begin() == 86);
    assert(*(--tree.end()) == 103);
    assert(tree.empty() == false);
    assert(*int_ptr == 86);

    tree.insert(75);
    assert(*tree.find_node(75) == 75);
    assert(*tree.begin() == 75);
    assert(*(--tree.end()) == 103);

    tree.insert(64);
    assert(*tree.find_node(64) == 64);
    assert(*tree.begin() == 64);
    assert(*(--tree.end()) == 103);

    tree.insert(53);
    assert(*tree.find_node(53) == 53);
    assert(*tree.begin() == 53);
    assert(*(--tree.end()) == 103);

    tree.insert(42);
    assert(*tree.find_node(42) == 42);
    assert(*tree.begin() == 42);
    assert(*(--tree.end()) == 103);

    tree.insert(35);
    assert(*tree.find_node(35) == 35);
    assert(*tree.begin() == 35);
    assert(*(--tree.end()) == 103);

    tree.insert(23);
    assert(*tree.find_node(23) == 23);
    assert(*tree.begin() == 23);
    assert(*(--tree.end()) == 103);

    tree.insert(10);
    assert(*tree.find_node(10) == 10);
    assert(*tree.begin() == 10);
    assert(*(--tree.end()) == 103);
  }

  /* 測試放入 greater<int>() */
  {
    RedBlackTree<int, greater<int>> tree;

    assert(tree.find_node(50) == tree.end());  // 測試空的 find
    assert(tree.empty() == true);              // 測試 empty()

    auto int_ptr = tree.insert(7);
    assert(*tree.find_node(7) == 7);
    assert(*tree.begin() == 7);
    assert(*(--tree.end()) == 7);
    assert(tree.empty() == false);
    assert(*int_ptr == 7);

    int_ptr = tree.insert(3);
    assert(*tree.find_node(3) == 3);
    assert(*tree.begin() == 7);
    assert(*(--tree.end()) == 3);
    assert(tree.empty() == false);
    assert(*int_ptr == 3);

    int_ptr = tree.insert(6);
    assert(*tree.find_node(6) == 6);
    assert(*tree.begin() == 7);
    assert(*(--tree.end()) == 3);
    assert(tree.empty() == false);
    assert(*int_ptr == 6);

    tree.insert(11);
    assert(*tree.find_node(11) == 11);
    assert(*tree.begin() == 11);
    assert(*(--tree.end()) == 3);

    tree.insert(10);
    assert(*tree.find_node(10) == 10);
    assert(*tree.begin() == 11);
    assert(*(--tree.end()) == 3);

    tree.insert(12);
    assert(*tree.find_node(12) == 12);
    assert(*tree.begin() == 12);
    assert(*(--tree.end()) == 3);

    tree.insert(1);
    assert(*tree.find_node(1) == 1);
    assert(*tree.begin() == 12);
    assert(*(--tree.end()) == 1);

    tree.insert(4);
    assert(*tree.find_node(4) == 4);
    assert(*tree.begin() == 12);
    assert(*(--tree.end()) == 1);

    tree.insert(4);  // 測試重複的數字
    auto it = tree.begin();
    int node_num = 0;
    while (it != tree.end()) {
      node_num++;
      ++it;
    }
    assert(node_num == 8);  // 測試重複的數字是否有被放進去

    cout << "test_insert_search passed!\n" << flush;
  }
}

void test_remove() {
  /* 大小交錯刪除 */
  {
    RedBlackTree<int> tree;

    tree.remove(8);  // 測試空樹刪除

    tree.insert(7);
    tree.insert(12);
    tree.insert(3);
    tree.insert(6);
    tree.insert(11);
    tree.insert(10);
    tree.insert(1);
    tree.insert(4);
    assert(*tree.begin() == 1);
    assert(*(--tree.end()) == 12);

    tree.remove(165);  // 測試刪除不存在的數字
    assert(tree.find_node(165) == tree.end());
    assert(*tree.begin() == 1);
    assert(*(--tree.end()) == 12);

    tree.remove(6);  // 刪除中間數
    assert(tree.find_node(6) == tree.end());
    assert(*tree.begin() == 1);
    assert(*(--tree.end()) == 12);

    tree.remove(12);  // 刪除最大數字
    assert(tree.find_node(12) == tree.end());
    assert(*tree.begin() == 1);
    assert(*(--tree.end()) == 11);

    tree.remove(1);  // 刪除最小數字
    assert(tree.find_node(1) == tree.end());
    assert(*tree.begin() == 3);
    assert(*(--tree.end()) == 11);

    tree.remove(3);
    assert(tree.find_node(3) == tree.end());
    assert(*tree.begin() == 4);
    assert(*(--tree.end()) == 11);

    tree.remove(4);
    assert(tree.find_node(4) == tree.end());
    assert(*tree.begin() == 7);
    assert(*(--tree.end()) == 11);

    tree.remove(10);
    assert(tree.find_node(10) == tree.end());
    assert(*tree.begin() == 7);
    assert(*(--tree.end()) == 11);

    tree.remove(11);
    assert(tree.find_node(11) == tree.end());
    assert(*tree.begin() == 7);
    assert(*(--tree.end()) == 7);

    tree.remove(7);
    assert(tree.find_node(7) == tree.end());
    assert(tree.begin() == tree.end());
    assert(tree.empty() == true);  // 測試樹是否為空
  }

  /* 由小到大刪除 */
  {
    RedBlackTree<int> tree;
    tree.insert(7);
    tree.insert(12);
    tree.insert(3);
    tree.insert(6);
    tree.insert(11);
    tree.insert(10);
    tree.insert(1);
    tree.insert(4);
    assert(*tree.begin() == 1);
    assert(*(--tree.end()) == 12);

    tree.remove(1);
    assert(tree.find_node(1) == tree.end());
    assert(*tree.begin() == 3);
    assert(*(--tree.end()) == 12);

    tree.remove(3);
    assert(tree.find_node(3) == tree.end());
    assert(*tree.begin() == 4);
    assert(*(--tree.end()) == 12);

    tree.remove(4);
    assert(tree.find_node(4) == tree.end());
    assert(*tree.begin() == 6);
    assert(*(--tree.end()) == 12);

    tree.remove(6);
    assert(tree.find_node(6) == tree.end());
    assert(*tree.begin() == 7);
    assert(*(--tree.end()) == 12);

    tree.remove(7);
    assert(tree.find_node(7) == tree.end());
    assert(*tree.begin() == 10);
    assert(*(--tree.end()) == 12);

    tree.remove(10);
    assert(tree.find_node(10) == tree.end());
    assert(*tree.begin() == 11);
    assert(*(--tree.end()) == 12);

    tree.remove(11);
    assert(tree.find_node(11) == tree.end());
    assert(*tree.begin() == 12);
    assert(*(--tree.end()) == 12);

    tree.remove(12);
    assert(tree.find_node(12) == tree.end());
    assert(tree.begin() == tree.end());
    assert(tree.empty() == true);  // 測試樹是否為空
  }

  /* 由大到小刪除 */
  {
    RedBlackTree<int> tree;
    tree.insert(7);
    tree.insert(12);
    tree.insert(3);
    tree.insert(6);
    tree.insert(11);
    tree.insert(10);
    tree.insert(1);
    tree.insert(4);
    assert(*tree.begin() == 1);
    assert(*(--tree.end()) == 12);

    tree.remove(12);
    assert(tree.find_node(12) == tree.end());
    assert(*tree.begin() == 1);
    assert(*(--tree.end()) == 11);

    tree.remove(11);
    assert(tree.find_node(11) == tree.end());
    assert(*tree.begin() == 1);
    assert(*(--tree.end()) == 10);

    tree.remove(10);
    assert(tree.find_node(10) == tree.end());
    assert(*tree.begin() == 1);
    assert(*(--tree.end()) == 7);

    tree.remove(7);
    assert(tree.find_node(7) == tree.end());
    assert(*tree.begin() == 1);
    assert(*(--tree.end()) == 6);

    tree.remove(6);
    assert(tree.find_node(6) == tree.end());
    assert(*tree.begin() == 1);
    assert(*(--tree.end()) == 4);

    tree.remove(4);
    assert(tree.find_node(4) == tree.end());
    assert(*tree.begin() == 1);
    assert(*(--tree.end()) == 3);

    tree.remove(3);
    assert(tree.find_node(3) == tree.end());
    assert(*tree.begin() == 1);
    assert(*(--tree.end()) == 1);

    tree.remove(1);
    assert(tree.find_node(1) == tree.end());
    assert(tree.begin() == tree.end());
    assert(tree.empty() == true);  // 測試樹是否為空
  }

  /* 測試加入 Compare */
  {
    RedBlackTree<int, greater<int>> tree;

    tree.remove(5);  // 測試空樹刪除

    tree.insert(7);
    tree.insert(3);
    tree.insert(6);
    tree.insert(11);
    tree.insert(10);
    tree.insert(12);
    tree.insert(1);
    tree.insert(4);

    tree.remove(165);  // 測試刪除不存在的數字
    assert(tree.find_node(165) == tree.end());
    assert(*tree.begin() == 12);
    assert(*(--tree.end()) == 1);

    tree.remove(6);  // 刪除中間數
    assert(tree.find_node(6) == tree.end());
    assert(*tree.begin() == 12);
    assert(*(--tree.end()) == 1);

    tree.remove(12);  // 刪除最大數字
    assert(tree.find_node(12) == tree.end());
    assert(*tree.begin() == 11);
    assert(*(--tree.end()) == 1);

    tree.remove(1);  // 刪除最小數字
    assert(tree.find_node(1) == tree.end());
    assert(*tree.begin() == 11);
    assert(*(--tree.end()) == 3);

    tree.remove(11);
    assert(tree.find_node(11) == tree.end());
    assert(*tree.begin() == 10);
    assert(*(--tree.end()) == 3);

    tree.remove(4);
    assert(tree.find_node(4) == tree.end());
    assert(*tree.begin() == 10);
    assert(*(--tree.end()) == 3);

    tree.remove(3);
    assert(tree.find_node(3) == tree.end());
    assert(*tree.begin() == 10);
    assert(*(--tree.end()) == 7);

    tree.remove(7);
    assert(tree.find_node(7) == tree.end());
    assert(*tree.begin() == 10);
    assert(*(--tree.end()) == 10);

    tree.remove(10);
    assert(tree.find_node(10) == tree.end());
    assert(tree.begin() == tree.end());
    assert(tree.empty() == true);  // 測試樹是否為空

    cout << "test_remove passed!\n" << flush;
  }
}

void test_get_successor() {
  {
    RedBlackTree<int> tree;
    for (int i = 0; i < 100; i++) {
      tree.insert(i);
    }

    auto it = tree.begin();
    for (int i = 0; i < 100; i++) {
      assert(*it == i);
      ++it;
    }
    assert(it == tree.end());  // 測試全部 inorder 走完後，會回到 nullptr

    cout << "test_get_successor passed!\n" << flush;
  }
}

struct Price_level {
  size_t id;
  size_t price;

  Price_level(size_t id, size_t p) : id(id), price(p) {}
};

class Comp {
 public:
  bool operator()(const Price_level& a, const Price_level& b) {
    return a.price < b.price;
  }
  bool operator()(const size_t pe, const Price_level& pl) {
    return pe < pl.price;
  }
  bool operator()(const Price_level& pl, const size_t pe) {
    return pl.price < pe;
  }
};

void test_insert_emplace() {
  /* 比較 price ，且大小交錯插入 */
  {
    RedBlackTree<Price_level, Comp> tree;

    /* 插入 ID = 1111，price = 60 的 Price_level */
    auto it = tree.insert_emplace(60, 1111, 60);
    assert(it && it->id == 1111 && it->price == 60);

    // 測試異構查詢
    assert(tree.find_node(60)->id == 1111);
    assert(tree.begin()->id == 1111);
    assert((--tree.end())->id == 1111);

    /* 測試插入相同的 price */
    it = tree.insert_emplace(60, 2222, 60);
    assert(it && it->id == 1111);
    assert(tree.find_node(60)->id == 1111);
    assert(tree.begin()->id == 1111);
    assert((--tree.end())->id == 1111);

    it = tree.insert_emplace(50, 5, 50);
    assert(it && it->id == 5 && it->price == 50);
    assert(tree.find_node(50)->id == 5);
    assert(tree.begin()->id == 5);
    assert((--tree.end())->id == 1111);

    it = tree.insert_emplace(20, 222, 20);
    assert(it && it->id == 222 && it->price == 20);
    assert(tree.find_node(20)->id == 222);
    assert(tree.begin()->id == 222);
    assert((--tree.end())->id == 1111);

    it = tree.insert_emplace(40, 44, 40);
    assert(it && it->id == 44 && it->price == 40);
    assert(tree.find_node(40)->id == 44);
    assert(tree.begin()->id == 222);
    assert((--tree.end())->id == 1111);

    it = tree.insert_emplace(30, 333, 30);
    assert(it && it->id == 333 && it->price == 30);
    assert(tree.find_node(30)->id == 333);
    assert(tree.begin()->id == 222);
    assert((--tree.end())->id == 1111);
  }

  /* 比較 price 且小到大插入 */
  {
    RedBlackTree<Price_level, Comp> tree;

    auto it = tree.insert_emplace(20, 222, 20);
    assert(it && it->id == 222 && it->price == 20);
    assert(tree.find_node(20)->id == 222);
    assert(tree.begin()->id == 222);
    assert((--tree.end())->id == 222);

    it = tree.insert_emplace(30, 333, 30);
    assert(it && it->id == 333 && it->price == 30);
    assert(tree.find_node(30)->id == 333);
    assert(tree.begin()->id == 222);
    assert((--tree.end())->id == 333);

    it = tree.insert_emplace(40, 44, 40);
    assert(it && it->id == 44 && it->price == 40);
    assert(tree.find_node(40)->id == 44);
    assert(tree.begin()->id == 222);
    assert((--tree.end())->id == 44);

    it = tree.insert_emplace(50, 5, 50);
    assert(it && it->id == 5 && it->price == 50);
    assert(tree.find_node(50)->id == 5);
    assert(tree.begin()->id == 222);
    assert((--tree.end())->id == 5);

    it = tree.insert_emplace(60, 1111, 60);
    assert(it && it->id == 1111 && it->price == 60);
    assert(tree.find_node(60)->id == 1111);
    assert(tree.begin()->id == 222);
    assert((--tree.end())->id == 1111);
  }

  /* 比較 price 且由大到小插入*/
  {
    RedBlackTree<Price_level, Comp> tree;

    auto it = tree.insert_emplace(60, 1111, 60);
    assert(it && it->id == 1111 && it->price == 60);
    assert(tree.find_node(60)->id == 1111);
    assert(tree.begin()->id == 1111);
    assert((--tree.end())->id == 1111);

    it = tree.insert_emplace(50, 5, 50);
    assert(it && it->id == 5 && it->price == 50);
    assert(tree.find_node(50)->id == 5);
    assert(tree.begin()->id == 5);
    assert((--tree.end())->id == 1111);

    it = tree.insert_emplace(40, 44, 40);
    assert(it && it->id == 44 && it->price == 40);
    assert(tree.find_node(40)->id == 44);
    assert(tree.begin()->id == 44);
    assert((--tree.end())->id == 1111);

    it = tree.insert_emplace(30, 333, 30);
    assert(it && it->id == 333 && it->price == 30);
    assert(tree.find_node(30)->id == 333);
    assert(tree.begin()->id == 333);
    assert((--tree.end())->id == 1111);

    it = tree.insert_emplace(20, 222, 20);
    assert(it && it->id == 222 && it->price == 20);
    assert(tree.find_node(20)->id == 222);
    assert(tree.begin()->id == 222);
    assert((--tree.end())->id == 1111);

    cout << "test_insert_emplace passed!\n" << flush;
  }
}

void test_heterogeneous_remove() {
  /* 大小交錯刪除 */
  {
    RedBlackTree<Price_level, Comp> tree;

    tree.insert_emplace(60, 1111, 60);
    tree.insert_emplace(50, 5, 50);
    tree.insert_emplace(20, 222, 20);
    tree.insert_emplace(40, 44, 40);
    tree.insert_emplace(30, 333, 30);

    tree.remove(30);
    assert(tree.find_node(30) == tree.end());
    assert(tree.begin()->id == 222);
    assert((--tree.end())->id == 1111);

    tree.remove(50);
    assert(tree.find_node(50) == tree.end());
    assert(tree.begin()->id == 222);
    assert((--tree.end())->id == 1111);

    tree.remove(60);
    assert(tree.find_node(60) == tree.end());
    assert(tree.begin()->id == 222);
    assert((--tree.end())->id == 44);

    tree.remove(40);
    assert(tree.find_node(40) == tree.end());
    assert(tree.begin()->id == 222);
    assert((--tree.end())->id == 222);

    tree.remove(20);
    assert(tree.find_node(20) == tree.end());
    assert(tree.begin() == tree.end());

    assert(tree.empty());
  }

  /* 由小到大刪除 */
  {
    RedBlackTree<Price_level, Comp> tree;

    tree.insert_emplace(60, 1111, 60);
    tree.insert_emplace(50, 5, 50);
    tree.insert_emplace(20, 222, 20);
    tree.insert_emplace(40, 44, 40);
    tree.insert_emplace(30, 333, 30);

    tree.remove(20);
    assert(tree.find_node(20) == tree.end());
    assert(tree.begin()->id == 333);
    assert((--tree.end())->id == 1111);

    tree.remove(30);
    assert(tree.find_node(30) == tree.end());
    assert(tree.begin()->id == 44);
    assert((--tree.end())->id == 1111);

    tree.remove(40);
    assert(tree.find_node(40) == tree.end());
    assert(tree.begin()->id == 5);
    assert((--tree.end())->id == 1111);

    tree.remove(50);
    assert(tree.find_node(50) == tree.end());
    assert(tree.begin()->id == 1111);
    assert((--tree.end())->id == 1111);

    tree.remove(60);
    assert(tree.find_node(60) == tree.end());
    assert(tree.begin() == tree.end());

    assert(tree.empty());
  }

  /* 由大到小刪除*/
  {
    RedBlackTree<Price_level, Comp> tree;

    tree.insert_emplace(60, 1111, 60);
    tree.insert_emplace(50, 5, 50);
    tree.insert_emplace(20, 222, 20);
    tree.insert_emplace(40, 44, 40);
    tree.insert_emplace(30, 333, 30);

    tree.remove(60);
    assert(tree.find_node(60) == tree.end());
    assert(tree.begin()->id == 222);
    assert((--tree.end())->id == 5);

    tree.remove(50);
    assert(tree.find_node(50) == tree.end());
    assert(tree.begin()->id == 222);
    assert((--tree.end())->id == 44);

    tree.remove(40);
    assert(tree.find_node(40) == tree.end());
    assert(tree.begin()->id == 222);
    assert((--tree.end())->id == 333);

    tree.remove(30);
    assert(tree.find_node(30) == tree.end());
    assert(tree.begin()->id == 222);
    assert((--tree.end())->id == 222);

    tree.remove(20);
    assert(tree.find_node(20) == tree.end());
    assert(tree.begin() == tree.end());

    assert(tree.empty());

    cout << "test_heterogeneous_remove passed\n" << flush;
  }
}

/* 遞迴確認 black height 與是否有連續兩個 RED */
int dfs_test_invariant(RedBlackTree<int>::Node* node) {
  if (!node) return 1;

  int left_bh = dfs_test_invariant(node->left.get());
  if (left_bh == -1) return -1;
  int right_bh = dfs_test_invariant(node->right.get());
  if (right_bh == -1) return -1;

  if (left_bh == right_bh) {
    /* 如果是黑色，則將 black height + 1 後向上傳 */
    if (RedBlackTree<int>::getColor(node) == Color::BLACK) {
      return left_bh + 1;
    } else /* 如果為紅色，則確認沒有連續兩個紅色 */ {
      if (RedBlackTree<int>::getColor(node->left.get()) == Color::RED)
        return -1;
      if (RedBlackTree<int>::getColor(node->right.get()) == Color::RED)
        return -1;
      return left_bh;
    }
  } else {
    return -1;
  }
}

void test_invariant() {
  {
    vector<int> insert_vec = {40, 18, 47, 88, 14, 84, 56, 17, 81, 100,
                              16, 7,  37, 22, 19, 15, 57, 97, 3,  27,
                              10, 55, 23, 74, 51, 71, 1,  46, 82, 77};
    RedBlackTree<int> my_rbt;
    for (int num : insert_vec) {
      my_rbt.insert(num);
      assert(RedBlackTree<int>::getColor(my_rbt.root_.get()) == Color::BLACK);
      assert(dfs_test_invariant(my_rbt.root_.get()) != -1);
    }

    vector<int> remove_vec = {19, 23, 27, 22, 47, 15, 3,  55, 7,  100,
                              14, 97, 84, 16, 46, 56, 82, 40, 17, 37,
                              18, 1,  81, 71, 57, 51, 77, 74, 10, 88};
    for (int num : remove_vec) {
      my_rbt.remove(num);
      assert(RedBlackTree<int>::getColor(my_rbt.root_.get()) == Color::BLACK);
      assert(dfs_test_invariant(my_rbt.root_.get()) != -1);
    }

    cout << "test_invariant passed!\n" << flush;
  }
}

void fuzz_test() {
  for (int times = 0; times < 10; times++) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> random_insertion(0, 1);
    uniform_int_distribution<> random_nums(0, 100000);
    vector<int> nums_in_rbt;
    RedBlackTree<int> my_rbt;
    set<int> other_rbt;

    nums_in_rbt.reserve(64);
    for (int i = 0; i < 1000; i++) {
      int isInsertion = random_insertion(gen);
      if (nums_in_rbt.empty() || isInsertion) {
        int num = random_nums(gen);

        auto [it, inserted] = other_rbt.insert(num);
        if (inserted) nums_in_rbt.push_back(num);
        my_rbt.insert(num);

        assert(equal(other_rbt.begin(), other_rbt.end(), my_rbt.begin(),
                     my_rbt.end()));
        assert(RedBlackTree<int>::getColor(my_rbt.root_.get()) == Color::BLACK);
        assert(dfs_test_invariant(my_rbt.root_.get()) != -1);
      } else {
        int idx = random_nums(gen) % nums_in_rbt.size();
        int num = nums_in_rbt[idx];
        nums_in_rbt[idx] = nums_in_rbt.back();
        nums_in_rbt.pop_back();

        other_rbt.erase(num);
        my_rbt.remove(num);

        assert(equal(other_rbt.begin(), other_rbt.end(), my_rbt.begin(),
                     my_rbt.end()));
        assert(RedBlackTree<int>::getColor(my_rbt.root_.get()) == Color::BLACK);
        assert(dfs_test_invariant(my_rbt.root_.get()) != -1);
      }
    }
  }

  cout << "fuzz_test passed!\n" << flush;
}