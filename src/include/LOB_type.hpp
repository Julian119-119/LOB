#ifndef __LOB_TYPE_HPP__
#define __LOB_TYPE_HPP__

#include <cstdint>
#include <iterator>
#include <list>
#include <memory_resource>
#include <ostream>

#include "RedBlackTree.hpp"

enum class Side: uint8_t { BUY, SELL };

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
  double price;
  uint64_t timestamp;
  uint32_t volume;

  Order(uint32_t ID, Side si, double P, uint64_t TS, uint32_t V)
      : order_id(ID), side(si), price(P), timestamp(TS), volume(V) {}
};

class PriceLevel {
private:
  double price;
  uint64_t total_volume;
  std::pmr::list<Order> order_queue;

public:
  PriceLevel(double P, std::pmr::memory_resource *pool)
      : price(P), total_volume(0), order_queue(pool) {}
  double getprice() const { return price; }
  auto push(Order newOrder);
  Order &front();
  void pop();
  bool isempty() { return order_queue.empty(); }

  friend class LOB;
};

class Less_priceLevel {
public:
  bool operator()(const PriceLevel &a, const PriceLevel &b) const {
    return a.getprice() < b.getprice();
  }
};

class Greater_priceLevel {
public:
  bool operator()(const PriceLevel &a, const PriceLevel &b) const {
    return a.getprice() > b.getprice();
  }
};

struct OrderLocation {
  PriceLevel *pos_price_level;
  std::pmr::list<Order>::iterator order_it;
  Side side;
};

class LOB {
private:
  std::pmr::unsynchronized_pool_resource pool;

  RedBlackTree<PriceLevel, Greater_priceLevel> buyer_tree;
  RedBlackTree<PriceLevel, Less_priceLevel> seller_tree;
  std::pmr::unordered_map<uint32_t, OrderLocation> order_map;

  bool order_matching(Order &new_order);
public:
  LOB() : order_map(&pool) {}
  double get_bid_price() const { return buyer_tree.get_smallest().price; }
  double get_ask_price() const { return seller_tree.get_smallest().price; }
  void place_order(Order new_order);
  void cancel_order(uint32_t tar_idx);
};

#endif