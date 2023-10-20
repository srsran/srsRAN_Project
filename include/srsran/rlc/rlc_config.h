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

#include "srsran/support/srsran_assert.h"
#include "srsran/support/timers.h"
#include "fmt/format.h"
#include <cstdint>
#include <memory>
#include <string>

namespace srsran {

/// RLC NR modes
enum class rlc_mode { tm, um_bidir, um_unidir_ul, um_unidir_dl, am };
inline bool from_string(rlc_mode& mode, const std::string& str)
{
  if (str == "am") {
    mode = rlc_mode::am;
    return true;
  }
  if (str == "um-bidir") {
    mode = rlc_mode::um_bidir;
    return true;
  }
  if (str == "um-unidir-ul") {
    mode = rlc_mode::um_unidir_ul;
    return true;
  }
  if (str == "um-unidir-dl") {
    mode = rlc_mode::um_unidir_dl;
    return true;
  }
  if (str == "tm") {
    mode = rlc_mode::tm;
    return true;
  }
  return false;
}

/// RLC UM NR sequence number field
enum class rlc_um_sn_size : uint16_t { size6bits = 6, size12bits = 12 };
constexpr uint16_t to_number(rlc_um_sn_size sn_size)
{
  return static_cast<uint16_t>(sn_size);
}
inline bool from_number(rlc_um_sn_size& sn_size, uint16_t num)
{
  if (num == 6) {
    sn_size = rlc_um_sn_size::size6bits;
    return true;
  }
  if (num == 12) {
    sn_size = rlc_um_sn_size::size12bits;
    return true;
  }
  return false;
}

/// RLC AM NR sequence number field
enum class rlc_am_sn_size : uint16_t { size12bits = 12, size18bits = 18 };

/// \brief Converts sequence number field to numeric its value
/// \param sn_size sequence number size
/// \return numeric value of the sequence number field
constexpr uint16_t to_number(rlc_am_sn_size sn_size)
{
  return static_cast<uint16_t>(sn_size);
}
inline bool from_number(rlc_am_sn_size& sn_size, uint16_t num)
{
  if (num == 12) {
    sn_size = rlc_am_sn_size::size12bits;
    return true;
  }
  if (num == 18) {
    sn_size = rlc_am_sn_size::size18bits;
    return true;
  }
  return false;
}

/// \brief Returns the value range of the sequence numbers
/// \param sn_size Length of the sequence number field in bits
/// \return cardinality of sn_size
constexpr uint32_t cardinality(uint16_t sn_size)
{
  srsran_assert(sn_size < 32, "Cardinality of sn_size={} exceeds return type 'uint32_t'", sn_size);
  return (1 << sn_size);
}

/// \brief Returns the UM_Window_Size and AM_Window_Size
/// Ref: 3GPP TS 38.322 Sec. 7.2
/// \param sn_size Length of the sequence number field in bits
/// \return size of the window
constexpr uint32_t window_size(uint16_t sn_size)
{
  return cardinality(sn_size - 1);
}

/// RLC AM NR segmentation info
enum class rlc_dc_field : unsigned { control = 0b00, data = 0b01 };
constexpr unsigned to_number(rlc_dc_field dc)
{
  return static_cast<unsigned>(dc);
}

/// RLC AM NR segmentation info
enum class rlc_si_field : unsigned {
  full_sdu       = 0b00, ///< Full SDU
  first_segment  = 0b01, ///< First SDU segment
  last_segment   = 0b10, ///< Last SDU segment
  middle_segment = 0b11  ///< Middle SDU segment
};

constexpr uint16_t to_number(rlc_si_field si_field)
{
  return static_cast<uint16_t>(si_field);
}

enum class rlc_control_pdu_type : unsigned { status_pdu = 0b000 };
constexpr uint16_t to_number(rlc_control_pdu_type type)
{
  return static_cast<uint16_t>(type);
}

/// \brief Configurable Rx parameters for RLC AM
/// Ref: 3GPP TS 38.322 Section 7
struct rlc_rx_am_config {
  rlc_am_sn_size sn_field_length; ///< Number of bits used for sequence number

  // Timers Ref: 3GPP TS 38.322 Section 7.3
  int32_t t_reassembly;      ///< Timer used by rx to detect PDU loss (ms)
  int32_t t_status_prohibit; ///< Timer used by rx to prohibit tx of status PDU (ms)
};

/// \brief Configurable Tx parameters for RLC AM
/// Ref: 3GPP TS 38.322 Section 7
struct rlc_tx_am_config {
  rlc_am_sn_size sn_field_length; ///< Number of bits used for sequence number

  // Timers Ref: 3GPP TS 38.322 Section 7.3
  int32_t t_poll_retx; ///< Poll retx timeout (ms)

  // Configurable Parameters. Ref: 3GPP TS 38.322 Section 7.4
  uint32_t max_retx_thresh; ///< Max number of retx
  int32_t  poll_pdu;        ///< Insert poll bit after this many PDUs
  int32_t  poll_byte;       ///< Insert poll bit after this much data (bytes)

  // Custom non-standard parameters
  uint32_t queue_size; ///< SDU queue size
  uint32_t max_window; ///< Custom parameter to limit the maximum window size for memory reasons. 0 means no limit.
};

/// \brief Configurable parameters for RLC AM
/// Ref: 3GPP TS 38.322 Section 7
struct rlc_am_config {
  rlc_rx_am_config rx; ///< Rx configuration
  rlc_tx_am_config tx; ///< Tx configuration
};

/// \brief Configurable Rx parameters for RLC UM
/// Ref: 3GPP TS 38.322 v15.3.0 Section 7
struct rlc_rx_um_config {
  rlc_um_sn_size sn_field_length; ///< Number of bits used for sequence number
  int32_t        t_reassembly;    ///< Timer used by rx to detect PDU loss (ms)
};

/// \brief Configurable Tx parameters for RLC UM
/// Ref: 3GPP TS 38.322 v15.3.0 Section 7
struct rlc_tx_um_config {
  rlc_um_sn_size sn_field_length; ///< Number of bits used for sequence number
  uint32_t       queue_size;      ///< SDU queue size
};

/// \brief Configurable parameters for RLC UM
/// Ref: 3GPP TS 38.322 v15.3.0 Section 7
struct rlc_um_config {
  rlc_rx_um_config rx; ///< Rx configuration
  rlc_tx_um_config tx; ///< Tx configuration
};

/// Configuration of RLC bearer.
struct rlc_config {
  rlc_mode       mode;
  rlc_um_config  um;
  rlc_am_config  am;
  timer_duration metrics_period;
};

} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::rlc_mode> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rlc_mode mode, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"TM", "UM Bi-dir", "UM Uni-dir-UL", "UM Uni-dir-DL", "AM"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(mode)]);
  }
};

template <>
struct formatter<srsran::rlc_um_sn_size> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rlc_um_sn_size sn_size, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", to_number(sn_size));
  }
};

template <>
struct formatter<srsran::rlc_am_sn_size> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rlc_am_sn_size sn_size, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", to_number(sn_size));
  }
};

template <>
struct formatter<srsran::rlc_dc_field> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rlc_dc_field dc, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"ctrl", "data"};
    return format_to(ctx.out(), "{}", options[to_number(dc)]);
  }
};

template <>
struct formatter<srsran::rlc_si_field> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rlc_si_field si, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"full", "first", "last", "mid"};
    return format_to(ctx.out(), "{}", options[to_number(si)]);
  }
};

template <>
struct formatter<srsran::rlc_control_pdu_type> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rlc_control_pdu_type cpt, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", to_number(cpt));
  }
};

// RLC UM TX config formatter
template <>
struct formatter<srsran::rlc_tx_um_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rlc_tx_um_config cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "tx_sn_size={}, queue_size={}", cfg.sn_field_length, cfg.queue_size);
  }
};

// RLC UM RX config formatter
template <>
struct formatter<srsran::rlc_rx_um_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rlc_rx_um_config cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "rx_sn_size={} t_reassembly={}", cfg.sn_field_length, cfg.t_reassembly);
  }
};

// RLC UM config formatter
template <>
struct formatter<srsran::rlc_um_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rlc_um_config cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{} {}", cfg.tx, cfg.rx);
  }
};

// RLC AM TX config formatter
template <>
struct formatter<srsran::rlc_tx_am_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rlc_tx_am_config cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "tx_sn_size={} t_poll_retx={} max_retx={} poll_pdu={} poll_byte={} queue_size={} max_window={}",
                     cfg.sn_field_length,
                     cfg.t_poll_retx,
                     cfg.max_retx_thresh,
                     cfg.poll_pdu,
                     cfg.poll_byte,
                     cfg.queue_size,
                     cfg.max_window);
  }
};

// RLC AM RX config formatter
template <>
struct formatter<srsran::rlc_rx_am_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rlc_rx_am_config cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "rx_sn_size={} t_reassembly={} t_status_prohibit={}",
                     cfg.sn_field_length,
                     cfg.t_reassembly,
                     cfg.t_status_prohibit);
  }
};

// RLC AM config formatter
template <>
struct formatter<srsran::rlc_am_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rlc_am_config cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{} {}", cfg.tx, cfg.rx);
  }
};

// RLC config formatter
template <>
struct formatter<srsran::rlc_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rlc_config cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    if (cfg.mode == srsran::rlc_mode::um_bidir) {
      return format_to(ctx.out(), "{} {}", cfg.mode, cfg.um);
    }
    if (cfg.mode == srsran::rlc_mode::am) {
      return format_to(ctx.out(), "{} {}", cfg.mode, cfg.am);
    }
    return format_to(ctx.out(), "unhandled mode={}", cfg.mode);
  }
};
} // namespace fmt
