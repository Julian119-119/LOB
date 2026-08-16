#include <cassert>
#include <iostream>

#include "LOB_L3_type.hpp"
#include "test_helper.hpp"

/* help function: 下訂單 */
inline static void add_buyer_order(L3_LOB& lob, uint32_t idx, double price,
                                   uint64_t ts, uint32_t vol,
                                   Time_In_Force tif = Time_In_Force::GTC) {
  Order new_order{idx, Side::BUY, price, ts, vol, tif};
  lob.place_order(new_order);
}
inline static void add_seller_order(L3_LOB& lob, uint32_t idx, double price,
                                    uint64_t ts, uint32_t vol,
                                    Time_In_Force tif = Time_In_Force::GTC) {
  Order new_order{idx, Side::SELL, price, ts, vol, tif};
  lob.place_order(new_order);
}

void test_order() {
  /* 測試 GTC 的下單與撮合 */
  {
    L3_LOB lob;

    assert(lob.has_order(1U) == false);
    assert(!is_valid_volume(lob.get_volume_at_price(100, Side::BUY)));

    // ID 1，賣出 100 元，50 股 (掛單)
    add_seller_order(lob, 1U, 100.0, 1000ULL, 50U);
    assert(lob.has_order(1U) == true);
    assert(lob.get_volume_at_price(100.0, Side::SELL) == 50);

    // ID 2，賣出 100 元，20 股（掛單）
    add_seller_order(lob, 2U, 100.0, 1001ULL, 20U);
    assert(lob.has_order(2U) == true);
    assert(lob.get_volume_at_price(100.0, Side::SELL) == 70);

    // ID 3，賣出 101 元，30 股（掛單）
    add_seller_order(lob, 3U, 101.0, 1002ULL, 30U);
    assert(lob.has_order(3U) == true);
    assert(lob.get_volume_at_price(101.0, Side::SELL) == 30);

    // ID 4，買入 99 元，100 股（掛單）
    add_buyer_order(lob, 4U, 99.0, 1003ULL, 100U);
    assert(lob.has_order(4U) == true);
    assert(lob.get_volume_at_price(99.0, Side::BUY) == 100);

    // ID 5，買入 100 元，30 股
    // ID 1 的成交 30 股，剩下 20 股
    // ID 5，不掛單
    add_buyer_order(lob, 5U, 100.0, 1004ULL, 30U);
    assert(lob.has_order(5U) == false);
    assert(lob.get_volume_at_price(100.0, Side::SELL) == 40);

    // ID 6, 買入 102 元，80 股
    // ID 1, ID 2, ID 3 全數結單
    // ID 6，剩下 10 股，掛單
    add_buyer_order(lob, 6U, 102.0, 1005ULL, 80U);
    assert(lob.has_order(6U) == true);
    assert(lob.has_order(1U) == false);
    assert(lob.has_order(2U) == false);
    assert(lob.has_order(3U) == false);
    assert(lob.get_volume_at_price(102.0, Side::BUY) == 10);

    std::cout << "place GTC order passed!\n";
  }

  /* 測試買方 IOC 的下單與撮合 */
  {
    L3_LOB lob;

    // 測試 lob 為空
    add_buyer_order(lob, 1U, 80.0, 999ULL, 80U, Time_In_Force::IOC);
    assert(lob.has_order(1U) == false);

    // 測試相同價格，同一訂單上部份成交
    add_seller_order(lob, 2U, 100.0, 1000ULL, 20U);
    add_buyer_order(lob, 3U, 100.0, 1001ULL, 30U, Time_In_Force::IOC);
    assert(lob.has_order(3U) == false);
    assert(lob.has_order(2U) == false);

    // 測試相同價格，不同訂單上部份成交
    add_seller_order(lob, 4U, 100.0, 1002ULL, 40U);
    add_seller_order(lob, 5U, 100.0, 1003ULL, 60U);
    add_buyer_order(lob, 6U, 100.0, 1004ULL, 110U, Time_In_Force::IOC);
    assert(lob.has_order(4U) == false);
    assert(lob.has_order(5U) == false);
    assert(lob.has_order(6U) == false);

    // 測試不同價格，同一訂單上部份成交
    add_seller_order(lob, 7U, 100.0, 1005ULL, 80U);
    add_buyer_order(lob, 8U, 110.0, 1006ULL, 90U, Time_In_Force::IOC);
    assert(lob.has_order(7U) == false);
    assert(lob.has_order(8U) == false);

    // 測試不同價格，不同訂單上部份成交
    add_seller_order(lob, 9U, 100.0, 1007ULL, 80U);
    add_seller_order(lob, 10U, 110.0, 1008ULL, 60U);
    add_buyer_order(lob, 11U, 120.0, 1009ULL, 150U, Time_In_Force::IOC);
    assert(lob.has_order(9U) == false);
    assert(lob.has_order(10U) == false);
    assert(lob.has_order(11U) == false);

    // 測試相同價格，同一訂單上全部成交
    add_seller_order(lob, 12U, 100.0, 1010ULL, 50U);
    add_buyer_order(lob, 13U, 100.0, 1011ULL, 50U, Time_In_Force::IOC);
    assert(lob.has_order(12U) == false);
    assert(lob.has_order(13U) == false);

    // 測試相同價格，不同訂單上全部成交
    add_seller_order(lob, 14U, 100.0, 1012ULL, 60U);
    add_seller_order(lob, 15U, 100.0, 1013ULL, 40U);
    add_buyer_order(lob, 16U, 100.0, 1014ULL, 100U, Time_In_Force::IOC);
    assert(lob.has_order(14U) == false);
    assert(lob.has_order(15U) == false);
    assert(lob.has_order(16U) == false);

    // 測試不同價格，同一訂單上全部成交
    add_seller_order(lob, 17U, 100.0, 1015ULL, 100U);
    add_buyer_order(lob, 18U, 110.0, 1016ULL, 100U, Time_In_Force::IOC);
    assert(lob.has_order(17U) == false);
    assert(lob.has_order(18U) == false);

    // 測試不同價格，不同訂單上全部成交
    add_seller_order(lob, 19U, 100.0, 1017ULL, 30U);
    add_seller_order(lob, 20U, 110.0, 1018ULL, 70U);
    add_buyer_order(lob, 21U, 120.0, 1019ULL, 100U, Time_In_Force::IOC);
    assert(lob.has_order(19U) == false);
    assert(lob.has_order(20U) == false);
    assert(lob.has_order(21U) == false);
  }

  /* 測試賣方 IOC 的下單與撮合 */
  {
    L3_LOB lob;

    // 測試 lob 為空
    add_buyer_order(lob, 1U, 80.0, 999ULL, 80U, Time_In_Force::IOC);
    assert(lob.has_order(1U) == false);

    // 測試相同價格，同一訂單上部份成交
    add_buyer_order(lob, 2U, 100.0, 1000ULL, 20U);
    add_seller_order(lob, 3U, 100.0, 1001ULL, 30U, Time_In_Force::IOC);
    assert(lob.has_order(3U) == false);
    assert(lob.has_order(2U) == false);

    // 測試相同價格，不同訂單上部份成交
    add_buyer_order(lob, 4U, 100.0, 1002ULL, 40U);
    add_buyer_order(lob, 5U, 100.0, 1003ULL, 60U);
    add_seller_order(lob, 6U, 100.0, 1004ULL, 110U, Time_In_Force::IOC);
    assert(lob.has_order(4U) == false);
    assert(lob.has_order(5U) == false);
    assert(lob.has_order(6U) == false);

    // 測試不同價格，同一訂單上部份成交
    add_buyer_order(lob, 7U, 100.0, 1005ULL, 80U);
    add_seller_order(lob, 8U, 90.0, 1006ULL, 90U, Time_In_Force::IOC);
    assert(lob.has_order(7U) == false);
    assert(lob.has_order(8U) == false);

    // 測試不同價格，不同訂單上部份成交
    add_buyer_order(lob, 9U, 100.0, 1007ULL, 80U);
    add_buyer_order(lob, 10U, 110.0, 1008ULL, 60U);
    add_seller_order(lob, 11U, 90.0, 1009ULL, 150U, Time_In_Force::IOC);
    assert(lob.has_order(9U) == false);
    assert(lob.has_order(10U) == false);
    assert(lob.has_order(11U) == false);

    // 測試相同價格，同一訂單上全部成交
    add_buyer_order(lob, 12U, 100.0, 1010ULL, 50U);
    add_seller_order(lob, 13U, 100.0, 1011ULL, 50U, Time_In_Force::IOC);
    assert(lob.has_order(12U) == false);
    assert(lob.has_order(13U) == false);

    // 測試相同價格，不同訂單上全部成交
    add_buyer_order(lob, 14U, 100.0, 1012ULL, 60U);
    add_buyer_order(lob, 15U, 100.0, 1013ULL, 40U);
    add_seller_order(lob, 16U, 100.0, 1014ULL, 100U, Time_In_Force::IOC);
    assert(lob.has_order(14U) == false);
    assert(lob.has_order(15U) == false);
    assert(lob.has_order(16U) == false);

    // 測試不同價格，同一訂單上全部成交
    add_buyer_order(lob, 17U, 100.0, 1015ULL, 100U);
    add_seller_order(lob, 18U, 90.0, 1016ULL, 100U, Time_In_Force::IOC);
    assert(lob.has_order(17U) == false);
    assert(lob.has_order(18U) == false);

    // 測試不同價格，不同訂單上全部成交
    add_buyer_order(lob, 19U, 100.0, 1017ULL, 30U);
    add_buyer_order(lob, 20U, 110.0, 1018ULL, 70U);
    add_seller_order(lob, 21U, 90.0, 1019ULL, 100U, Time_In_Force::IOC);
    assert(lob.has_order(19U) == false);
    assert(lob.has_order(20U) == false);
    assert(lob.has_order(21U) == false);

    std::cout << "place IOC order passed!\n";
  }

  /* 測試買方 FOK 的下單與撮合 */
  {
    L3_LOB lob;

    // 測試 lob 為空
    add_buyer_order(lob, 1U, 80.0, 999ULL, 80U, Time_In_Force::FOK);
    assert(lob.has_order(1U) == false);

    // 測試相同價格下，掛單數量小於訂單數量
    add_seller_order(lob, 2U, 100.0, 1000ULL, 20U);
    add_buyer_order(lob, 3U, 100.0, 1001ULL, 30U, Time_In_Force::FOK);
    assert(lob.has_order(2U) == true);
    assert(lob.get_volume_at_price(100.0, Side::SELL) == 20U);
    assert(lob.has_order(3U) == false);
    // 清空 lob
    add_buyer_order(lob, 3U, 100.0, 1001ULL, 20U);
    assert(is_valid_volume(lob.get_volume_at_price(100.0, Side::SELL)) ==
           false);

    // 測試相同價格下，掛單數量小於訂單數量
    add_seller_order(lob, 4U, 100.0, 1002ULL, 40U);
    add_seller_order(lob, 5U, 100.0, 1003ULL, 60U);
    add_buyer_order(lob, 6U, 100.0, 1004ULL, 110U, Time_In_Force::FOK);
    assert(lob.has_order(4U) == true);
    assert(lob.has_order(5U) == true);
    assert(lob.get_volume_at_price(100, Side::SELL) == 100U);
    assert(lob.has_order(6U) == false);
    // 清空 lob
    add_buyer_order(lob, 6U, 100.0, 1004ULL, 100U);
    assert(is_valid_volume(lob.get_volume_at_price(100.0, Side::SELL)) ==
           false);

    // 測試不同價格下，掛單數量小於訂單數量
    add_seller_order(lob, 7U, 100.0, 1005ULL, 80U);
    add_buyer_order(lob, 8U, 110.0, 1006ULL, 90U, Time_In_Force::FOK);
    assert(lob.has_order(7U) == true);
    assert(lob.get_volume_at_price(100.0, Side::SELL) == 80U);
    assert(lob.has_order(8U) == false);
    // 清空 lob
    add_buyer_order(lob, 8U, 100.0, 1006ULL, 80U);
    assert(is_valid_volume(lob.get_volume_at_price(100.0, Side::SELL)) ==
           false);

    // 測試不同價格下，掛單數量小於訂單數量
    add_seller_order(lob, 9U, 100.0, 1007ULL, 80U);
    add_seller_order(lob, 10U, 110.0, 1008ULL, 60U);
    add_buyer_order(lob, 11U, 120.0, 1009ULL, 150U, Time_In_Force::FOK);
    assert(lob.has_order(9U) == true);
    assert(lob.get_volume_at_price(100.0, Side::SELL) == 80U);
    assert(lob.get_volume_at_price(110.0, Side::SELL) == 60U);
    assert(lob.has_order(10U) == true);
    assert(lob.has_order(11U) == false);
    // 清空 lob
    add_buyer_order(lob, 11U, 110.0, 1009ULL, 140U);
    assert(is_valid_volume(lob.get_volume_at_price(100.0, Side::SELL)) ==
           false);
    assert(is_valid_volume(lob.get_volume_at_price(110.0, Side::SELL)) ==
           false);

    // 測試相同價格下，同一訂單上全部成交
    add_seller_order(lob, 12U, 100.0, 1010ULL, 50U);
    add_buyer_order(lob, 13U, 100.0, 1011ULL, 50U, Time_In_Force::FOK);
    assert(lob.has_order(12U) == false);
    assert(is_valid_volume(lob.get_volume_at_price(100.0, Side::SELL)) ==
           false);
    assert(lob.has_order(13U) == false);

    // 測試相同價格下，不同訂單上全部成交
    add_seller_order(lob, 14U, 100.0, 1012ULL, 60U);
    add_seller_order(lob, 15U, 100.0, 1013ULL, 40U);
    add_buyer_order(lob, 16U, 100.0, 1014ULL, 100U, Time_In_Force::FOK);
    assert(lob.has_order(14U) == false);
    assert(is_valid_volume(lob.get_volume_at_price(100.0, Side::SELL)) ==
           false);
    assert(lob.has_order(15U) == false);
    assert(lob.has_order(16U) == false);

    // 測試不同價格下，同一訂單上全部成交
    add_seller_order(lob, 17U, 100.0, 1015ULL, 100U);
    add_buyer_order(lob, 18U, 110.0, 1016ULL, 100U, Time_In_Force::FOK);
    assert(lob.has_order(17U) == false);
    assert(is_valid_volume(lob.get_volume_at_price(100.0, Side::SELL)) ==
           false);
    assert(lob.has_order(18U) == false);

    // 測試不同價格下，不同訂單上全部成交
    add_seller_order(lob, 19U, 100.0, 1017ULL, 30U);
    add_seller_order(lob, 20U, 110.0, 1018ULL, 70U);
    add_buyer_order(lob, 21U, 120.0, 1019ULL, 100U, Time_In_Force::FOK);
    assert(lob.has_order(19U) == false);
    assert(is_valid_volume(lob.get_volume_at_price(100.0, Side::SELL)) ==
           false);
    assert(lob.has_order(20U) == false);
    assert(is_valid_volume(lob.get_volume_at_price(110.0, Side::SELL)) ==
           false);
    assert(lob.has_order(21U) == false);
  }

  /* 測試賣方 FOK 的賣單與撮合 */
  {
    L3_LOB lob;

    // 測試 lob 為空
    add_seller_order(lob, 1U, 80.0, 999ULL, 80U, Time_In_Force::FOK);
    assert(lob.has_order(1U) == false);

    // 測試相同價格下，掛單數量小於訂單數量
    add_buyer_order(lob, 2U, 100.0, 1000ULL, 20U);
    add_seller_order(lob, 3U, 100.0, 1001ULL, 30U, Time_In_Force::FOK);
    assert(lob.has_order(2U) == true);
    assert(lob.get_volume_at_price(100.0, Side::BUY) == 20U);
    assert(lob.has_order(3U) == false);
    // 清空 lob
    add_seller_order(lob, 3U, 100.0, 1001ULL, 20U);
    assert(is_valid_volume(lob.get_volume_at_price(100.0, Side::BUY)) ==
           false);

    // 測試相同價格下，掛單數量小於訂單數量
    add_buyer_order(lob, 4U, 100.0, 1002ULL, 40U);
    add_buyer_order(lob, 5U, 100.0, 1003ULL, 60U);
    add_seller_order(lob, 6U, 100.0, 1004ULL, 110U, Time_In_Force::FOK);
    assert(lob.has_order(4U) == true);
    assert(lob.has_order(5U) == true);
    assert(lob.get_volume_at_price(100, Side::BUY) == 100U);
    assert(lob.has_order(6U) == false);
    // 清空 lob
    add_seller_order(lob, 6U, 100.0, 1004ULL, 100U);
    assert(is_valid_volume(lob.get_volume_at_price(100.0, Side::BUY)) ==
           false);

    // 測試不同價格下，掛單數量小於訂單數量
    add_buyer_order(lob, 7U, 100.0, 1005ULL, 80U);
    add_seller_order(lob, 8U, 90.0, 1006ULL, 90U, Time_In_Force::FOK);
    assert(lob.has_order(7U) == true);
    assert(lob.get_volume_at_price(100.0, Side::BUY) == 80U);
    assert(lob.has_order(8U) == false);
    // 清空 lob
    add_seller_order(lob, 8U, 100.0, 1006ULL, 80U);
    assert(is_valid_volume(lob.get_volume_at_price(100.0, Side::BUY)) ==
           false);

    // 測試不同價格下，掛單數量小於訂單數量
    add_buyer_order(lob, 9U, 100.0, 1007ULL, 80U);
    add_buyer_order(lob, 10U, 110.0, 1008ULL, 60U);
    add_seller_order(lob, 11U, 90.0, 1009ULL, 150U, Time_In_Force::FOK);
    assert(lob.has_order(9U) == true);
    assert(lob.get_volume_at_price(100.0, Side::BUY) == 80U);
    assert(lob.get_volume_at_price(110.0, Side::BUY) == 60U);
    assert(lob.has_order(10U) == true);
    assert(lob.has_order(11U) == false);
    // 清空 lob
    add_seller_order(lob, 11U, 100.0, 1009ULL, 140U);
    assert(is_valid_volume(lob.get_volume_at_price(100.0, Side::BUY)) ==
           false);
    assert(is_valid_volume(lob.get_volume_at_price(110.0, Side::BUY)) ==
           false);

    // 測試相同價格下，同一訂單上全部成交
    add_buyer_order(lob, 12U, 100.0, 1010ULL, 50U);
    add_seller_order(lob, 13U, 100.0, 1011ULL, 50U, Time_In_Force::FOK);
    // std::cerr << lob.get_volume_at_price(100.0, Side::SELL) << '\n';
    assert(lob.has_order(12U) == false);
    assert(is_valid_volume(lob.get_volume_at_price(100.0, Side::BUY)) ==
           false);
    assert(lob.has_order(13U) == false);

    // 測試相同價格下，不同訂單上全部成交
    add_buyer_order(lob, 14U, 100.0, 1012ULL, 60U);
    add_buyer_order(lob, 15U, 100.0, 1013ULL, 40U);
    add_seller_order(lob, 16U, 100.0, 1014ULL, 100U, Time_In_Force::FOK);
    assert(lob.has_order(14U) == false);
    assert(is_valid_volume(lob.get_volume_at_price(100.0, Side::BUY)) ==
           false);
    assert(lob.has_order(15U) == false);
    assert(lob.has_order(16U) == false);

    // 測試不同價格下，同一訂單上全部成交
    add_buyer_order(lob, 17U, 100.0, 1015ULL, 100U);
    add_seller_order(lob, 18U, 90.0, 1016ULL, 100U, Time_In_Force::FOK);
    assert(lob.has_order(17U) == false);
    assert(is_valid_volume(lob.get_volume_at_price(100.0, Side::BUY)) ==
           false);
    assert(lob.has_order(18U) == false);

    // 測試不同價格下，不同訂單上全部成交
    add_buyer_order(lob, 19U, 100.0, 1017ULL, 30U);
    add_buyer_order(lob, 20U, 110.0, 1018ULL, 70U);
    add_seller_order(lob, 21U, 90.0, 1019ULL, 100U, Time_In_Force::FOK);
    assert(lob.has_order(19U) == false);
    assert(is_valid_volume(lob.get_volume_at_price(100.0, Side::BUY)) ==
           false);
    assert(lob.has_order(20U) == false);
    assert(is_valid_volume(lob.get_volume_at_price(110.0, Side::BUY)) ==
           false);
    assert(lob.has_order(21U) == false);

    std::cout << "place FOK order passed!\n";
  }

  /* 測試取消訂單 */
  {
    L3_LOB lob;
    std::vector<uint32_t> order_list;
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

    std::cout << "cancel order passed!\n";
  }

  std::cout << "test order passed!\n";
}