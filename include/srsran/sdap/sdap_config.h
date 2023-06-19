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

#include "fmt/format.h"
#include <cstdint>
#include <memory>
#include <string>

namespace srsran {

/// SDAP header options for UL
enum class sdap_hdr_ul_cfg { present = 0, absent };

/// SDAP header options for DL
enum class sdap_hdr_dl_cfg { present = 0, absent };

/// Configurable parameters for SDAP mapping
struct sdap_config {
  bool            default_drb = false;
  sdap_hdr_ul_cfg header_ul   = sdap_hdr_ul_cfg::absent;
  sdap_hdr_dl_cfg header_dl   = sdap_hdr_dl_cfg::absent;
};

} // namespace srsran

//
// Formatters
//
namespace fmt {

// Header config
template <>
struct formatter<srsran::sdap_hdr_ul_cfg> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::sdap_hdr_ul_cfg hdr_cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"present", "absent"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(hdr_cfg)]);
  }
};

// Header config
template <>
struct formatter<srsran::sdap_hdr_dl_cfg> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::sdap_hdr_dl_cfg hdr_cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"present", "absent"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(hdr_cfg)]);
  }
};

// SDAP config
template <>
struct formatter<srsran::sdap_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::sdap_config cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "default_drb={} hdr_ul={} hdr_dl={}", cfg.default_drb, cfg.header_ul, cfg.header_dl);
  }
};

} // namespace fmt
