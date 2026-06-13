#ifndef __LOB_TYPE_HPP__
#define __LOB_TYPE_HPP__

#include <cstdint>
#include <iterator>
#include <list>
#include <memory_resource>
#include <ostream>
#include <optional>

#include "RedBlackTree.hpp"

constexpr int NO_VALUE = -1;

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

/* 測試，用來檢查*/
/*
inline std::ostream& operator<<(std::ostream& os, const Order& order) {
  os << "ID: " << order.order_id << ", side: " << order.side
     << ", price: " << order.price << "$, timestamp: " << order.timestamp
     << ", volume: " << order.volume;

  return os;
}
*/

class PriceLevel {
 private:
  double price;
  uint64_t total_volume;
  std::pmr::list<Order> order_queue;

 public:
  PriceLevel(double P, std::pmr::memory_resource* pool)
      : price(P), total_volume(0), order_queue(pool) {}
  double getprice() const { return price; }
  auto push(Order newOrder);
  Order& front();
  void pop();
  bool empty() { return order_queue.empty(); }

  friend class LOB;
  friend std::ostream& operator<<(std::ostream& os, const PriceLevel& pl);
};

/* 測試用，用來讓 inorder 可以檢查它*/
/*
inline std::ostream& operator<<(std::ostream& os, const PriceLevel& pl) {
  os << "Price: " << pl.price << " | Total volume: " << pl.total_volume
     << " | order in this price level is ";
  for (const auto& curr_order : pl.order_queue) {
    os << curr_order << "; ";
  }

  return os;
}
*/

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

class Greater_priceLevel {
 public:
  bool operator()(const PriceLevel& a, const PriceLevel& b) const {
    return a.getprice() > b.getprice();
  }
  bool operator()(const PriceLevel& a, const double p) const {
    return a.getprice() < p;
  }
  bool operator()(const double p, const PriceLevel& a) const {
    return p < a.getprice();
  }
};

struct OrderLocation {
  PriceLevel* pos_price_level;
  std::pmr::list<Order>::iterator order_it;
  Side side;
};

class LOB {
 private:
  std::pmr::unsynchronized_pool_resource pool;

  RedBlackTree<PriceLevel, Greater_priceLevel> buyer_tree;
  RedBlackTree<PriceLevel, Less_priceLevel> seller_tree;
  std::pmr::unordered_map<uint32_t, OrderLocation> order_map;

  void order_matching(Order& new_order);

 public:
  LOB() : order_map(&pool) {}
  double get_bid_price() const;
  double get_ask_price() const;
  bool has_order(uint32_t order_idx) const;

  void place_order(Order new_order);
  void cancel_order(uint32_t tar_idx);
  // std::string inorder_check();
};

#endif