/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/ran/tdd/tdd_ul_dl_config.h"
#include "srsran/support/format_utils.h"

namespace fmt {

template <>
struct formatter<srsran::tdd_ul_dl_pattern> : public basic_fmt_parser {
  template <typename FormatContext>
  auto format(const srsran::tdd_ul_dl_pattern& cfg, FormatContext& ctx)
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
struct formatter<srsran::tdd_ul_dl_config_common> : public basic_fmt_parser {
  template <typename FormatContext>
  auto format(const srsran::tdd_ul_dl_config_common& cfg, FormatContext& ctx)
  {
    return format_to(ctx.out(), "scs={} pattern1={{{}}} pattern2={{{}}}", cfg.ref_scs, cfg.pattern1, cfg.pattern2);
  }
};

} // namespace fmt