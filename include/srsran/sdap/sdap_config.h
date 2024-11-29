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
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::sdap_hdr_ul_cfg hdr_cfg, FormatContext& ctx) const
  {
    static constexpr const char* options[] = {"present", "absent"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(hdr_cfg)]);
  }
};

// Header config
template <>
struct formatter<srsran::sdap_hdr_dl_cfg> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::sdap_hdr_dl_cfg hdr_cfg, FormatContext& ctx) const
  {
    static constexpr const char* options[] = {"present", "absent"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(hdr_cfg)]);
  }
};

// SDAP config
template <>
struct formatter<srsran::sdap_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::sdap_config cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "default_drb={} hdr_ul={} hdr_dl={}", cfg.default_drb, cfg.header_ul, cfg.header_dl);
  }
};

} // namespace fmt
