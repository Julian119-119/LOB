#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <queue>
#include <random>
#include <sstream>
#include <vector>

#include "test.hpp"
#include "LOB_type.hpp"
using namespace std;



/* help function: 下訂單 */
inline static void add_buyer_order(LOB& lob, uint32_t idx, double price, uint64_t ts,
                     uint32_t vol, Time_In_Force tif = Time_In_Force::GTC) {
  Order new_order{idx, Side::BUY, price, ts, vol, tif};
  lob.place_order(new_order);
}

inline static void add_seller_order(LOB& lob, uint32_t idx, double price, uint64_t ts,
                      uint32_t vol, Time_In_Force tif = Time_In_Force::GTC) {
  Order new_order{idx, Side::SELL, price, ts, vol, tif};
  lob.place_order(new_order);
}

void testLOB() {
  cout << "\n===============  test lob ===============\n";

  /* 測試 get_best_bid_price */
  {
    LOB lob;

    assert(!is_valid_price(lob.get_best_bid_price()));

    add_buyer_order(lob, 1U, 100.0, 1000ULL, 50U);
    assert(lob.get_best_bid_price() == 100.0);

    add_buyer_order(lob, 2U, 110.0, 1002ULL, 60U);
    assert(lob.get_best_bid_price() == 110.0);

    add_buyer_order(lob, 3U, 100.0, 1003ULL, 10U);
    assert(lob.get_best_bid_price() == 110.0);

    add_buyer_order(lob, 4U, 90.0, 1004ULL, 5U);
    assert(lob.get_best_bid_price() == 110.0);

    add_buyer_order(lob, 5U, 85.0, 1005ULL, 1U);
    assert(lob.get_best_bid_price() == 110.0);

    add_buyer_order(lob, 6U, 115.0, 1006ULL, 70U);
    assert(lob.get_best_bid_price() == 115.0);

    cout << "get_best_bid_price passed!\n";
  }

  /* 測試下單與撮合 */
  {
    LOB lob;

    assert(lob.has_order(1U) == false);
    assert(!is_valid_volume(lob.get_volume_at_price(100, Side::BUY)));

    // ID 1，賣出 100 元，50 股 (掛單)
    Order s1{1U, Side::SELL, 100.0, 1000ULL, 50U};
    lob.place_order(s1);
    assert(lob.has_order(1U) == true);
    assert(lob.get_volume_at_price(100.0, Side::SELL) == 50);

    // ID 2，賣出 100 元，20 股（掛單）
    Order s2{2U, Side::SELL, 100.0, 1001ULL, 20U};
    lob.place_order(s2);
    assert(lob.has_order(2U) == true);
    assert(lob.get_volume_at_price(100.0, Side::SELL) == 70);

    // ID 3，賣出 101 元，30 股（掛單）
    Order s3{3U, Side::SELL, 101.0, 1002ULL, 30U};
    lob.place_order(s3);
    assert(lob.has_order(3U) == true);
    assert(lob.get_volume_at_price(101.0, Side::SELL) == 30);

    // ID 4，買入 99 元，100 股（掛單）
    Order b1{4U, Side::BUY, 99.0, 1003ULL, 100U};
    lob.place_order(b1);
    assert(lob.has_order(4U) == true);
    assert(lob.get_volume_at_price(99.0, Side::BUY) == 100);

    // ID 5，買入 100 元，30 股
    // ID 1 的成交 30 股，剩下 20 股
    // ID 5，不掛單
    Order b2{5U, Side::BUY, 100.0, 1004ULL, 30U};
    lob.place_order(b2);
    assert(lob.has_order(5U) == false);
    assert(lob.get_volume_at_price(100.0, Side::SELL) == 40);

    // ID 6, 買入 102 元，80 股
    // ID 1, ID 2, ID 3 全數結單
    // ID 6，剩下 10 股，掛單
    Order b3{6U, Side::BUY, 102.0, 1005ULL, 80U};
    lob.place_order(b3);
    assert(lob.has_order(6U) == true);
    assert(lob.has_order(1U) == false);
    assert(lob.has_order(2U) == false);
    assert(lob.has_order(3U) == false);
    assert(lob.get_volume_at_price(102.0, Side::BUY) == 10);

    cout << "place and match order passed!\n";
  }

  /* 測試取消訂單 */
  {
    LOB lob;
    vector<uint32_t> order_list;
    order_list.reserve(16);

    Order s1{1U, Side::SELL, 100.0, 1000ULL, 50U};
    lob.place_order(s1);
    order_list.push_back(s1.order_id);

    Order s2{2U, Side::SELL, 100.0, 1001ULL, 20U};
    lob.place_order(s2);
    order_list.push_back(s2.order_id);

    Order s3{3U, Side::SELL, 101.0, 1002ULL, 30U};
    lob.place_order(s3);
    order_list.push_back(s3.order_id);

    lob.cancel_order(4U);
    for (auto tar_order_idx : order_list) {
      lob.cancel_order(tar_order_idx);
      assert(lob.has_order(tar_order_idx) == false);
    }

    cout << "cancel order passed!\n";
  }

  /* 測試當 k < 掛單數時的 get_top_k_info */
  {
    LOB lob;
    vector<PriceLevelInfo> info_list;
    int test_num = 5;
    int price_level_num = 6;
    info_list.reserve(test_num);

    int test_number = 5;
    uint32_t curr_id = 1U;
    double curr_price = 100.0;
    uint64_t curr_timestamp = 1000ULL;
    uint32_t curr_volume = 10U;

    for (int i = 0; i < price_level_num; i++) {
      Order order(curr_id, Side::SELL, curr_price, curr_timestamp, curr_volume);
      lob.place_order(order);
      assert(lob.has_order(curr_id));

      if (i < test_num) {
        info_list.emplace_back(curr_price, curr_volume);
      }

      curr_id++;
      curr_price++;
      curr_timestamp++;
      curr_volume++;
    }

    vector<PriceLevelInfo> lob_top_k = lob.get_top_k_info(test_num, Side::SELL);
    for (int i = 0; i < info_list.size(); i++) {
      assert(info_list[i].price == lob_top_k[i].price);
      assert(info_list[i].volume == lob_top_k[i].volume);
    }

    cout << "when k < the number of order, get_top_k_info passed!\n";
  }

  /* 測試當 k < 掛單數時的 get_top_k_info */
  {
    LOB lob;
    vector<PriceLevelInfo> info_list;
    int test_num = 5;
    int price_level_num = 2;
    info_list.reserve(test_num);

    uint32_t curr_id = 1U;
    double curr_price = 100.0;
    uint64_t curr_timestamp = 1000ULL;
    uint32_t curr_volume = 10U;

    for (int i = 0; i < price_level_num; i++) {
      Order order(curr_id, Side::SELL, curr_price, curr_timestamp, curr_volume);
      lob.place_order(order);
      assert(lob.has_order(curr_id));

      if (i < test_num) {
        info_list.emplace_back(curr_price, curr_volume);
      }

      curr_id++;
      curr_price++;
      curr_timestamp++;
      curr_volume++;
    }

    vector<PriceLevelInfo> lob_top_k = lob.get_top_k_info(test_num, Side::SELL);
    for (int i = 0; i < info_list.size(); i++) {
      assert(info_list[i].price == lob_top_k[i].price);
      assert(info_list[i].volume == lob_top_k[i].volume);
    }
    cout << "when k > the number of order, get_top_k_info passed!\n";
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