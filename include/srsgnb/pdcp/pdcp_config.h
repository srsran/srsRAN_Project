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

/// PDCP NR SRB or DRB information.
enum class pdcp_rb_type { srb, drb };

/// PDCP NR RLC mode information.
enum class pdcp_rlc_mode { um, am };

/// PDCP NR sequence number field.
enum class pdcp_sn_size : uint16_t { size12bits = 12, size18bits = 18 };
constexpr uint16_t to_number(pdcp_sn_size sn_size)
{
  return static_cast<uint16_t>(sn_size);
}

/// Maximum supported PDCP SDU size, see TS 38.323, section 4.3.1.
constexpr uint16_t pdcp_max_sdu_size = 9000;

/// PDCP security direction
enum class pdcp_security_direction { uplink, downlink };

/// PDCP integrity/ciphering enabled
enum class pdcp_integrity_enabled { no, enabled };
enum class pdcp_ciphering_enabled { no, enabled };

/// PDCP NR t-Reordering timer values.
/// This timer is used to detect loss of PDCP Data PDUs.
/// See TS 38.322 for timer description and TS 38.331 for valid timer durations.
enum class pdcp_t_reordering {
  ms0      = 0,
  ms1      = 1,
  ms2      = 2,
  ms4      = 4,
  ms5      = 5,
  ms8      = 8,
  ms10     = 10,
  ms15     = 15,
  ms20     = 20,
  ms30     = 30,
  ms40     = 40,
  ms50     = 50,
  ms60     = 60,
  ms80     = 80,
  ms100    = 100,
  ms120    = 120,
  ms140    = 140,
  ms160    = 160,
  ms180    = 180,
  ms200    = 200,
  ms220    = 220,
  ms240    = 240,
  ms260    = 260,
  ms280    = 280,
  ms300    = 300,
  ms500    = 500,
  ms750    = 750,
  ms1000   = 1000,
  ms1250   = 1250,
  ms1500   = 1500,
  ms1750   = 1750,
  ms2000   = 2000,
  ms2250   = 2250,
  ms2500   = 2500,
  ms2750   = 2750,
  ms3000   = 3000,
  infinity = -1
};

/// PDCP NR discard timer values.
/// This timer is configured only for DRBs. In the transmitter,
/// a new timer is started upon reception of an SDU from upper layer.
/// See TS 38.322 for timer description and TS 38.331 for valid timer durations.
enum class pdcp_discard_timer {
  not_configured = 0,
  ms10           = 10,
  ms20           = 20,
  ms30           = 30,
  ms40           = 40,
  ms50           = 50,
  ms60           = 60,
  ms75           = 75,
  ms100          = 100,
  ms150          = 150,
  ms200          = 200,
  ms250          = 250,
  ms300          = 300,
  ms500          = 500,
  ms750          = 750,
  ms1500         = 1500,
  infinity       = -1
};

/// \brief Configurable parameters for PDCP that are common
/// for both TX and RX.
///
/// Ref: 3GPP TS 38.331 version 15.2.1
struct pdcp_config_common {
  pdcp_rb_type  rb_type;
  pdcp_rlc_mode rlc_mode;
  pdcp_sn_size  sn_size = pdcp_sn_size::size12bits; // TODO remove default initialization and force parameter selection
  pdcp_security_direction direction;
  bool                    integrity_protection_required;
  bool                    ciphering_required;
};

/// \brief Configurable parameters for PDCP
///
/// Parameters and valid values for them are taken from
/// the RRC-NR PDCP-Config Information Element.
///
/// Ref: 3GPP TS 38.331 version 15.2.1
struct pdcp_config {
  struct pdcp_tx_config : pdcp_config_common {
    pdcp_discard_timer discard_timer;
    bool               status_report_required;
  } tx;
  struct pdcp_rx_config : pdcp_config_common {
    bool              out_of_order_delivery;
    pdcp_t_reordering t_reordering;
  } rx;
};
} // namespace srsgnb

//
// Formatters
//
namespace fmt {

// RB type
template <>
struct formatter<srsgnb::pdcp_rb_type> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::pdcp_rb_type type, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"SRB", "DRB"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(type)]);
  }
};

// RLC mode
template <>
struct formatter<srsgnb::pdcp_rlc_mode> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::pdcp_rlc_mode mode, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"UM", "AM"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(mode)]);
  }
};

// TX config
template <>
struct formatter<srsgnb::pdcp_config::pdcp_tx_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::pdcp_config::pdcp_tx_config cfg, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "rb_type={}, rlc_mode={}, sn_size={}, discard_timer={}",
                     cfg.rb_type,
                     cfg.rlc_mode,
                     cfg.sn_size,
                     cfg.discard_timer);
  }
};

// RX config
template <>
struct formatter<srsgnb::pdcp_config::pdcp_rx_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::pdcp_config::pdcp_rx_config cfg, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "rb_type={}, rlc_mode={}, sn_size={}, t_reordering={}",
                     cfg.rb_type,
                     cfg.rlc_mode,
                     cfg.sn_size,
                     cfg.t_reordering);
  }
};
} // namespace fmt
