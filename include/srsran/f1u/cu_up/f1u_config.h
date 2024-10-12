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

namespace srsran {
namespace srs_cu_up {

/// \brief Configurable parameters of the F1-U bearer in the CU-UP
struct f1u_config {
  bool warn_on_drop = true; ///< Log a warning instead of an info message whenever a PDU is dropped
};

} // namespace srs_cu_up
} // namespace srsran

namespace fmt {

// F1-U config formatter
template <>
struct formatter<srsran::srs_cu_up::f1u_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::srs_cu_up::f1u_config cfg, FormatContext& ctx)
  {
    return format_to(ctx.out(), "warn_on_drop={}", cfg.warn_on_drop);
  }
};

} // namespace fmt
