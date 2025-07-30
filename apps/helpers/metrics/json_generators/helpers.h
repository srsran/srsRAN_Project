/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/slot_point.h"
#include "fmt/chrono.h"
#include <chrono>
#include <cmath>

namespace srsran {

inline void to_json(nlohmann::json& json, const slot_point& slot)
{
  json = fmt::format("{}", slot);
}

namespace app_helpers {
namespace json_generators {

/// Return the given value if it is not a Nan or Inf, otherwise returns 0.
inline double validate_fp_value(double value)
{
  if (!std::isnan(value) && !std::isinf(value)) {
    return value;
  }
  return 0.0;
}

} // namespace json_generators
} // namespace app_helpers
} // namespace srsran
