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
#include <sys/socket.h>

namespace srsgnb {

/// \brief Configurable parameters for the GTP-U
struct gtpu_config {
  struct gtpu_rx_config {
    uint32_t local_teid;
  } rx;
  struct gtpu_tx_config {
    uint32_t         peer_teid;
    sockaddr_storage peer_addr;
  } tx;
};
} // namespace srsgnb

//
// Formatters
//
namespace fmt {

// RX config
template <>
struct formatter<srsgnb::gtpu_config::gtpu_rx_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::gtpu_config::gtpu_rx_config& cfg, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "local teid={}", cfg.local_teid);
  }
};

// TX config
template <>
struct formatter<srsgnb::gtpu_config::gtpu_tx_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::gtpu_config::gtpu_tx_config& cfg, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "peer teid={}", cfg.peer_teid);
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
    return format_to(ctx.out(), "rx=[{}], tx=[{}]", cfg.rx, cfg.tx);
  }
};
} // namespace fmt
