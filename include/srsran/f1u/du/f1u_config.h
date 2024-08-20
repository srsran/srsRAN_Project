/*
 *
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

namespace srsran::srs_du {

/// \brief Configurable parameters of the F1-U bearer in the DU
struct f1u_config {
  uint32_t t_notify;              ///< Timer used for periodic transmission of uplink notifications
  uint32_t rlc_queue_bytes_limit; ///< RLC queue limit in bytes. Used for initial report of buffer space to the CU-UP.
  bool     warn_on_drop = true;   ///< Log a warning instead of an info message whenever a PDU is dropped

  bool operator==(const f1u_config& other) const
  {
    return t_notify == other.t_notify and rlc_queue_bytes_limit == other.rlc_queue_bytes_limit and
           warn_on_drop == other.warn_on_drop;
  }
};

} // namespace srsran::srs_du

namespace fmt {

// F1-U config formatter
template <>
struct formatter<srsran::srs_du::f1u_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::srs_du::f1u_config cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "t_notify={} rlc_queue_bytes_limit={} warn_on_drop={}",
                     cfg.t_notify,
                     cfg.rlc_queue_bytes_limit,
                     cfg.warn_on_drop);
  }
};

} // namespace fmt
