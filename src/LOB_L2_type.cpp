#include "LOB_L2_type.hpp"

#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <stdexcept>
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
void L2_LOB::load_CSV_snapshot(
    const std::string& path,
    std::function<void(const L2_LOB&, const Rows_data&)> on_batch_applied,
    std::optional<uint64_t> stop_at_timestamp) {
  std::vector<Rows_data> rows;
  rows.reserve(512);
  std::fstream fin;
  fin.open(path, std::ios::in);
  if (!fin.is_open()) {
    throw std::runtime_error(
        "L2_LOB::load_CSV_snapshot: failed to open file: " + path);
  }

  bool current_batch_is_snapshot = true;
  std::string line;
  std::optional<Rows_data> pending;

  /* 直接讀取標題列，並丟棄它 */
  getline(fin, line);
  line.clear();

  while (1) {
    uint64_t row_timestamp;
    double row_price;
    uint64_t row_total_volume;
    Side row_side;
    bool row_is_snapshot;

    if (!pending.has_value()) {
      /* 在沒有暫存值的時候，才讀取 */
      /* 若讀取失敗，則代表讀完了 */
      if (!getline(fin, line)) {
        /* 將最後一個狀態的 Rows_data 應用後才結束 */
        if (rows.size() != 0) {
          apply_snapshot(rows);
          if (on_batch_applied) on_batch_applied(*this, rows.back());
        }
        return;
      }

      /* 解析字串 */
      std::stringstream ss(line);
      std::string field;
      getline(ss, field, ','); /* 讀取 timestamp */
      row_timestamp = std::stoull(field);
      getline(ss, field, ','); /* 讀取 side */
      row_side = field == "bid" ? Side::BUY : Side::SELL;
      getline(ss, field, ','); /* 讀取 price */
      row_price = std::stod(field);
      getline(ss, field, ','); /* 讀取 volume */
      row_total_volume = std::stoull(field);
      getline(ss, field, ','); /* 讀取 is_snapshot */
      row_is_snapshot = field == "true" ? true : false;

    } else /* 有暫存值，則直接用 */ {
      Rows_data row_data = pending.value();
      row_timestamp = row_data.timestamp;
      row_side = row_data.side;
      row_price = row_data.price;
      row_total_volume = row_data.volume;
      row_is_snapshot = row_data.is_snapshot;
      pending.reset();
    }

    /* 從重建 L2_LOB 到 delta */
    if (current_batch_is_snapshot && !row_is_snapshot) {
      current_batch_is_snapshot = false;
      pending = Rows_data(row_timestamp, row_price, row_total_volume, row_side,
                          row_is_snapshot);
      apply_snapshot(rows);
      // 有傳入輸出的 lambda 時，就輸出
      if (on_batch_applied && rows.size() != 0)
        on_batch_applied(*this, rows.back());
      rows.clear();
      continue;
    }

    /* 判斷是否要提前終止 */
    if (stop_at_timestamp.has_value() && stop_at_timestamp < row_timestamp) {
      return;
    }

    /* 從 delta 到重建 L2_LOB */
    if (!current_batch_is_snapshot && row_is_snapshot)
      current_batch_is_snapshot = true;

    /* 如果是重建的 Row_data 就放入 vector，否則直接應用在 L2_LOB 上 */
    if (row_is_snapshot) {
      rows.emplace_back(row_timestamp, row_price, row_total_volume, row_side,
                        row_is_snapshot);
    } else {
      Rows_data curr_row(row_timestamp, row_price, row_total_volume, row_side,
                         row_is_snapshot);
      apply_single_row(curr_row);
      if (on_batch_applied) on_batch_applied(*this, curr_row);
    }

    line.clear();
  }
}

/*****************************************************************************
 * apply_single_row: 套用單比的 Row_data，不負責控管當前 batch 狀態          *
 *****************************************************************************/
void L2_LOB::apply_single_row(const Rows_data& row_data) {
  if (row_data.is_snapshot == true) /* 重建 LOB*/ {
    if (row_data.side == Side::BUY) /* 買方 */ {
      buyer_tree.insert_emplace(row_data.price, row_data.price,
                                row_data.volume);
    } else /* 賣方 */ {
      seller_tree.insert_emplace(row_data.price, row_data.price,
                                 row_data.volume);
    }
  } else /* delta */ {
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
        if (row_data.volume != 0)
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
        if (row_data.volume != 0)
          seller_tree.insert_emplace(row_data.price, row_data.price,
                                     row_data.volume);
      }
    }
  }
}

/*****************************************************************************
 * apply_snapshot: 依據 Raw_data，重建 LOB                                   *
 *****************************************************************************/
void L2_LOB::apply_snapshot(const std::vector<Rows_data>& rows) {
  bool prev_is_snapshot = false;
  for (const Rows_data& row_data : rows) {
    /* 剛開始進入重建 */
    if (row_data.is_snapshot && prev_is_snapshot == false) {
      prev_is_snapshot = true;
      buyer_tree.clear();
      seller_tree.clear();
    }

    /* 剛開始進入 delta */
    if (!row_data.is_snapshot && prev_is_snapshot == true)
      prev_is_snapshot = false;

    apply_single_row(row_data);
  }
}
