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

#include "srsran/ran/tdd/tdd_ul_dl_config.h"
#include "srsran/support/format/fmt_basic_parser.h"
#include "fmt/std.h"

namespace fmt {

template <>
struct formatter<srsran::tdd_ul_dl_pattern> : public basic_parser {
  template <typename FormatContext>
  auto format(const srsran::tdd_ul_dl_pattern& cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "period={} dl_slots={} dl_symbols={} ul_slots={} ul_symbols={}",
                     cfg.dl_ul_tx_period_nof_slots,
                     cfg.nof_dl_slots,
                     cfg.nof_dl_symbols,
                     cfg.nof_ul_slots,
                     cfg.nof_ul_symbols);
  }
};

template <>
struct formatter<srsran::tdd_ul_dl_config_common> : public basic_parser {
  template <typename FormatContext>
  auto format(const srsran::tdd_ul_dl_config_common& cfg, FormatContext& ctx) const
  {
    return format_to(
        ctx.out(), "scs={} pattern1={{{}}} pattern2={{{}}}", fmt::underlying(cfg.ref_scs), cfg.pattern1, cfg.pattern2);
  }
};

} // namespace fmt
