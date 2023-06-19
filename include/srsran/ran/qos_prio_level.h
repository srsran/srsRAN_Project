/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/support/srsran_assert.h"
#include "fmt/format.h"

namespace srsran {

// See TS 38.473: QoS Priority Level valid values: (0..127)
constexpr static uint8_t MAX_QOS_PRIO_LEVEL = 127;

/// \brief QoS Priority Level.
enum class qos_prio_level_t : uint8_t { min = 0, max = MAX_QOS_PRIO_LEVEL, invalid = MAX_QOS_PRIO_LEVEL + 1 };

/// Convert QoS Priority Level type to integer.
constexpr inline uint8_t qos_prio_level_to_uint(qos_prio_level_t qos_prio_level)
{
  return static_cast<uint8_t>(qos_prio_level);
}

/// Convert integer to QoS Priority Level type.
constexpr inline qos_prio_level_t uint_to_qos_prio_level(uint8_t qos_prio_level)
{
  srsran_assert(qos_prio_level < MAX_QOS_PRIO_LEVEL,
                "Invalid QoS Priority Level {} - Must be between [{}..{}]",
                qos_prio_level,
                qos_prio_level_t::min,
                qos_prio_level_t::max);
  return static_cast<qos_prio_level_t>(qos_prio_level);
}

} // namespace srsran

// Formatters
namespace fmt {
template <>
struct formatter<srsran::qos_prio_level_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::qos_prio_level_t& qos_prio_level, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "QoS Priority Level={:#x}", qos_prio_level_to_uint(qos_prio_level));
  }
};

} // namespace fmt
