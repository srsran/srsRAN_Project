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

#include "srsran/adt/optional.h"
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

/// \brief Converts sequence number field to numeric its value
/// \param sn_size sequence number size
/// \return numeric value of the sequence number field
enum class rlc_t_reassembly {
  ms0   = 0,
  ms5   = 5,
  ms10  = 10,
  ms15  = 15,
  ms20  = 20,
  ms25  = 25,
  ms30  = 30,
  ms35  = 35,
  ms40  = 40,
  ms45  = 45,
  ms50  = 50,
  ms55  = 55,
  ms60  = 60,
  ms65  = 65,
  ms70  = 70,
  ms75  = 75,
  ms80  = 80,
  ms85  = 85,
  ms90  = 90,
  ms95  = 95,
  ms100 = 100,
  ms110 = 110,
  ms120 = 120,
  ms130 = 130,
  ms140 = 140,
  ms150 = 150,
  ms160 = 160,
  ms170 = 170,
  ms180 = 180,
  ms190 = 190,
  ms200 = 200
};
inline bool rlc_t_reassembly_from_int(rlc_t_reassembly& t_reassembly, int num)
{
  switch (num) {
    case 0:
    case 5:
    case 10:
    case 15:
    case 20:
    case 25:
    case 30:
    case 35:
    case 40:
    case 45:
    case 50:
    case 55:
    case 60:
    case 65:
    case 70:
    case 75:
    case 80:
    case 85:
    case 90:
    case 95:
    case 100:
    case 110:
    case 120:
    case 130:
    case 140:
    case 150:
    case 160:
    case 170:
    case 180:
    case 190:
    case 200:
      t_reassembly = static_cast<rlc_t_reassembly>(num);
      return true;
    default:
      return false;
  }
}

enum class rlc_t_poll_retransmit {
  ms5    = 5,
  ms10   = 10,
  ms15   = 15,
  ms20   = 20,
  ms25   = 25,
  ms30   = 30,
  ms35   = 35,
  ms40   = 40,
  ms45   = 45,
  ms50   = 50,
  ms55   = 55,
  ms60   = 60,
  ms65   = 65,
  ms70   = 70,
  ms75   = 75,
  ms80   = 80,
  ms85   = 85,
  ms90   = 90,
  ms95   = 95,
  ms100  = 100,
  ms105  = 105,
  ms110  = 110,
  ms115  = 115,
  ms120  = 120,
  ms125  = 125,
  ms130  = 130,
  ms135  = 135,
  ms140  = 140,
  ms145  = 145,
  ms150  = 150,
  ms155  = 155,
  ms160  = 160,
  ms165  = 165,
  ms170  = 170,
  ms175  = 175,
  ms180  = 180,
  ms185  = 185,
  ms190  = 190,
  ms195  = 195,
  ms200  = 200,
  ms205  = 205,
  ms210  = 210,
  ms215  = 215,
  ms220  = 220,
  ms225  = 225,
  ms230  = 230,
  ms235  = 235,
  ms240  = 240,
  ms245  = 245,
  ms250  = 250,
  ms300  = 300,
  ms350  = 350,
  ms400  = 400,
  ms450  = 450,
  ms500  = 500,
  ms800  = 800,
  ms1000 = 1000,
  ms2000 = 2000,
  ms4000 = 4000
};

inline bool rlc_t_poll_retransmit_from_int(rlc_t_poll_retransmit& t_poll_retransmit, int num)
{
  switch (num) {
    case 5:
    case 10:
    case 15:
    case 20:
    case 25:
    case 30:
    case 35:
    case 40:
    case 45:
    case 50:
    case 55:
    case 60:
    case 65:
    case 70:
    case 75:
    case 80:
    case 85:
    case 90:
    case 95:
    case 100:
    case 105:
    case 110:
    case 115:
    case 120:
    case 125:
    case 130:
    case 135:
    case 140:
    case 145:
    case 150:
    case 155:
    case 160:
    case 165:
    case 170:
    case 175:
    case 180:
    case 185:
    case 190:
    case 195:
    case 200:
    case 205:
    case 210:
    case 215:
    case 220:
    case 225:
    case 230:
    case 235:
    case 240:
    case 245:
    case 250:
    case 300:
    case 350:
    case 400:
    case 450:
    case 500:
    case 800:
    case 1000:
    case 2000:
    case 4000:
      t_poll_retransmit = static_cast<rlc_t_poll_retransmit>(num);
      return true;
    default:
      return false;
  }
}

enum class rlc_max_retx_threshold { t1 = 1, t2 = 2, t3 = 3, t4 = 4, t6 = 6, t8 = 8, t16 = 16, t32 = 32 };
inline bool rlc_max_retx_threshold_from_int(rlc_max_retx_threshold& max_retx_threshold, int num)
{
  switch (num) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 6:
    case 8:
    case 16:
    case 32:
      max_retx_threshold = static_cast<rlc_max_retx_threshold>(num);
      return true;
    default:
      return false;
  }
}

enum class rlc_poll_pdu {
  p4       = 4,
  p8       = 8,
  p16      = 16,
  p32      = 32,
  p64      = 64,
  p128     = 128,
  p256     = 256,
  p512     = 512,
  p1024    = 1024,
  p2048    = 2048,
  p4096    = 4096,
  p6144    = 6144,
  p8192    = 8192,
  p12288   = 12288,
  p16384   = 16384,
  p20480   = 20480,
  p24576   = 24576,
  p28672   = 28672,
  p32768   = 32768,
  p40960   = 40960,
  p49152   = 49152,
  p57344   = 57344,
  p65536   = 65536,
  infinity = -1
};
inline bool rlc_poll_pdu_from_int(rlc_poll_pdu& poll_pdu, int num)
{
  switch (num) {
    case 4:
    case 8:
    case 16:
    case 32:
    case 64:
    case 128:
    case 256:
    case 512:
    case 1024:
    case 2048:
    case 4096:
    case 6144:
    case 8192:
    case 12288:
    case 16384:
    case 20480:
    case 24576:
    case 28672:
    case 32768:
    case 40960:
    case 49152:
    case 57344:
    case 65536:
    case -1:
      poll_pdu = static_cast<rlc_poll_pdu>(num);
      return true;
    default:
      return false;
  }
}

enum class rlc_poll_kilo_bytes {
  kB1      = 1,
  kB2      = 2,
  kB5      = 5,
  kB8      = 8,
  kB10     = 10,
  kB15     = 15,
  kB25     = 25,
  kB50     = 50,
  kB75     = 75,
  kB100    = 100,
  kB125    = 125,
  kB250    = 250,
  kB375    = 375,
  kB500    = 500,
  kB750    = 750,
  kB1000   = 1000,
  kB1250   = 1250,
  kB1500   = 1500,
  kB2000   = 2000,
  kB3000   = 3000,
  kB4000   = 4000,
  kB4500   = 4500,
  kB5000   = 5000,
  kB5500   = 5500,
  kB6000   = 6000,
  kB6500   = 6500,
  kB7000   = 7000,
  kB7500   = 7500,
  mB8      = 8000,
  mB9      = 9000,
  mB10     = 10000,
  mB11     = 11000,
  mB12     = 12000,
  mB13     = 13000,
  mB14     = 14000,
  mB15     = 15000,
  mB16     = 16000,
  mB17     = 17000,
  mB18     = 18000,
  mB20     = 20000,
  mB25     = 25000,
  mB30     = 30000,
  mB40     = 40000,
  infinity = -1
};
inline bool rlc_poll_kilo_bytes_from_int(rlc_poll_kilo_bytes& poll_bytes, int num)
{
  switch (num) {
    case 1:
    case 2:
    case 5:
    case 8:
    case 10:
    case 15:
    case 25:
    case 50:
    case 75:
    case 100:
    case 125:
    case 250:
    case 375:
    case 500:
    case 750:
    case 1000:
    case 1250:
    case 1500:
    case 2000:
    case 3000:
    case 4000:
    case 4500:
    case 5000:
    case 5500:
    case 6000:
    case 6500:
    case 7000:
    case 7500:
    case 8000:
    case 9000:
    case 10000:
    case 11000:
    case 12000:
    case 13000:
    case 14000:
    case 15000:
    case 16000:
    case 17000:
    case 18000:
    case 20000:
    case 25000:
    case 30000:
    case 40000:
    case -1:
      poll_bytes = static_cast<rlc_poll_kilo_bytes>(num);
      return true;
    default:
      return false;
  }
}

enum class rlc_t_status_prohibit {
  ms0    = 0,
  ms5    = 5,
  ms10   = 10,
  ms15   = 15,
  ms20   = 20,
  ms25   = 25,
  ms30   = 30,
  ms35   = 35,
  ms40   = 40,
  ms45   = 45,
  ms50   = 50,
  ms55   = 55,
  ms60   = 60,
  ms65   = 65,
  ms70   = 70,
  ms75   = 75,
  ms80   = 80,
  ms85   = 85,
  ms90   = 90,
  ms95   = 95,
  ms100  = 100,
  ms105  = 105,
  ms110  = 110,
  ms115  = 115,
  ms120  = 120,
  ms125  = 125,
  ms130  = 130,
  ms135  = 135,
  ms140  = 140,
  ms145  = 145,
  ms150  = 150,
  ms155  = 155,
  ms160  = 160,
  ms165  = 165,
  ms170  = 170,
  ms175  = 175,
  ms180  = 180,
  ms185  = 185,
  ms190  = 190,
  ms195  = 195,
  ms200  = 200,
  ms205  = 205,
  ms210  = 210,
  ms215  = 215,
  ms220  = 220,
  ms225  = 225,
  ms230  = 230,
  ms235  = 235,
  ms240  = 240,
  ms245  = 245,
  ms250  = 250,
  ms300  = 300,
  ms350  = 350,
  ms400  = 400,
  ms450  = 450,
  ms500  = 500,
  ms800  = 800,
  ms1000 = 1000,
  ms1200 = 1200,
  ms1600 = 1600,
  ms2000 = 2000,
  ms2400 = 2400,
};
inline bool rlc_t_status_prohibit_from_int(rlc_t_status_prohibit& status_prohibit, int num)
{
  switch (num) {
    case 0:
    case 5:
    case 10:
    case 15:
    case 20:
    case 25:
    case 30:
    case 35:
    case 40:
    case 45:
    case 50:
    case 55:
    case 60:
    case 65:
    case 70:
    case 75:
    case 80:
    case 85:
    case 90:
    case 95:
    case 100:
    case 105:
    case 110:
    case 115:
    case 120:
    case 125:
    case 130:
    case 135:
    case 140:
    case 145:
    case 150:
    case 155:
    case 160:
    case 165:
    case 170:
    case 175:
    case 180:
    case 185:
    case 190:
    case 195:
    case 200:
    case 205:
    case 210:
    case 215:
    case 220:
    case 225:
    case 230:
    case 235:
    case 240:
    case 245:
    case 250:
    case 300:
    case 350:
    case 400:
    case 450:
    case 500:
    case 800:
    case 1000:
    case 1200:
    case 1600:
    case 2000:
    case 2400:
      status_prohibit = static_cast<rlc_t_status_prohibit>(num);
      return true;
    default:
      return false;
  }
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
///
/// Ref: 3GPP TS 38.322 Section 7
struct rlc_rx_am_config {
  rlc_am_sn_size sn_field_length; ///< Number of bits used for sequence number

  // Timers Ref: 3GPP TS 38.322 Section 7.3
  int32_t t_reassembly;      ///< Timer used by rx to detect PDU loss (ms)
  int32_t t_status_prohibit; ///< Timer used by rx to prohibit tx of status PDU (ms)

  // Implementation-specific parameters that are not specified by 3GPP
  /// Maximum number of visited SNs in the RX window when building a status report.
  optional<uint32_t> max_sn_per_status;
};

/// \brief Configurable Tx parameters for RLC AM
///
/// Ref: 3GPP TS 38.322 Section 7
struct rlc_tx_am_config {
  rlc_am_sn_size sn_field_length; ///< Number of bits used for sequence number

  // Timers Ref: 3GPP TS 38.322 Section 7.3
  int32_t t_poll_retx; ///< Poll retx timeout (ms)

  // Configurable Parameters. Ref: 3GPP TS 38.322 Section 7.4
  uint32_t max_retx_thresh; ///< Max number of retx
  int32_t  poll_pdu;        ///< Insert poll bit after this many PDUs
  int32_t  poll_byte;       ///< Insert poll bit after this much data (bytes)

  // Implementation-specific parameters that are not specified by 3GPP
  uint32_t queue_size; ///< SDU queue size
  uint32_t max_window; ///< Custom parameter to limit the maximum window size for memory reasons. 0 means no limit.
};

/// \brief Configurable parameters for RLC AM
///
/// Ref: 3GPP TS 38.322 Section 7
struct rlc_am_config {
  rlc_rx_am_config rx; ///< Rx configuration
  rlc_tx_am_config tx; ///< Tx configuration
};

/// \brief Configurable Rx parameters for RLC UM
///
/// Ref: 3GPP TS 38.322 v15.3.0 Section 7
struct rlc_rx_um_config {
  rlc_um_sn_size sn_field_length; ///< Number of bits used for sequence number
  int32_t        t_reassembly;    ///< Timer used by rx to detect PDU loss (ms)
};

/// \brief Configurable Tx parameters for RLC UM
///
/// Ref: 3GPP TS 38.322 v15.3.0 Section 7
struct rlc_tx_um_config {
  rlc_um_sn_size sn_field_length; ///< Number of bits used for sequence number

  // Implementation-specific parameters that are not specified by 3GPP
  uint32_t queue_size; ///< SDU queue size
};

/// \brief Configurable parameters for RLC UM
///
/// Ref: 3GPP TS 38.322 v15.3.0 Section 7
struct rlc_um_config {
  rlc_rx_um_config rx; ///< Rx configuration
  rlc_tx_um_config tx; ///< Tx configuration
};

/// \brief Configurable Rx parameters for RLC TM
///
/// This is a dummy class for consistency with other RLC modes.
/// It may include implementation-specific parameters that are not specified by 3GPP.
struct rlc_rx_tm_config {
  // No members needed here yet
};

/// \brief Configurable Tx parameters for RLC TM
///
/// This includes only implementation-specific parameters that are not specified by 3GPP
struct rlc_tx_tm_config {
  // Implementation-specific parameters that are not specified by 3GPP
  uint32_t queue_size; ///< SDU queue size
};

/// \brief Configurable parameters for RLC TM
///
/// This includes only implementation-specific parameters that are not specified by 3GPP
struct rlc_tm_config {
  rlc_rx_tm_config rx; ///< Rx configuration
  rlc_tx_tm_config tx; ///< Tx configuration
};

/// Configuration of RLC bearer.
struct rlc_config {
  rlc_mode       mode;
  rlc_tm_config  tm;
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

// RLC TM TX config formatter
template <>
struct formatter<srsran::rlc_tx_tm_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rlc_tx_tm_config cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "queue_size={}", cfg.queue_size);
  }
};

// RLC TM RX config formatter
template <>
struct formatter<srsran::rlc_rx_tm_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rlc_rx_tm_config cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "");
  }
};

// RLC TM config formatter
template <>
struct formatter<srsran::rlc_tm_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rlc_tm_config cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{} {}", cfg.tx, cfg.rx);
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
    return format_to(ctx.out(), "tx_sn_size={} queue_size={}", cfg.sn_field_length, cfg.queue_size);
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
                     "rx_sn_size={} t_reassembly={} t_status_prohibit={} max_sn_per_status={}",
                     cfg.sn_field_length,
                     cfg.t_reassembly,
                     cfg.t_status_prohibit,
                     cfg.max_sn_per_status);
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
    if (cfg.mode == srsran::rlc_mode::tm) {
      return format_to(ctx.out(), "{} {}", cfg.mode, cfg.tm);
    }
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
