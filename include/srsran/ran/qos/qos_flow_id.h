/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "fmt/format.h"
#include <cstdint>

namespace srsran {

// See TS 38.463 Section 9.3.1.24: QoS Flow ID valid values: (0..63)
static constexpr uint8_t MAX_NOF_QOS_FLOWS = 64;

/// \brief QoS Flow ID.
/// \remark See TS 38.463 Section 9.3.1.21: QoS Flow ID valid values: (0..63)
enum class qos_flow_id_t : uint8_t { min = 0, max = MAX_NOF_QOS_FLOWS - 1, invalid = MAX_NOF_QOS_FLOWS };

/// Convert QoS Flow ID type to integer.
constexpr inline uint8_t qos_flow_id_to_uint(qos_flow_id_t id)
{
  return static_cast<uint8_t>(id);
}

/// Convert integer to QoS Flow ID type.
constexpr inline qos_flow_id_t uint_to_qos_flow_id(uint8_t idx)
{
  return static_cast<qos_flow_id_t>(idx);
}

} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::qos_flow_id_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::qos_flow_id_t& qfi, FormatContext& ctx)
  {
    switch (qfi) {
      case srsran::qos_flow_id_t::invalid:
        return format_to(ctx.out(), "invalid QFI");
      default:
        return format_to(ctx.out(), "QFI={:#}", qos_flow_id_to_uint(qfi));
    }
  }
};

} // namespace fmt
