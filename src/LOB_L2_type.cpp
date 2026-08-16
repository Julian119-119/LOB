#include "LOB_L2_type.hpp"

#include <fstream>
#include <iostream>
#include <string>

/*****************************************************************************
 * 查詢                                                                      *
 *****************************************************************************/
double L2_LOB::get_best_bid_price() const {
  if (buyer_tree.empty())
    return PRICE_NO_VALUE;
  else
    return buyer_tree.begin()->getprice();
}

double L2_LOB::get_best_ask_price() const {
  if (seller_tree.empty())
    return PRICE_NO_VALUE;
  else
    return seller_tree.begin()->getprice();
}

uint64_t L2_LOB::get_best_bid_volume() const {
  if (buyer_tree.empty()) {
    return VOLUME_NO_VALUE;
  } else {
    return buyer_tree.begin()->get_total_volume();
  }
}

uint64_t L2_LOB::get_best_ask_volume() const {
  if (seller_tree.empty()) {
    return VOLUME_NO_VALUE;
  } else {
    return seller_tree.begin()->get_total_volume();
  }
}

double L2_LOB::get_mid_price() const {
  if (buyer_tree.empty() || seller_tree.empty()) {
    return PRICE_NO_VALUE;
  }

  double best_bid = get_best_bid_price();
  double best_ask = get_best_ask_price();
  return best_bid + (best_ask - best_bid) / 2;
}

double L2_LOB::get_spread() const {
  if (buyer_tree.empty() || seller_tree.empty()) {
    return PRICE_NO_VALUE;
  }

  return get_best_ask_price() - get_best_bid_price();
}

uint64_t L2_LOB::get_volume_at_price(double tar_price, Side tar_side) const {
  /* 買方 */
  if (tar_side == Side::BUY) {
    auto it = buyer_tree.find_node(tar_price);

    if (it == buyer_tree.end())
      return VOLUME_NO_VALUE;
    else
      return it->total_volume;
  } else /* 賣方 */ {
    auto it = seller_tree.find_node(tar_price);

    if (it == seller_tree.end())
      return VOLUME_NO_VALUE;
    else
      return it->total_volume;
  }
}

std::vector<PriceLevelInfo> L2_LOB::get_top_k_info(size_t k, Side side) {
  std::vector<PriceLevelInfo> info;
  info.reserve(k);

  if (side == Side::BUY) /* 買方 */ {
    auto it = buyer_tree.begin();
    for (size_t i = 0; it != buyer_tree.end() && i < k; i++) {
      info.emplace_back(it->getprice(), it->get_total_volume());
      ++it;
    }
  } else /* 賣方 */ {
    auto it = seller_tree.begin();
    for (size_t i = 0; it != seller_tree.end() && i < k; i++) {
      info.emplace_back(it->getprice(), it->get_total_volume());
      ++it;
    }
  }

  return info;
}

/*****************************************************************************
 * load_CSV_snapshot: 處理解析的 CSV 檔                                      *
 *****************************************************************************/
std::vector<Rows_data> L2_LOB::load_CSV_snapshot(const std::string& path) {
  std::vector<Rows_data> rows;
  rows.reserve(512);
  std::fstream fin;
  fin.open(path, std::ios::in);

  double row_price;
  uint64_t row_total_volume;
  Side row_side;
  bool row_is_snapshot;

  bool isTitle = true;
  std::string line;
  while (getline(fin, line)) {
    if (!isTitle) {
      std::stringstream ss(line);
      std::string field;

      getline(ss, field, ','); /* 讀取 timestamp */

      getline(ss, field, ','); /* 讀取 side */
      if (field == "bid") {
        row_side = Side::BUY;
      } else {
        row_side = Side::SELL;
      }

      getline(ss, field, ','); /* 讀取 price */
      row_price = std::stod(field);

      getline(ss, field, ','); /* 讀取 volume */
      row_total_volume = std::stoull(field);

      getline(ss, field, ','); /* 讀取 is_snapshot */
      if (field == "true") {
        row_is_snapshot = true;
      } else {
        row_is_snapshot = false;
      }

      rows.emplace_back(row_price, row_total_volume, row_side, row_is_snapshot);
    } else {
      isTitle = false;
    }

    line.clear();
  }

  return rows;
}

/*****************************************************************************
 * apply_snapshot: 依據 Raw_data，重建 LOB                                   *
 *****************************************************************************/
void L2_LOB::apply_snapshot(const std::vector<Rows_data>& rows) {
  bool prev_is_snapshot = false;
  for (const Rows_data& row_data : rows) {
    if (row_data.is_snapshot) /* 重建 snapshot */ {
      /* 剛開始進入重建 */
      if (prev_is_snapshot == false) {
        prev_is_snapshot = true;
        buyer_tree.clear();
        seller_tree.clear();
      }

      if (row_data.side == Side::BUY) /* 買方 */ {
        buyer_tree.insert_emplace(row_data.price, row_data.price,
                                  row_data.volume);
      } else /* 賣方 */ {
        seller_tree.insert_emplace(row_data.price, row_data.price,
                                   row_data.volume);
      }
    } else /* delta：修改 Price_Level */ {

      /* 剛開始進入 delta */
      if (prev_is_snapshot == true) {
        prev_is_snapshot = false;
      }

      if (row_data.side == Side::BUY) /* 買方 */ {
        auto it = buyer_tree.find_node(row_data.price);
        if (it != buyer_tree.end()) /* 修改當前有的 PriceLevel */ {
          if (row_data.volume == 0) {
            /* TODO: remove() 目前只接受了 key，無法接受 iterator */
            /* 所以實際上用了兩次的 find_node */
            buyer_tree.remove(row_data.price);
          } else {
            it->total_volume = row_data.volume;
          }
        } else /* 插入新的節點 */ {
          buyer_tree.insert_emplace(row_data.price, row_data.price,
                                    row_data.volume);
        }
      } else /* 賣方 */ {
        auto it = seller_tree.find_node(row_data.price);
        if (it != seller_tree.end()) /* 修改當前有的 PriceLevel */ {
          if (row_data.volume == 0) {
            seller_tree.remove(row_data.price);
          } else {
            it->total_volume = row_data.volume;
          }
        } else {
          seller_tree.insert_emplace(row_data.price, row_data.price,
                                     row_data.volume);
        }
      }
    }
  }
}
