#include <iostream>
#include <vector>
#include <cassert>

#include "LOB_type.hpp"
#include "test_helper.hpp"

/* help function: 下訂單 */
inline static void add_buyer_order(LOB& lob, uint32_t idx, double price,
                                   uint64_t ts, uint32_t vol,
                                   Time_In_Force tif = Time_In_Force::GTC) {
  Order new_order{idx, Side::BUY, price, ts, vol, tif};
  lob.place_order(new_order);
}
inline static void add_seller_order(LOB& lob, uint32_t idx, double price,
                                    uint64_t ts, uint32_t vol,
                                    Time_In_Force tif = Time_In_Force::GTC) {
  Order new_order{idx, Side::SELL, price, ts, vol, tif};
  lob.place_order(new_order);
}

void test_query() {
  /* 測試 get_best_bid_price 與 get_best_bid_volume */
  {
    LOB lob;

    assert(!is_valid_price(lob.get_best_bid_price()));
    assert(!is_valid_volume(lob.get_best_bid_volume()));

    add_buyer_order(lob, 1U, 100.0, 1000ULL, 50U);
    assert(lob.get_best_bid_price() == 100.0);
    assert(lob.get_best_bid_volume() == 50U);

    add_buyer_order(lob, 2U, 110.0, 1002ULL, 60U);
    assert(lob.get_best_bid_price() == 110.0);
    assert(lob.get_best_bid_volume() == 60U);

    add_buyer_order(lob, 3U, 100.0, 1003ULL, 10U);
    assert(lob.get_best_bid_price() == 110.0);
    assert(lob.get_best_bid_volume() == 60U);

    add_buyer_order(lob, 4U, 90.0, 1004ULL, 5U);
    assert(lob.get_best_bid_price() == 110.0);
    assert(lob.get_best_bid_volume() == 60U);

    add_buyer_order(lob, 5U, 85.0, 1005ULL, 1U);
    assert(lob.get_best_bid_price() == 110.0);
    assert(lob.get_best_bid_volume() == 60U);

    add_buyer_order(lob, 6U, 115.0, 1006ULL, 70U);
    assert(lob.get_best_bid_price() == 115.0);
    assert(lob.get_best_bid_volume() == 70U);

    add_buyer_order(lob, 7U, 115.0, 1007ULL, 80U);
    assert(lob.get_best_bid_volume() == 150U);

    std::cout << "get_best_bid_price passed!\n";
  }

  /* 測試 get_best_ask_price 和 get_best_ask_volume */
  {
    LOB lob;

    assert(!is_valid_price(lob.get_best_ask_price()));
    assert(!is_valid_volume(lob.get_best_ask_volume()));

    add_seller_order(lob, 1U, 100.0, 1000ULL, 50U);
    assert(lob.get_best_ask_price() == 100.0);
    assert(lob.get_best_ask_volume() == 50U);

    add_seller_order(lob, 2U, 110.0, 1002ULL, 60U);
    assert(lob.get_best_ask_price() == 100.0);
    assert(lob.get_best_ask_volume() == 50U);

    add_seller_order(lob, 3U, 100.0, 1003ULL, 10U);
    assert(lob.get_best_ask_price() == 100.0);
    assert(lob.get_best_ask_volume() == 60U);

    add_seller_order(lob, 4U, 90.0, 1004ULL, 5U);
    assert(lob.get_best_ask_price() == 90.0);
    assert(lob.get_best_ask_volume() == 5U);

    add_seller_order(lob, 5U, 85.0, 1005ULL, 1U);
    assert(lob.get_best_ask_price() == 85.0);
    assert(lob.get_best_ask_volume() == 1U);

    add_seller_order(lob, 6U, 115.0, 1006ULL, 70U);
    assert(lob.get_best_ask_price() == 85.0);
    assert(lob.get_best_ask_volume() == 1U);

    std::cout << "get_best_ask_price passed!\n";
  }

  /* 測試 get_mid_price 和 get_spread */
  {
    LOB lob;

    assert(!is_valid_price(lob.get_mid_price()));
    assert(!is_valid_price(lob.get_spread()));

    add_buyer_order(lob, 1U, 100.0, 1000ULL, 10U);
    assert(!is_valid_price(lob.get_mid_price()));
    assert(!is_valid_price(lob.get_spread()));

    add_seller_order(lob, 2U, 110.0, 1001ULL, 5U);
    assert(lob.get_mid_price() == 105.0);
    assert(lob.get_spread() == 10.0);

    add_buyer_order(lob, 3U, 90.0, 1002ULL, 20U);
    assert(lob.get_mid_price() == 105.0);
    assert(lob.get_spread() == 10.0);

    add_seller_order(lob, 4U, 100.0, 1003ULL, 10U);
    assert(lob.get_mid_price() == 100.0);
    assert(lob.get_spread() == 20.0);

    add_buyer_order(lob, 5U, 110.0, 1004ULL, 1U);
    assert(lob.get_mid_price() == 100.0);
    assert(lob.get_spread() == 20.0);

    add_buyer_order(lob, 6U, 110.0, 1005ULL, 4U);
    assert(!is_valid_price(lob.get_mid_price()));
    assert(!is_valid_price(lob.get_spread()));
  }

  /* 測試 seller side 的 get_top_k_info */
  /* 測試當 k < 掛單數時的 get_top_k_info */
  {
    LOB lob;
    std::vector<PriceLevelInfo> info_list;
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

    std::vector<PriceLevelInfo> lob_top_k =
        lob.get_top_k_info(test_num, Side::SELL);
    for (int i = 0; i < info_list.size(); i++) {
      assert(info_list[i].price == lob_top_k[i].price);
      assert(info_list[i].volume == lob_top_k[i].volume);
    }
  }

  /* 測試當 k < 掛單數時的 get_top_k_info */
  {
    LOB lob;
    std::vector<PriceLevelInfo> info_list;
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

    std::vector<PriceLevelInfo> lob_top_k =
        lob.get_top_k_info(test_num, Side::SELL);
    for (int i = 0; i < info_list.size(); i++) {
      assert(info_list[i].price == lob_top_k[i].price);
      assert(info_list[i].volume == lob_top_k[i].volume);
    }
  }

  /* 測試 buy side 的 get_top_k_info */
  /* 測試當 k < 掛單數時的 get_top_k_info */
  {
    LOB lob;
    std::vector<PriceLevelInfo> info_list;
    int test_num = 5;
    int price_level_num = 6;
    info_list.reserve(test_num);

    int test_number = 5;
    uint32_t curr_id = 1U;
    double curr_price = 100.0;
    uint64_t curr_timestamp = 1000ULL;
    uint32_t curr_volume = 10U;

    for (int i = 0; i < price_level_num; i++) {
      Order order(curr_id, Side::BUY, curr_price, curr_timestamp, curr_volume);
      lob.place_order(order);
      assert(lob.has_order(curr_id));

      if (i < test_num) {
        info_list.emplace_back(curr_price, curr_volume);
      }

      curr_id++;
      curr_price--;
      curr_timestamp++;
      curr_volume++;
    }

    std::vector<PriceLevelInfo> lob_top_k =
        lob.get_top_k_info(test_num, Side::BUY);
    for (int i = 0; i < info_list.size(); i++) {
      assert(info_list[i].price == lob_top_k[i].price);
      assert(info_list[i].volume == lob_top_k[i].volume);
    }

    /* 測試當 k < 掛單數時的 get_top_k_info */
    {
      LOB lob;
      std::vector<PriceLevelInfo> info_list;
      int test_num = 5;
      int price_level_num = 2;
      info_list.reserve(test_num);

      uint32_t curr_id = 1U;
      double curr_price = 100.0;
      uint64_t curr_timestamp = 1000ULL;
      uint32_t curr_volume = 10U;

      for (int i = 0; i < price_level_num; i++) {
        Order order(curr_id, Side::BUY, curr_price, curr_timestamp,
                    curr_volume);
        lob.place_order(order);
        assert(lob.has_order(curr_id));

        if (i < test_num) {
          info_list.emplace_back(curr_price, curr_volume);
        }

        curr_id++;
        curr_price--;
        curr_timestamp++;
        curr_volume++;
      }

      std::vector<PriceLevelInfo> lob_top_k =
          lob.get_top_k_info(test_num, Side::BUY);
      for (int i = 0; i < info_list.size(); i++) {
        assert(info_list[i].price == lob_top_k[i].price);
        assert(info_list[i].volume == lob_top_k[i].volume);
      }
      std::cout << "get_top_k_info passed!\n";
    }
  }

  std::cout << "test query passed!\n" << std::flush;
}