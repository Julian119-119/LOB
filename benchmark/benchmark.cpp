#include <chrono>
#include <iostream>
#include <random>
#include <variant>
#include <vector>

#include "LOB_type.hpp"
#include "test_helper.hpp"

const double stock_price = 100.0;
const double tick_size = 0.5;
const long test_data_size = 10'000'000;
const int print_point_num = 30;

struct Cancel {
  uint32_t id;
  Cancel(uint32_t new_id) : id(new_id) {}
};

struct Place {
  Order order;
  Place(Order new_order) : order(new_order) {}
};

// 使用 std::variant 儲存不同的訂單資訊
using Behavior = std::variant<Place, Cancel>;

// 使用 visit 時，必須有的 templete
template<class... Ts>
struct overloads : Ts... { using Ts::operator()...; };

template<class... Ts>
overloads(Ts...) -> overloads<Ts...>;

void testspend() {
  std::cout << "\n========== testing in progress ==========\n" << std::flush;
  std::random_device rd;
  std::mt19937 gen(rd());

  // 設定分佈狀況
  // 假設 price 的 mean 為 100，sd 為 0.3
  std::normal_distribution<double> price_norm_dist{stock_price, 0.3};
  // 購買的量 （volume）
  std::uniform_int_distribution<uint64_t> volume_unif_dist{1, 1000};
  // 買賣方向
  std::uniform_int_distribution<int> side_code{0, 1};
  uint32_t user_id = 1;
  // timestamp
  uint64_t ts = 1779600968;
  // 隨機產生要放單或徹單，如果值大於 80 就徹單，否則放單
  std::uniform_int_distribution<int> place_or_cancel_dist{1, 100};
  // 在儲存 id 的 vector 中找尋要刪除的 id
  std::uniform_int_distribution<uint32_t> id_dist;

  // 模擬市場流動
  LOB book;
  std::vector<uint32_t> all_id;
  all_id.reserve(test_data_size);

  // 儲存指令與訂單的 vector
  std::vector<Behavior> behavior;
  behavior.reserve(test_data_size);

  // 建構隨機訂單
  std::cout << "\nGenerating random orders\n" << std::flush;
  for (long i = 0; i < test_data_size; i++) {
    if (all_id.empty() || place_or_cancel_dist(gen) <= 80) /* 下單 */ {
      // 產生訂單的資訊
      uint32_t order_id = user_id++;
      double price = round(price_norm_dist(gen) / tick_size) * tick_size;
      Side side = side_code(gen) ? Side::BUY : Side::SELL;
      uint64_t timestamp = ts++;
      uint64_t volume = volume_unif_dist(gen);

      // 建置訂單並且將其 push 進 behavior 中
      Order new_order(order_id, side, price, timestamp, volume);
      all_id.push_back(order_id);
      behavior.emplace_back(Place{new_order});
    } else /* 徹單 */ {
      // 拿到隨機產生的 idx
      uint32_t idx =
          id_dist(gen, decltype(id_dist)::param_type(0, all_id.size() - 1));
      // 將要刪除的 order_id 放進 behavior 中
      behavior.emplace_back(Cancel{idx});
      // 將最後一個元素往前塞
      all_id[idx] = all_id.back();
      all_id.pop_back();
    }
    if (i % (test_data_size / print_point_num) == 0) {
      std::cout << '.' << std::flush;
    }
  }
  std::cout << '\n';

  const auto visitor =
      overloads{[&](const Place& p) { book.place_order(p.order); },
                [&](const Cancel& c) { book.cancel_order(c.id); }};

  // 開始測試時間
  std::cout << "\nStarting benchmark\n" << std::flush;
  auto start_time = std::chrono::high_resolution_clock::now();
  int i = 0;
  for (const Behavior& curr_behavior : behavior) {
    std::visit(visitor, curr_behavior);

    // 在 visit 時，實時印出 '.'
    if (i % (test_data_size / print_point_num) == 0) {
      std::cout << '.' << std::flush;
    }
    i++;
  }

  // 時間測試結束，並印出結果
  auto end_time = std::chrono::high_resolution_clock::now();
  std::cout << "\n\n================ outcome ================\n";
  auto duration_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
      end_time - start_time);
  std::cout << "\nTotal number of test data: " << test_data_size << '\n';
  std::cout << "Total spending time: " << duration_time.count() / 1'000'000
            << "ms\n";
  std::cout << "Average spending time: "
            << duration_time.count() / test_data_size << "ns\n";
}

int main() {
  testspend();

  return 0;
}