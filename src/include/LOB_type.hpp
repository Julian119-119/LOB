#ifndef __LOB_TYPE_HPP__
#define __LOB_TYPE_HPP__

#include <cstdint>
#include <iterator>
#include <list>
#include <memory_resource>
#include <optional>
#include <ostream>

#include "RedBlackTree.hpp"

static constexpr uint32_t IDX_NO_VALUE = std::numeric_limits<uint32_t>::max();
static constexpr uint64_t VOLUME_NO_VALUE =
    std::numeric_limits<uint64_t>::max();
static constexpr double PRICE_NO_VALUE = std::numeric_limits<double>::max();

inline bool is_valid_idx(uint32_t idx) { return idx != IDX_NO_VALUE; }

inline bool is_valid_volume(uint64_t volume) {
  return volume != VOLUME_NO_VALUE;
}

inline bool is_valid_price(double price) { return price != PRICE_NO_VALUE; }

enum class Side : uint8_t { BUY, SELL };
enum class Time_in_force : uint8_t { GTC, IOC, FOK };

inline std::ostream& operator<<(std::ostream& os, const Side& side) {
  if (side == Side::BUY) {
    os << "BUY";
  } else {
    os << "SELL";
  }

  return os;
}

/************************************************************
 * Order: 訂單。紀錄每筆交易                                *
 ************************************************************/
struct Order {
  uint32_t order_id;
  Side side;
  Time_in_force time_in_force;
  double price;
  uint64_t timestamp;
  uint32_t volume;

  Order(uint32_t ID, Side si, double P, uint64_t TS, uint32_t V,
        Time_in_force tif = Time_in_force::GTC)
      : order_id(ID),
        side(si),
        price(P),
        timestamp(TS),
        volume(V),
        time_in_force(tif) {}
};

inline std::ostream& operator<<(std::ostream& os, const Order& order) {
  os << "ID: " << order.order_id << ", side: " << order.side
     << ", price: " << order.price << "$, timestamp: " << order.timestamp
     << ", volume: " << order.volume;

  return os;
}

/***********************************************************
 * PriceLevel: RBT node 儲存的 class。用於紀錄價格檔位     *
 ***********************************************************/
class PriceLevel {
 private:
  double price;
  uint64_t total_volume;
  std::pmr::list<Order> order_queue;

 public:
  PriceLevel(double P, std::pmr::memory_resource* pool)
      : price(P), total_volume(0), order_queue(pool) {}
  double getprice() const { return price; }
  uint64_t get_total_volume() const { return total_volume; }
  auto push(Order newOrder);
  Order& front();
  void pop();
  bool empty() { return order_queue.empty(); }

  friend class LOB;
  friend std::ostream& operator<<(std::ostream& os, const PriceLevel& pl);
};

inline std::ostream& operator<<(std::ostream& os, const PriceLevel& pl) {
  os << "Price: " << pl.price << " | Total volume: " << pl.total_volume
     << " | order in this price level is ";
  for (const auto& curr_order : pl.order_queue) {
    os << curr_order << "; ";
  }

  return os;
}

/******************************************************************
 * Less_priceLevel: 比較兩者的 price 是否比較小。用於 seller tree *
 ******************************************************************/
class Less_priceLevel {
 public:
  bool operator()(const PriceLevel& a, const PriceLevel& b) const {
    return a.getprice() < b.getprice();
  }
  bool operator()(const PriceLevel& a, const double p) const {
    return a.getprice() < p;
  }
  bool operator()(const double P, const PriceLevel& a) const {
    return P < a.getprice();
  }
};

/******************************************************************
 * Less_priceLevel: 比較兩者的 price 是否比較小。用於 buyer tree  *
 ******************************************************************/
class Greater_priceLevel {
 public:
  bool operator()(const PriceLevel& a, const PriceLevel& b) const {
    return a.getprice() > b.getprice();
  }
  bool operator()(const PriceLevel& a, const double p) const {
    return a.getprice() > p;
  }
  bool operator()(const double p, const PriceLevel& a) const {
    return p > a.getprice();
  }
};

/*****************************************************
 * OrderLocation: 儲存於 hash table 中，用於快速查詢 *
 *****************************************************/
struct OrderLocation {
  PriceLevel* pos_price_level;
  std::pmr::list<Order>::iterator order_it;
  Side side;
};

/******************************************************
 * LOB:  limit order bool 主體                        *
 ******************************************************/
class LOB {
 private:
  std::pmr::unsynchronized_pool_resource pool;

  RedBlackTree<PriceLevel, Greater_priceLevel> buyer_tree;
  RedBlackTree<PriceLevel, Less_priceLevel> seller_tree;
  std::pmr::unordered_map<uint32_t, OrderLocation> order_map;

  void order_matching(Order& new_order); /* 訂單撮合 */
 public:
  LOB() : order_map(&pool) {}

  /* 查詢 */
  double get_best_bid_price() const;
  double get_best_ask_price() const;
  uint64_t get_best_bid_volume() const;
  uint64_t get_best_ask_volume() const;
  double get_mid_price() const; /* 取得買賣方中間價格 */
  double get_spread() const;    /* 取得買賣價差 */
  bool has_order(uint32_t order_idx) const;
  uint64_t get_volume_at_price(double tar_price, Side side) const;

  /* 下單與取消訂單 */
  void place_order(Order new_order);
  void cancel_order(uint32_t tar_idx);
};

#endif