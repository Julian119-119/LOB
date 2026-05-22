#include "LOB_type.hpp"
#include <algorithm>
#include <iostream>

auto PriceLevel::push(Order newOrder) {
  total_volume += newOrder.volume;
  // 新的單必定比舊的時間要晚，所以 push 在最後面
  order_queue.push_back(newOrder);
  return std::prev(order_queue.end(), 1);
}

Order &PriceLevel::front() { return order_queue.front(); }

void PriceLevel::pop() {
  total_volume -= order_queue.front().volume;
  order_queue.pop_front();
}

bool LOB::order_matching(Order &new_order) {
  if (new_order.side == Side::BUY) {
    bool ismaked = false;
    while (new_order.volume) {
      PriceLevel *seller_price_level = &seller_tree.get_smallest();
      // 成交
      if (seller_price_level->price <= new_order.price) {
        ismaked = true;
        Order &seller_order = seller_price_level->front();

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
      } else /* 沒成交 */ {
        // 印出掛單訊息
        std::cout << "[LIMIT] Order " << new_order.order_id << " ("
                  << new_order.side << ") ";
        if (ismaked) {
          std::cout << "place to Book";
        } else {
          std::cout << "rest remaining to Book";
        }
        std::cout << " | Price" << new_order.price
                  << " | Volume: " << new_order.volume << std::endl;

        // 掛單
        // 目前這裡為了要 insert 必須先製作出一個 price level
        // 再複製進去，因此效能不好，等待未來修改
        PriceLevel new_price_level(new_order.price, pool);
        PriceLevel *placed_price_level =
            &buyer_tree.insert(new_price_level)->data;
        auto new_order_lt = placed_price_level->push(new_order);
        order_map.emplace(new_order.order_id, OrderLocation{placed_price_level, new_order_lt, Side::BUY});
      }
    }
  }
}