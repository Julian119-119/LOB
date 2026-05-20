#ifndef __LOB_TYPE_H__
#define __LOB_TYPE_H__

#include <cstdint>
#include <list>

#include "RedBlackTree.hpp"

struct order {
  uint32_t order_id;
  double price;
  uint64_t timestamp;
  uint32_t volume;

  order(uint32_t ID, double P, uint64_t TS, uint32_t V)
      : order_id(ID), price(P), timestamp(TS), volume(V) {}
};

struct priceLevel {
  double price;
  uint64_t total_volume;
  std::list<order> order_queue;

  priceLevel(double P) : price(P), total_volume(0) {}
  auto push(order newOrder);
  order &front();
  void pop();
};

struct less_priceLevel {
  bool operator()(const priceLevel &a, const priceLevel &b) const {
    return a.price < b.price;
  }
};

struct greater_priceLevel {
  bool operator()(const priceLevel &a, const priceLevel &b) const {
    return a.price > b.price;
  }
};

/****************************************************************************/

auto priceLevel::push(order newOrder) {
  total_volume += newOrder.volume;
  // 新的單必定比舊的時間要晚，所以 push 在最後面
  order_queue.push_back(newOrder);
  return order_queue.end();
}

order &priceLevel::front() {
  if (order_queue.empty())
    std::runtime_error("There is empty price level");
   
  return order_queue.front();
}

void priceLevel::pop() {
    if (order_queue.empty()) 
        std::runtime_error("There is empty price level");
    
    total_volume -= order_queue.front().volume;
    order_queue.pop_front();
}


#endif