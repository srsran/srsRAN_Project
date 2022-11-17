/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/support/srsgnb_assert.h"
#include <cstdint>
#include <memory>
#include <string>

namespace srsgnb {

/// \brief Configurable parameters for PDCP
///
/// Parameters and valid values for them are taken from
/// the RRC-NR PDCP-Config Information Element.
///
/// Ref: 3GPP TS 38.331 version 15.2.1
struct gtpu_config {
  struct gtpu_dl_config {
    uint32_t src_teid;
  } dl;
  struct gtpu_ul_config {
    uint32_t dst_teid;
  } ul;
};
} // namespace srsgnb

//
// Formatters
//
namespace fmt {

// DL config
template <>
struct formatter<srsgnb::gtpu_config::gtpu_dl_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::gtpu_config::gtpu_dl_config& cfg, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "src_teid={}", cfg.src_teid);
  }
};

// UL config
template <>
struct formatter<srsgnb::gtpu_config::gtpu_ul_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::gtpu_config::gtpu_ul_config& cfg, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "dst_teid={}", cfg.dst_teid);
  }
};

// GTP-U config
template <>
struct formatter<srsgnb::gtpu_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::gtpu_config& cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "dl=[{}], ul=[{}]", cfg.dl, cfg.ul);
  }
};
} // namespace fmt
