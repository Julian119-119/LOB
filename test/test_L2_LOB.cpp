#include <cassert>
#include <iostream>

#include "LOB_L2_type.hpp"
#include "test_helper.hpp"

void test_load_CSV_snapshot() {
  {
    L2_LOB book;
    std::string path = "test/fixtures/l2_load_csv_snapshot_test.csv";
    book.load_CSV_snapshot(path);

    std::vector<PriceLevelInfo> sell_ans;
    sell_ans.push_back({6421.5, 18640});
    sell_ans.push_back({6422.0, 1190});
    sell_ans.push_back({6422.5, 1080});
    sell_ans.push_back({6423.0, 1510});
    sell_ans.push_back({6423.5, 6510});
    sell_ans.push_back({6431.0, 12790});

    std::vector<PriceLevelInfo> buy_ans;
    buy_ans.push_back({6421.0, 141360});
    buy_ans.push_back({6420.5, 18830});
    buy_ans.push_back({6420.0, 34410});
    buy_ans.push_back({6419.5, 6570});
    buy_ans.push_back({6419.0, 8080});
    buy_ans.push_back({6414.5, 19280});

    std::vector<PriceLevelInfo> buy_pli = book.get_top_k_info(7, Side::BUY);
    std::vector<PriceLevelInfo> sell_pli = book.get_top_k_info(7, Side::SELL);

    assert(buy_ans == buy_pli);
    assert(sell_ans == sell_pli);
  }

  std::cout << "test_load_CSV_snapshot passed!\n" << std::flush;
}

void test_apply_snapshot() {
  /* 測試沒有 delta 的狀況 */
  {
    L2_LOB book;
    std::vector<Rows_data> rows;
    rows.emplace_back(100.5, 100, Side::BUY, true);
    rows.emplace_back(100.8, 50, Side::BUY, true);
    rows.emplace_back(100.7, 200, Side::BUY, true);
    rows.emplace_back(110.0, 10, Side::BUY, true);
    rows.emplace_back(10, 150, Side::SELL, true);
    rows.emplace_back(80.0, 10, Side::SELL, true);
    rows.emplace_back(50.7, 200, Side::SELL, true);
    rows.emplace_back(20.8, 50, Side::SELL, true);
    book.apply_snapshot(rows);

    std::vector<PriceLevelInfo> buy_ans;
    buy_ans.push_back({110.0, 10});
    buy_ans.push_back({100.8, 50});
    buy_ans.push_back({100.7, 200});
    buy_ans.push_back({100.5, 100});

    std::vector<PriceLevelInfo> sell_ans;
    sell_ans.push_back({10, 150});
    sell_ans.push_back({20.8, 50});
    sell_ans.push_back({50.7, 200});
    sell_ans.push_back({80.0, 10});

    std::vector<PriceLevelInfo> buy_pli = book.get_top_k_info(4, Side::BUY);
    assert(buy_pli == buy_ans);

    std::vector<PriceLevelInfo> sell_pli = book.get_top_k_info(4, Side::SELL);
    assert(sell_pli == sell_ans);
  }

  /* 測試有 delta 的狀況 */
  {
    L2_LOB book;
    std::vector<Rows_data> rows;

    /* 預設 L2_LOB 的 Rows_data */
    rows.emplace_back(6421.5, 18640, Side::SELL, true);
    rows.emplace_back(6422, 1190, Side::SELL, true);
    rows.emplace_back(6422.5, 1080, Side::SELL, true);
    rows.emplace_back(6423, 1400, Side::SELL, true);
    rows.emplace_back(6421, 141360, Side::BUY, true);
    rows.emplace_back(6420.5, 18830, Side::BUY, true);
    rows.emplace_back(6420, 34410, Side::BUY, true);
    rows.emplace_back(6419.5, 6570, Side::BUY, true);

    /* 增加 PriceLevel */
    rows.emplace_back(6423.5, 6630, Side::SELL, false);
    rows.emplace_back(6419, 8080, Side::BUY, false);

    /* 修改 volume，但不歸零 */
    rows.emplace_back(6423, 100, Side::SELL, false);
    rows.emplace_back(6419.5, 100, Side::BUY, false);

    /* 修改 volueme, 但修改後的 volume 歸零 */
    rows.emplace_back(6421.5, 0, Side::SELL, false);
    rows.emplace_back(6421, 0, Side::BUY, false);

    std::vector<PriceLevelInfo> sell_ans;
    sell_ans.push_back({6422, 1190});
    sell_ans.push_back({6422.5, 1080});
    sell_ans.push_back({6423, 100});
    sell_ans.push_back({6423.5, 6630});

    std::vector<PriceLevelInfo> buy_ans;
    buy_ans.push_back({6420.5, 18830});
    buy_ans.push_back({6420, 34410});
    buy_ans.push_back({6419.5, 100});
    buy_ans.push_back({6419, 8080});

    book.apply_snapshot(rows);
    /* 刻意在 get_top_k_info 中 k 多一，確保沒有殭屍節點 */
    std::vector<PriceLevelInfo> buy_pli = book.get_top_k_info(5, Side::BUY);
    std::vector<PriceLevelInfo> sell_pli = book.get_top_k_info(5, Side::SELL);
    assert(buy_pli == buy_ans);
    assert(sell_pli == sell_ans);
  }

  /* 測試有 delta 後又有 is_snapshot 的狀況 */
  {
    L2_LOB book;
    std::vector<Rows_data> rows;

    /* 預設 L2_LOB 的 Rows_data */
    rows.emplace_back(6421.5, 18640, Side::SELL, true);
    rows.emplace_back(6422, 1190, Side::SELL, true);
    rows.emplace_back(6422.5, 1080, Side::SELL, true);
    rows.emplace_back(6423, 1400, Side::SELL, true);
    rows.emplace_back(6421, 141360, Side::BUY, true);
    rows.emplace_back(6420.5, 18830, Side::BUY, true);
    rows.emplace_back(6420, 34410, Side::BUY, true);
    rows.emplace_back(6419.5, 6570, Side::BUY, true);

    /* 增加 PriceLevel */
    rows.emplace_back(6423.5, 6630, Side::SELL, false);
    rows.emplace_back(6419, 8080, Side::BUY, false);

    /* 重新出現 is_sanpshot */
    rows.emplace_back(100, 100, Side::SELL, true);
    rows.emplace_back(100, 100, Side::BUY, true);

    book.apply_snapshot(rows);

    std::vector<PriceLevelInfo> sell_ans;
    sell_ans.push_back({100, 100});
    std::vector<PriceLevelInfo> buy_ans;
    buy_ans.push_back({100, 100});
    std::vector<PriceLevelInfo> sell_pli = book.get_top_k_info(2, Side::SELL);
    std::vector<PriceLevelInfo> buy_pli = book.get_top_k_info(2, Side::BUY);

    assert(sell_pli == sell_ans);
    assert(buy_pli == buy_ans);
  }

  std::cout << "test_apply_snapshot passed!\n" << std::flush;
}
