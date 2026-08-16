#ifndef __LOB_COMMON_HPP__
#define __LOB_COMMON_HPP__

#include <cstdint>
#include <ostream>
#include <limits>

static constexpr uint32_t IDX_NO_VALUE = std::numeric_limits<uint32_t>::max();
static constexpr uint64_t VOLUME_NO_VALUE =
    std::numeric_limits<uint64_t>::max();
static constexpr double PRICE_NO_VALUE = std::numeric_limits<double>::max();

/*****************************************************************************
 * is_valid function: 用於判斷回傳值是否為真                                 *
 *****************************************************************************/
inline bool is_valid_idx(uint32_t idx) { return idx != IDX_NO_VALUE; }
inline bool is_valid_volume(uint64_t volume) {
  return volume != VOLUME_NO_VALUE;
}
inline bool is_valid_price(double price) { return price != PRICE_NO_VALUE; }

/*****************************************************************************
 * Side: 標記該筆訂單為買方還是賣方                                          *
 * Time_In_Force: 標記該筆訂單的 time in force 種類                          *
 *****************************************************************************/
enum class Side : uint8_t { BUY, SELL };
enum class Time_In_Force : uint8_t { GTC, IOC, FOK };

inline std::ostream& operator<<(std::ostream& os, const Side& side) {
  if (side == Side::BUY) {
    os << "BUY";
  } else {
    os << "SELL";
  }

  return os;
}

/*****************************************************************************
 * PriceLevelInfo: 用於回傳資訊                                              *
 *****************************************************************************/
struct PriceLevelInfo {
  double price;
  uint64_t volume;

  PriceLevelInfo(double p, uint64_t vol) : price(p), volume(vol) {};

  friend bool operator==(const PriceLevelInfo& a, const PriceLevelInfo& b);
};

inline bool operator==(const PriceLevelInfo& a, const PriceLevelInfo& b) {
  return a.price == b.price && a.volume == b.volume;
}

#endif
