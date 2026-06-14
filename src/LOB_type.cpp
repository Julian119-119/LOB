#include "LOB_type.hpp"

#include <algorithm>
#include <sstream>

auto PriceLevel::push(Order newOrder) {
  total_volume += newOrder.volume;
  // 新的單必定比舊的時間要晚，所以 push 在最後面
  order_queue.push_back(newOrder);
  return std::prev(order_queue.end(), 1);
}

Order& PriceLevel::front() { return order_queue.front(); }

void PriceLevel::pop() {
  total_volume -= order_queue.front().volume;
  order_queue.pop_front();
}

double LOB::get_bid_price() const {
  if (buyer_tree.empty())
    return NO_VALUE;
  else
    return buyer_tree.get_leftmost_node()->data.getprice();
}

double LOB::get_ask_price() const {
  if (seller_tree.empty())
    return NO_VALUE;
  else
    return seller_tree.get_leftmost_node()->data.getprice();
}

bool LOB::has_order(uint32_t order_idx) const {
  if (order_map.find(order_idx) != order_map.end()) {
    return true;
  } else {
    return false;
  }
}

uint32_t LOB::get_volume_at_price(double tar_price, Side tar_side) const {
  /* 買方 */
  if (tar_side == Side::BUY) {
    auto lt = buyer_tree.get_leftmost_node();
    while (lt && lt->data.price != tar_price) {
      lt = buyer_tree.get_successor(lt);
    }

    if (!lt) 
        return NO_VALUE;
    else 
        return lt->data.total_volume;
  } else /* 賣方 */ {
    auto lt = seller_tree.get_leftmost_node();
    while (lt && lt->data.price != tar_price) {
      lt = seller_tree.get_successor(lt);
    }

    if (!lt)
        return NO_VALUE;
    else
        return lt->data.total_volume;
  }
}

void LOB::order_matching(Order& new_order) {
  // 如果 time in force 為 FOK 則先試跑，確認可全部成交才撮合
  // 否則直接徹單
  if (new_order.time_in_force == Time_in_force::FOK) {
    if (new_order.side == Side::BUY) /* 新訂單為買方 */ {
      auto order_it = buyer_tree.get_leftmost_node();
      uint32_t curr_total_volume = 0;
      while (1) {
        if (order_it && order_it->data.price <= new_order.price) {
          curr_total_volume += order_it->data.total_volume;
          if (curr_total_volume >= new_order.volume)
            break;
          else
            order_it = buyer_tree.get_successor(order_it);
        } else
          return;
      }
    } else /* 新訂單為賣方 */ {
      auto order_it = seller_tree.get_leftmost_node();
      uint32_t curr_total_volume = 0;
      while (1) {
        if (order_it && order_it->data.price >= new_order.price) {
          curr_total_volume += order_it->data.total_volume;
          if (curr_total_volume >= new_order.volume)
            break;
          else
            order_it = buyer_tree.get_successor(order_it);
        } else
          return;
      }
    }
  }

  /* 實際撮合 */
  if (new_order.side == Side::BUY) /* 新訂單為買方 */ {
    bool is_matched = false;
    while (!seller_tree.empty() && new_order.volume) {
      PriceLevel* seller_price_level = &seller_tree.get_leftmost_node()->data;
      // 成交
      if (seller_price_level->price <= new_order.price) {
        is_matched = true;
        Order& seller_order = seller_price_level->front();

        // 判斷成交量
        uint32_t make_deal_volume;
        make_deal_volume = std::min(new_order.volume, seller_order.volume);
        new_order.volume -= make_deal_volume;
        seller_order.volume -= make_deal_volume;

        // 清空賣方訂單，push 出去，並判斷 price level 是否為空
        if (seller_order.volume == 0) {
          order_map.erase(seller_order.order_id);
          seller_price_level->pop();
          if (seller_price_level->empty()) {
            seller_tree.remove(seller_price_level->price);
          }
        }
      } else {
        break;
      }
    }
    if (new_order.time_in_force != Time_in_force::IOC && new_order.volume > 0) {

      // 掛單
      // 使用異構插入，插入時確認沒有節點才建構
      auto placed_price_level =
          buyer_tree.insert_emplace(new_order.price, new_order.price, &pool);
      auto new_order_lt = placed_price_level->push(new_order);
      order_map.emplace(
          new_order.order_id,
          OrderLocation{placed_price_level, new_order_lt, Side::BUY});

      return;
    }
  } else /* 新訂單為賣方 */ {
    bool is_matched = false;
    while (!buyer_tree.empty() && new_order.volume) {
      PriceLevel* buyer_price_level = &buyer_tree.get_leftmost_node()->data;
      // 成交
      if (buyer_price_level->price >= new_order.price) {
        is_matched = true;
        Order& buyer_order = buyer_price_level->front();

        // 判斷成交量
        uint32_t make_deal_volume;
        make_deal_volume = std::min(new_order.volume, buyer_order.volume);
        new_order.volume -= make_deal_volume;
        buyer_order.volume -= make_deal_volume;

        // 如果清空了買方訂單，push 出去，並判斷 price level 是否為空
        if (buyer_order.volume == 0) {
          order_map.erase(buyer_order.order_id);
          buyer_price_level->pop();
          if (buyer_price_level->empty()) {
            buyer_tree.remove(buyer_price_level->price);
          }
        }
      } else {
        break;
      }
    }
    if (new_order.time_in_force != Time_in_force::IOC && new_order.volume > 0) {

      // 掛單
      // 使用異構插入，插入時確認沒有節點才建構
      auto placed_price_level =
          seller_tree.insert_emplace(new_order.price, new_order.price, &pool);
      auto new_order_lt = placed_price_level->push(new_order);
      order_map.emplace(
          new_order.order_id,
          OrderLocation{placed_price_level, new_order_lt, Side::SELL});

      return;
    }
  }
}

void LOB::place_order(Order new_order) { order_matching(new_order); }

void LOB::cancel_order(uint32_t tar_idx) {
  auto location_lt = order_map.find(tar_idx);
  if (location_lt == order_map.end()) {
    return;
  }

  OrderLocation* location = &location_lt->second;

  location->pos_price_level->order_queue.erase(location->order_it);
  if (location->pos_price_level->empty()) {
    if (location->side == Side::BUY) {
      buyer_tree.remove(location->pos_price_level->price);
    } else {
      seller_tree.remove(location->pos_price_level->price);
    }
  }
  order_map.erase(tar_idx);
}