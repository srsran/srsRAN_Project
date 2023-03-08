/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

/// PDCP NR SRB or DRB information.
enum class pdcp_rb_type { srb, drb };

/// PDCP NR RLC mode information.
enum class pdcp_rlc_mode { um, am };

/// PDCP NR sequence number field.
enum class pdcp_sn_size : uint8_t { size12bits = 12, size18bits = 18 };
inline bool pdcp_sn_size_from_uint(pdcp_sn_size& sn_size, uint16_t num)
{
  if (num == 12) {
    sn_size = pdcp_sn_size::size12bits;
    return true;
  }
  if (num == 18) {
    sn_size = pdcp_sn_size::size18bits;
    return true;
  }
  return false;
}

/// \brief Convert PDCP SN size from enum to unsigned integer.
constexpr uint8_t pdcp_sn_size_to_uint(pdcp_sn_size sn_size)
{
  return static_cast<uint8_t>(sn_size);
}

/// Maximum supported PDCP SDU size, see TS 38.323, section 4.3.1.
constexpr uint16_t pdcp_max_sdu_size = 9000;

/// PDCP security direction
enum class pdcp_security_direction { uplink, downlink };

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
inline bool pdcp_t_reordering_from_int(pdcp_t_reordering t_reord, int num)
{
  switch (num) {
    case 0:
    case 1:
    case 2:
    case 4:
    case 5:
    case 8:
    case 10:
    case 15:
    case 20:
    case 30:
    case 40:
    case 50:
    case 60:
    case 80:
    case 100:
    case 120:
    case 140:
    case 160:
    case 180:
    case 200:
    case 220:
    case 240:
    case 260:
    case 280:
    case 300:
    case 500:
    case 750:
    case 1000:
    case 1250:
    case 1500:
    case 1750:
    case 2000:
    case 2250:
    case 2500:
    case 2750:
    case 3000:
    case -1:
      t_reord = static_cast<pdcp_t_reordering>(num);
      return true;
    default:
      return false;
  }
}

/// \brief Convert PDCP NR t-Reordering from enum to integer.
constexpr uint8_t pdcp_t_reordering_to_int(pdcp_t_reordering t_reordering)
{
  return static_cast<int16_t>(t_reordering);
}

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
inline bool pdcp_discard_timer_from_int(pdcp_discard_timer& discard_timer, int num)
{
  switch (num) {
    case 0:
    case 10:
    case 20:
    case 30:
    case 40:
    case 50:
    case 60:
    case 75:
    case 100:
    case 150:
    case 200:
    case 250:
    case 300:
    case 500:
    case 750:
    case 1500:
    case -1:
      discard_timer = static_cast<pdcp_discard_timer>(num);
      return true;
    default:
      return false;
  }
}

/// \brief Convert PDCP NR discard timer from enum to integer.
constexpr uint8_t pdcp_discard_timer_to_int(pdcp_discard_timer discard_timer)
{
  return static_cast<int16_t>(discard_timer);
}

/// The PDCP cannot re-use COUNTs, see TS 38.331, section 5.3.1.2.
/// To avoid this, we define two thresholds, one where we accept messages
/// but notify the RRC and another where we no longer accept messages.
/// Here, we define some default values for this, both for TX and RX.
const uint32_t pdcp_tx_default_max_count_notify = 0xc0000000;
const uint32_t pdcp_tx_default_max_count_hard   = UINT32_MAX - 1;
const uint32_t pdcp_rx_default_max_count_notify = 0xc0000000;
const uint32_t pdcp_rx_default_max_count_hard   = 0xd0000000;
struct pdcp_max_count {
  uint32_t notify;
  uint32_t hard;
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
    pdcp_max_count     max_count = {pdcp_tx_default_max_count_notify, pdcp_tx_default_max_count_hard};
  } tx;
  struct pdcp_rx_config : pdcp_config_common {
    bool              out_of_order_delivery;
    pdcp_t_reordering t_reordering;
    pdcp_max_count    max_count = {pdcp_rx_default_max_count_notify, pdcp_rx_default_max_count_hard};
  } rx;
};

/// \brief Make default SRB parameters for PDCP
/// Ref: 3GPP TS 38.331, section 9.2.1
inline pdcp_config pdcp_make_default_srb_config()
{
  pdcp_config config = {};
  // common TX/RX parameters
  config.tx.rb_type                       = pdcp_rb_type::srb;
  config.rx.rb_type                       = pdcp_rb_type::srb;
  config.tx.rlc_mode                      = pdcp_rlc_mode::am;
  config.rx.rlc_mode                      = pdcp_rlc_mode::am;
  config.tx.sn_size                       = pdcp_sn_size::size12bits;
  config.rx.sn_size                       = pdcp_sn_size::size12bits;
  config.tx.direction                     = pdcp_security_direction::downlink;
  config.rx.direction                     = pdcp_security_direction::uplink;
  config.tx.integrity_protection_required = true;
  config.rx.integrity_protection_required = true;
  config.tx.ciphering_required            = true;
  config.rx.ciphering_required            = true;

  // Tx config
  config.tx.discard_timer          = pdcp_discard_timer::not_configured;
  config.tx.status_report_required = false;

  // Tx config
  config.rx.out_of_order_delivery = false;
  config.rx.t_reordering          = pdcp_t_reordering::infinity;

  return config;
}
} // namespace srsran

//
// Formatters
//
namespace fmt {

// SN size
template <>
struct formatter<srsran::pdcp_sn_size> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::pdcp_sn_size sn_size, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", pdcp_sn_size_to_uint(sn_size));
  }
};

// RB type
template <>
struct formatter<srsran::pdcp_rb_type> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::pdcp_rb_type type, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"SRB", "DRB"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(type)]);
  }
};

// RLC mode
template <>
struct formatter<srsran::pdcp_rlc_mode> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::pdcp_rlc_mode mode, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"UM", "AM"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(mode)]);
  }
};

// TX config
template <>
struct formatter<srsran::pdcp_config::pdcp_tx_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::pdcp_config::pdcp_tx_config cfg, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "rb_type={} rlc_mode={} sn_size={} discard_timer={} count_notify={} count_max={}",
                     cfg.rb_type,
                     cfg.rlc_mode,
                     cfg.sn_size,
                     cfg.discard_timer,
                     cfg.max_count.notify,
                     cfg.max_count.hard);
  }
};

// RX config
template <>
struct formatter<srsran::pdcp_config::pdcp_rx_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::pdcp_config::pdcp_rx_config cfg, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "rb_type={} rlc_mode={} sn_size={} t_reordering={} count_notify={} count_max={}",
                     cfg.rb_type,
                     cfg.rlc_mode,
                     cfg.sn_size,
                     cfg.t_reordering,
                     cfg.max_count.notify,
                     cfg.max_count.hard);
  }
};

// PDCP config
template <>
struct formatter<srsran::pdcp_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::pdcp_config& cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{} {}", cfg.tx, cfg.rx);
  }
};
} // namespace fmt
