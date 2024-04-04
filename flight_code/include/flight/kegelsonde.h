#include <concepts>
#include <variant>
#include <optional>
#include "core/core.h"

#ifndef FLIGHT_CODE_INCLUDE_FLIGHT_ADDITIONAL_PRESSURE_H_
#define FLIGHT_CODE_INCLUDE_FLIGHT_ADDITIONAL_PRESSURE_H_

namespace bfs {

struct KegelsondeConfig {
  int8_t dev;
  std::optional<int8_t> transducer;
  int16_t sampling_period_ms;
  std::variant<TwoWire *, SPIClass *> bus;
};
struct KegelsondeData {
  bool new_data;
  bool healthy;
  std::vector<float> pres_mbar;
};



}  // namespace bfs

#endif