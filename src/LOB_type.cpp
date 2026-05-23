#include "LOB_type.hpp"

#include <algorithm>
#include <iostream>
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

/* 檢查用途 */
/*
std::string LOB::inorder_check() {
  std::stringstream ss;
  ss << "buyer tree: \n" << buyer_tree.inorder();
  ss << "\nseller tree: \n" << seller_tree.inorder();
  return ss.str();
}
*/

void LOB::order_matching(Order& new_order) {
  if (new_order.side == Side::BUY) /* 新訂單為買方 */ {
    bool is_matched = false;
    while (!seller_tree.isempty() && new_order.volume) {
      PriceLevel* seller_price_level = seller_tree.get_leftmost_node();
      // 成交
      if (seller_price_level->price <= new_order.price) {
        is_matched = true;
        Order& seller_order = seller_price_level->front();

        // 判斷成交量
        uint32_t make_deal_volume;
        make_deal_volume = std::min(new_order.volume, seller_order.volume);
        new_order.volume -= make_deal_volume;
        seller_order.volume -= make_deal_volume;

        // 顯示成交狀況
        std::cout << "[TRADE] Match! Buyer ID: " << new_order.order_id
                  << " | Seller ID: " << seller_order.order_id
                  << " | Price: " << seller_order.price
                  << " | Volume: " << make_deal_volume << std::endl;

        // 清空賣方訂單，push 出去，並判斷 price level 是否為空
        if (seller_order.volume == 0) {
          order_map.erase(seller_order.order_id);
          seller_price_level->pop();
          if (seller_price_level->isempty()) {
            // 因為暫時 remove 只能輸入 T data，所以會消耗一點
            // 複製的成本，等待未來在修改 RBT 時一起修改
            seller_tree.remove(*seller_price_level);
          }
        }
      } else {
        break;
      }
    }
    if (new_order.volume > 0) {
      // 印出掛單訊息
      std::cout << "[LIMIT] Order " << new_order.order_id << " ("
                << new_order.side << ") ";
      if (!is_matched) {
        std::cout << "place to Book";
      } else {
        std::cout << "rest remaining to Book";
      }
      std::cout << " | Price: " << new_order.price
                << " | Volume: " << new_order.volume << std::endl;

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
    while (!buyer_tree.isempty() && new_order.volume) {
      PriceLevel* buyer_price_level = buyer_tree.get_leftmost_node();
      // 成交
      if (buyer_price_level->price >= new_order.price) {
        is_matched = true;
        Order& buyer_order = buyer_price_level->front();

        // 判斷成交量
        uint32_t make_deal_volume;
        make_deal_volume = std::min(new_order.volume, buyer_order.volume);
        new_order.volume -= make_deal_volume;
        buyer_order.volume -= make_deal_volume;

        // 顯示成交狀況
        std::cout << "[TRADE] Match! Buyer ID: " << buyer_order.order_id
                  << " | Seller ID: " << new_order.order_id
                  << " | Price: " << buyer_order.price
                  << " | Volume: " << make_deal_volume << std::endl;

        // 如果清空了買方訂單，push 出去，並判斷 price level 是否為空
        if (buyer_order.volume == 0) {
          order_map.erase(buyer_order.order_id);
          buyer_price_level->pop();
          if (buyer_price_level->isempty()) {
            // 因為暫時 remove 只能輸入 T data，所以會消耗一點
            // 複製的成本，等待未來在修改 RBT 時一起修改
            buyer_tree.remove(*buyer_price_level);
          }
        }
      } else {
        break;
      }
    }
    if (new_order.volume > 0) {
      // 印出掛單訊息
      std::cout << "[LIMIT] Order " << new_order.order_id << " ("
                << new_order.side << ") ";
      if (!is_matched) {
        std::cout << "place to Book";
      } else {
        std::cout << "rest remaining to Book";
      }
      std::cout << " | Price: " << new_order.price
                << " | Volume: " << new_order.volume << std::endl;

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
  OrderLocation* location = &order_map.at(tar_idx);
  if (!location) {
    std::cout << "[CANCEL REJECT] Order " << tar_idx << " not found."
              << std::endl;

    return;
  }
  location->pos_price_level->order_queue.erase(location->order_it);
  if (location->pos_price_level->isempty()) {
    // 與之前相同，remove 此時只能接受型別 T，等待未來的優化
    if (location->side == Side::BUY) {
      buyer_tree.remove(*location->pos_price_level);
    } else {
      seller_tree.remove(*location->pos_price_level);
    }
  }
  order_map.erase(tar_idx);
}