#include <cassert>
#include <iostream>

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

void test_invariant() {
  {
    RedBlackTree<int> tree;

    tree.insert(17);
    tree.insert(18);
    tree.insert(33);
    tree.insert(34);
    tree.insert(46);
    tree.insert(50);
    tree.insert(53);
    tree.insert(72);
    tree.insert(74);
    tree.insert(80);
    tree.insert(88);

    /* 測試 root_ 是否為 black */
    assert(tree.root_->color == Color::BLACK);

    /* 測試沒有連續兩個 Red */
    auto it = tree.leftmost_node_;
    while (it) {
      if (it->color == Color::RED) {
        if (it->left) assert(it->left->color == Color::BLACK);
        if (it->right) assert(it->right->color == Color::BLACK);
      }
      it = tree.successor(it);
    }

    cout << "test_invariant passed!\n" << flush;
  }
}