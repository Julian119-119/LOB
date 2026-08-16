#ifndef __LOB_L2_TYPE_HPP__
#define __LOB_L2_TYPE_HPP__

#include <vector>

#include "LOB_common.hpp"
#include "RedBlackTree.hpp"

/*****************************************************************************
 * PriceLevel: RBT node 儲存的 class。用於紀錄價格檔位                       *
 *****************************************************************************/
class L2_PriceLevel {
 private:
  double price;
  uint64_t total_volume;

 public:
  L2_PriceLevel(double P) : price(P), total_volume(0) {}
  L2_PriceLevel(double p, uint64_t vol) : price(p), total_volume(vol) {}
  double getprice() const { return price; }
  uint64_t get_total_volume() const { return total_volume; }

  friend class L2_LOB;
};

/*****************************************************************************
 * Less_priceLevel: 比較兩者的 price 是否比較小。用於 seller tree            *
 *****************************************************************************/
class Less_L2_PriceLevel {
 public:
  bool operator()(const L2_PriceLevel& a, const L2_PriceLevel& b) const {
    return a.getprice() < b.getprice();
  }
  bool operator()(const L2_PriceLevel& a, const double p) const {
    return a.getprice() < p;
  }
  bool operator()(const double P, const L2_PriceLevel& a) const {
    return P < a.getprice();
  }
  bool operator()(const PriceLevelInfo& a, const PriceLevelInfo& b) const {
    return a.price < b.price;
  }
};

/*****************************************************************************
 * Greater_priceLevel: 比較兩者的 price 是否比較大。用於 buyer tree          *
 *****************************************************************************/
class Greater_L2_PriceLevel {
 public:
  bool operator()(const L2_PriceLevel& a, const L2_PriceLevel& b) const {
    return a.getprice() > b.getprice();
  }
  bool operator()(const L2_PriceLevel& a, const double p) const {
    return a.getprice() > p;
  }
  bool operator()(const double p, const L2_PriceLevel& a) const {
    return p > a.getprice();
  }
  bool operator()(const PriceLevelInfo& a, const PriceLevelInfo& b) const {
    return a.price > b.price;
  }
};

/*****************************************************************************
 * Rows_data: load_snapshot_rows 解析完 csv 後，每一個 Raw 的資訊              *
 *****************************************************************************/
struct Rows_data {
  double price;
  uint64_t volume;
  Side side;
  bool is_snapshot;

  Rows_data(double p, uint64_t vol, Side sd, bool is_ss)
      : price(p), volume(vol), side(sd), is_snapshot(is_ss) {}

  friend bool operator==(const Rows_data& a, const Rows_data& b);
};

inline bool operator==(const Rows_data& a, const Rows_data& b) {
  return a.price == b.price && a.volume == b.volume && a.side == b.side &&
         a.is_snapshot == b.is_snapshot;
}

/*****************************************************************************
 * L2_LOB:  limit order bool 主體                                            *
 *****************************************************************************/
class L2_LOB {
 private:
  RedBlackTree<L2_PriceLevel, Less_L2_PriceLevel> seller_tree;
  RedBlackTree<L2_PriceLevel, Greater_L2_PriceLevel> buyer_tree;

 public:
  void apply_snapshot(const std::vector<Rows_data>& rows); /* 重建 LOB */
  std::vector<Rows_data> load_CSV_snapshot(
      const std::string& path); /* 解析 CSV */

  /* 查詢 */
  double get_best_bid_price() const;    /* 取得最佳賣價       */
  double get_best_ask_price() const;    /* 取得最佳買價       */
  uint64_t get_best_bid_volume() const; /* 取得最佳賣價的量   */
  uint64_t get_best_ask_volume() const; /* 取得最佳買價的量   */
  double get_mid_price() const;         /* 取得買賣方中間價格 */
  double get_spread() const;            /* 取得買賣價差       */
  uint64_t get_volume_at_price(double tar_price, Side side) const;
  std::vector<PriceLevelInfo> get_top_k_info(size_t k, Side side);
};

#endif