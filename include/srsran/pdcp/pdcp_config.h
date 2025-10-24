/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/pdcp/pdcp_sn_size.h"
#include "srsran/pdcp/pdcp_t_reordering.h"
#include "srsran/support/timers.h"
#include "fmt/std.h"
#include <cstdint>

namespace srsran {

class pdcp_metrics_notifier;

/// PDCP NR SRB or DRB information.
enum class pdcp_rb_type { srb, drb };

/// PDCP NR RLC mode information.
enum class pdcp_rlc_mode { um, am };

/// \brief Returns the value range of the sequence numbers.
/// \param sn_size Length of the sequence number field in bits.
/// \return cardinality of sn_size.
constexpr uint32_t pcdp_sn_cardinality(uint16_t sn_size)
{
  srsran_assert(sn_size < 32, "Cardinality of sn_size={} exceeds return type 'uint32_t'", sn_size);
  return (1 << sn_size);
}

/// \brief Returns the PDCP window size.
/// \param sn_size Length of the sequence number field in bits.
/// \return size of the window.
constexpr uint32_t pdcp_window_size(uint16_t sn_size)
{
  return pcdp_sn_cardinality(sn_size - 1);
}

/// \brief Returns the PDCP header size.
/// \param sn_size Length of the sequence number field in bits.
/// \return size of the data PDU header.
constexpr uint32_t pdcp_data_header_size(pdcp_sn_size sn_size)
{
  return sn_size == pdcp_sn_size::size12bits ? 2 : 3;
}

/// \brief Returns the PDCP trailer size.
/// \param rb_type           whether this is a SRB or DRB.
/// \param integrity_enabled whether integrity is enabled or not.
/// \return size of the data trailer.
constexpr uint32_t pdcp_data_trailer_size(pdcp_rb_type rb_type, bool integrity_enabled)
{
  constexpr uint32_t mac_i_size = 4;
  if (rb_type == pdcp_rb_type::srb) {
    return mac_i_size;
  }
  if (integrity_enabled) {
    return mac_i_size;
  }
  return 0;
}

/// Maximum supported PDCP SDU size, see TS 38.323, section 4.3.1.
constexpr uint16_t pdcp_max_sdu_size = 9000;

/// PDCP security direction.
enum class pdcp_security_direction { uplink, downlink };

/// PDCP NR discard timer values.
/// This timer is configured only for DRBs. In the transmitter, a new timer is started upon reception of an SDU from
/// upper layer.
/// See TS 38.322 for timer description and TS 38.331 for valid timer durations.
enum class pdcp_discard_timer {
  ms10     = 10,
  ms20     = 20,
  ms30     = 30,
  ms40     = 40,
  ms50     = 50,
  ms60     = 60,
  ms75     = 75,
  ms100    = 100,
  ms150    = 150,
  ms200    = 200,
  ms250    = 250,
  ms300    = 300,
  ms500    = 500,
  ms750    = 750,
  ms1500   = 1500,
  infinity = -1
};

constexpr bool pdcp_discard_timer_from_int(pdcp_discard_timer& discard_timer, int num)
{
  switch (num) {
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

/// Convert PDCP NR discard timer from enum to integer.
constexpr int16_t pdcp_discard_timer_to_int(pdcp_discard_timer discard_timer)
{
  return static_cast<int16_t>(discard_timer);
}

/// Reordering timeout for serialization of TX PDUs after parallelized crypto operations.
constexpr uint32_t pdcp_tx_crypto_reordering_timeout_ms = 40;

/// The PDCP cannot re-use COUNTs, see TS 38.331, section 5.3.1.2.
/// To avoid this, we define two thresholds, one where we accept messages but notify the RRC and another where we no
/// longer accept messages. Here, we define some default values for this, both for TX and RX.
constexpr uint32_t pdcp_tx_default_max_count_notify = 0xc0000000;
constexpr uint32_t pdcp_tx_default_max_count_hard   = UINT32_MAX - 1;
constexpr uint32_t pdcp_rx_default_max_count_notify = 0xc0000000;
constexpr uint32_t pdcp_rx_default_max_count_hard   = 0xd0000000;
struct pdcp_max_count {
  uint32_t notify;
  uint32_t hard;
};

struct pdcp_custom_config_base {
  pdcp_max_count max_count = {pdcp_tx_default_max_count_notify, pdcp_tx_default_max_count_hard};
};

struct pdcp_custom_config_tx : public pdcp_custom_config_base {
  bool warn_on_drop = false;
  bool test_mode    = false;
};

struct pdcp_custom_config_rx : public pdcp_custom_config_base {
  // Empty
};

/// \brief Non-standard configurable parameters for PDCP.
///
/// Configurable parameters for the PDCP entity that are not explicitly specified in the RRC specifications. This
/// separation is necessary, as the CU-CP cannot transmit these parameters to the CU-UP, so it's necessary for the CU-UP
/// to store these configurations itself.
struct pdcp_custom_config {
  timer_duration         metrics_period;
  pdcp_metrics_notifier* metrics_notifier = nullptr;
  pdcp_custom_config_tx  tx               = {};
  pdcp_custom_config_rx  rx               = {};
};

/// \brief Configurable parameters for PDCP that are common for both TX and RX.
/// Ref: 3GPP TS 38.331 version 15.2.1.
struct pdcp_config_common {
  pdcp_rb_type            rb_type;
  pdcp_rlc_mode           rlc_mode;
  pdcp_sn_size            sn_size;
  pdcp_security_direction direction;
};

struct pdcp_tx_config : pdcp_config_common {
  std::optional<pdcp_discard_timer> discard_timer;
  bool                              status_report_required;
  pdcp_custom_config_tx             custom;
};

struct pdcp_rx_config : pdcp_config_common {
  bool                  out_of_order_delivery;
  pdcp_t_reordering     t_reordering;
  pdcp_custom_config_rx custom;
};

/// \brief Configurable parameters for PDCP.
///
/// Parameters and valid values for them are taken from the RRC-NR PDCP-Config Information Element.
/// Ref: 3GPP TS 38.331 version 15.2.1.
struct pdcp_config {
  pdcp_rb_type  rb_type;
  pdcp_rlc_mode rlc_mode;
  bool          integrity_protection_required;
  bool          ciphering_required;
  struct {
    pdcp_sn_size                      sn_size;
    pdcp_security_direction           direction;
    std::optional<pdcp_discard_timer> discard_timer;
    bool                              status_report_required;
  } tx;
  struct {
    pdcp_sn_size            sn_size;
    pdcp_security_direction direction;
    bool                    out_of_order_delivery;
    pdcp_t_reordering       t_reordering;
  } rx;
  pdcp_custom_config custom;

  pdcp_tx_config get_tx_config() const
  {
    pdcp_tx_config cfg;
    cfg.rb_type                = rb_type;
    cfg.rlc_mode               = rlc_mode;
    cfg.sn_size                = tx.sn_size;
    cfg.direction              = tx.direction;
    cfg.discard_timer          = tx.discard_timer;
    cfg.status_report_required = tx.status_report_required;
    cfg.custom                 = custom.tx;
    return cfg;
  }
  pdcp_rx_config get_rx_config() const
  {
    pdcp_rx_config cfg;
    cfg.rb_type               = rb_type;
    cfg.rlc_mode              = rlc_mode;
    cfg.sn_size               = rx.sn_size;
    cfg.direction             = rx.direction;
    cfg.out_of_order_delivery = rx.out_of_order_delivery;
    cfg.t_reordering          = rx.t_reordering;
    cfg.custom                = custom.rx;
    return cfg;
  }
};

/// \brief Make default SRB parameters for PDCP.
/// Ref: 3GPP TS 38.331, section 9.2.1.
inline pdcp_config pdcp_make_default_srb_config()
{
  pdcp_config config = {};
  // Common TX/RX parameters.
  config.rb_type                       = pdcp_rb_type::srb;
  config.rlc_mode                      = pdcp_rlc_mode::am;
  config.integrity_protection_required = true;
  config.ciphering_required            = true;

  // Tx config.
  config.tx.sn_size                = pdcp_sn_size::size12bits;
  config.tx.direction              = pdcp_security_direction::downlink;
  config.tx.status_report_required = false;
  config.tx.discard_timer          = pdcp_discard_timer::infinity;

  // Rx config.
  config.rx.sn_size               = pdcp_sn_size::size12bits;
  config.rx.direction             = pdcp_security_direction::uplink;
  config.rx.out_of_order_delivery = false;
  config.rx.t_reordering          = pdcp_t_reordering::infinity;

  // Custom config.
  config.custom = {};

  return config;
}

struct pdcp_count_info {
  uint32_t sn  = 0;
  uint32_t hfn = 0;
};

struct pdcp_sn_status_info {
  pdcp_count_info ul_count;
  pdcp_count_info dl_count;
};

} // namespace srsran

//
// Formatters
//
namespace fmt {

template <>
struct formatter<srsran::pdcp_rb_type> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::pdcp_rb_type type, FormatContext& ctx) const
  {
    static constexpr const char* options[] = {"SRB", "DRB"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(type)]);
  }
};

template <>
struct formatter<srsran::pdcp_rlc_mode> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::pdcp_rlc_mode mode, FormatContext& ctx) const
  {
    static constexpr const char* options[] = {"UM", "AM"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(mode)]);
  }
};

template <>
struct formatter<srsran::pdcp_t_reordering> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::pdcp_t_reordering t_reordering, FormatContext& ctx) const
  {
    if (t_reordering == srsran::pdcp_t_reordering::infinity) {
      return format_to(ctx.out(), "infinity");
    }
    return format_to(ctx.out(), "{}", srsran::pdcp_t_reordering_to_int(t_reordering));
  }
};

template <>
struct formatter<srsran::pdcp_discard_timer> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::pdcp_discard_timer discard_timer, FormatContext& ctx) const
  {
    if (discard_timer == srsran::pdcp_discard_timer::infinity) {
      return format_to(ctx.out(), "infinity");
    }
    return format_to(ctx.out(), "{}", srsran::pdcp_discard_timer_to_int(discard_timer));
  }
};

template <>
struct formatter<srsran::pdcp_custom_config_tx> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::pdcp_custom_config_tx cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "count_notify={} count_max={} warn_on_drop={} test_mode={}",
                     cfg.max_count.notify,
                     cfg.max_count.hard,
                     cfg.warn_on_drop,
                     cfg.test_mode);
  }
};

template <>
struct formatter<srsran::pdcp_custom_config_rx> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::pdcp_custom_config_rx cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "count_notify={} count_max={}", cfg.max_count.notify, cfg.max_count.hard);
  }
};

template <>
struct formatter<srsran::pdcp_tx_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::pdcp_tx_config& cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "rb_type={} rlc_mode={} sn_size={} discard_timer={} {}",
                     cfg.rb_type,
                     cfg.rlc_mode,
                     cfg.sn_size,
                     cfg.discard_timer,
                     cfg.custom);
  }
};

template <>
struct formatter<srsran::pdcp_rx_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::pdcp_rx_config& cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "rb_type={} rlc_mode={} sn_size={} t_reordering={} {}",
                     cfg.rb_type,
                     cfg.rlc_mode,
                     cfg.sn_size,
                     cfg.t_reordering,
                     cfg.custom);
  }
};

template <>
struct formatter<srsran::pdcp_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::pdcp_config& cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "rb_type={} rlc_mode={} int_req={} cip_req={} TX=[sn_size={} discard_timer={}] "
                     "RX=[sn_size={} t_reordering={} out_of_order={}] custom_tx=[{}] custom_rx=[{}]",
                     cfg.rb_type,
                     cfg.rlc_mode,
                     cfg.integrity_protection_required,
                     cfg.ciphering_required,
                     cfg.tx.sn_size,
                     cfg.tx.discard_timer,
                     cfg.rx.sn_size,
                     cfg.rx.t_reordering,
                     cfg.rx.out_of_order_delivery,
                     cfg.custom.tx,
                     cfg.custom.rx);
  }
};

} // namespace fmt
