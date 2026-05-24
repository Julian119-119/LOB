#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <queue>
#include <random>
#include <sstream>
#include <vector>

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

    lob.cancel_order(4U);
    lob.cancel_order(s1.order_id);
    lob.cancel_order(s2.order_id);
    lob.cancel_order(s3.order_id);
  }

  cout << "test lob passed!\n" << flush;
}

const double stock_price = 100.0;
const double tick_size = 0.5;
const long test_data_size = 1'000'000'00;
const int print_point_num = 30;

void testspend() {
  cout << "\n==========testspend==========\n" << flush;
  random_device rd;
  mt19937 gen(rd());

  // 設定分佈狀況
  // 假設 price 的 mean 為 100，sd 為 0.3
  normal_distribution<double> price_norm_dist{stock_price, 0.3};
  // 購買的量 （volume）
  uniform_int_distribution<uint64_t> volume_unif_dist{1, 1000};
  // 買賣方向
  uniform_int_distribution<int> side_code{0, 1};
  uint32_t user_id = 1;
  // timestamp
  uint64_t ts = 1779600968;
  // 隨機產生要放單或徹單，如果值大於 80 就徹單，否則放單
  uniform_int_distribution<int> place_or_cancel_dist{1, 100};
  // 在儲存 id 的 vector 中找尋要刪除的 id
  uniform_int_distribution<uint32_t> id_dist;

  // 模擬市場流動
  LOB book;
  vector<uint32_t> all_id;
  all_id.reserve(test_data_size);

  auto start_time = chrono::high_resolution_clock::now();

  for (long i = 0; i < test_data_size; i++) {
    if (all_id.empty() || place_or_cancel_dist(gen) <= 80) /* 放單 */ {
      uint32_t order_id = user_id++;
      double price = round(price_norm_dist(gen) / tick_size) * tick_size;
      Side side = side_code(gen) ? Side::BUY : Side::SELL;
      uint64_t timestamp = ts++;
      uint64_t volume = volume_unif_dist(gen);

      Order new_order(order_id, side, price, timestamp, volume);
      all_id.push_back(order_id);
      book.place_order(new_order);
    } else /* 徹單 */ {
      // 拿到隨機產生的 idx
      uint32_t idx =
          id_dist(gen, decltype(id_dist)::param_type(0, all_id.size() - 1));
      book.cancel_order(all_id[idx]);
      // 將最後一個元素往前塞
      all_id[idx] = all_id.back();
      all_id.pop_back();
    }
    if (i % (test_data_size / print_point_num) == 0) {
      cout << '.' << flush;
    }
  }

  auto end_time = chrono::high_resolution_clock::now();
  auto duration_time =
      chrono::duration_cast<chrono::nanoseconds>(end_time - start_time);
  cout << "\ntotal spending time: " << duration_time.count() / 1'000'000
       << "ms\n";
  cout << "average spending time: " << duration_time.count() / test_data_size
       << "ns\n";
}

int main() {
  testInt();
  testString();
  testSmallest();
  testRedBlackTrees();
  testLOB();
  testspend();

  return 0;
}