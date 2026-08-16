#include "LOB_L3_type.hpp"

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

double L3_LOB::get_best_bid_price() const {
  if (buyer_tree.empty())
    return PRICE_NO_VALUE;
  else
    return buyer_tree.begin()->getprice();
}

double L3_LOB::get_best_ask_price() const {
  if (seller_tree.empty())
    return PRICE_NO_VALUE;
  else
    return seller_tree.begin()->getprice();
}

uint64_t L3_LOB::get_best_bid_volume() const {
  if (buyer_tree.empty()) {
    return VOLUME_NO_VALUE;
  } else {
    return buyer_tree.begin()->get_total_volume();
  }
}

uint64_t L3_LOB::get_best_ask_volume() const {
  if (seller_tree.empty()) {
    return VOLUME_NO_VALUE;
  } else {
    return seller_tree.begin()->get_total_volume();
  }
}

double L3_LOB::get_mid_price() const {
  if (buyer_tree.empty() || seller_tree.empty()) {
    return PRICE_NO_VALUE;
  }

  double best_bid = get_best_bid_price();
  double best_ask = get_best_ask_price();
  return best_bid + (best_ask - best_bid) / 2;
}

double L3_LOB::get_spread() const {
  if (buyer_tree.empty() || seller_tree.empty()) {
    return PRICE_NO_VALUE;
  }

  return get_best_ask_price() - get_best_bid_price();
}

bool L3_LOB::has_order(uint32_t order_idx) const {
  if (order_map.find(order_idx) != order_map.end()) {
    return true;
  } else {
    return false;
  }
}

uint64_t L3_LOB::get_volume_at_price(double tar_price, Side tar_side) const {
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

std::vector<PriceLevelInfo> L3_LOB::get_top_k_info(size_t k, Side side) {
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

void L3_LOB::order_matching(Order& new_order) {
  // 如果 time in force 為 FOK 則先試跑，確認可全部成交才撮合
  // 否則直接徹單
  if (new_order.time_in_force == Time_In_Force::FOK) {
    if (new_order.side == Side::BUY) /* 新訂單為買方 */ {
      auto order_it = seller_tree.begin();
      uint32_t curr_total_volume = 0;
      while (1) {
        if (order_it != seller_tree.end() &&
            order_it->price <= new_order.price) {
          curr_total_volume += order_it->total_volume;
          if (curr_total_volume >= new_order.volume)
            break;
          else
            ++order_it;
        } else
          return;
      }
    } else /* 新訂單為賣方 */ {
      auto order_it = buyer_tree.begin();
      uint32_t curr_total_volume = 0;
      while (1) {
        if (order_it != buyer_tree.end() &&
            order_it->price >= new_order.price) {
          curr_total_volume += order_it->total_volume;
          if (curr_total_volume >= new_order.volume)
            break;
          else
            ++order_it;
        } else
          return;
      }
    }
  }

  /* 實際撮合 */
  if (new_order.side == Side::BUY) /* 新訂單為買方 */ {
    while (!seller_tree.empty() && new_order.volume) {
      auto seller_price_level = seller_tree.begin();
      // 成交
      if (seller_price_level->price <= new_order.price) {
        Order& seller_order = seller_price_level->front();

        // 判斷成交量
        uint32_t make_deal_volume;
        make_deal_volume = std::min(new_order.volume, seller_order.volume);
        seller_price_level->total_volume -= make_deal_volume;
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
    if (new_order.time_in_force != Time_In_Force::IOC && new_order.volume > 0) {
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
    while (!buyer_tree.empty() && new_order.volume) {
      auto buyer_price_level = buyer_tree.begin();
      // 成交
      if (buyer_price_level->price >= new_order.price) {
        Order& buyer_order = buyer_price_level->front();

        // 判斷成交量
        uint32_t make_deal_volume;
        make_deal_volume = std::min(new_order.volume, buyer_order.volume);
        buyer_price_level->total_volume -= make_deal_volume;
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
    if (new_order.time_in_force != Time_In_Force::IOC && new_order.volume > 0) {
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

void L3_LOB::place_order(Order new_order) { order_matching(new_order); }

void L3_LOB::cancel_order(uint32_t tar_idx) {
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