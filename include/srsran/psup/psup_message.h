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

#include "srsran/adt/optional.h"
#include "srsran/ran/cu_types.h"
#include <cstdint>

namespace srsran {

/// PSUP PDU Type
///
/// Ref: TS 38.415 Sec. 5.5.3.1
enum class psup_pdu_type : uint8_t { dl_pdu_session_information = 0, ul_pdu_session_information = 1 };

/// Convert PSUP PDU Type to integer.
constexpr inline uint8_t psup_pdu_type_to_uint(psup_pdu_type pdu_type)
{
  return static_cast<uint8_t>(pdu_type);
}

/// Convert integer to PSUP PDU Type.
constexpr inline psup_pdu_type uint_to_psup_pdu_type(uint8_t num)
{
  return static_cast<psup_pdu_type>(num);
}

// See TS 38.415 Sec. 5.5.3.7: Pagin Policy Indicator (PPI) - value range: (0..7)
constexpr static uint8_t MAX_NOF_PPI = 8;

/// PSUP Paging Policy Indicator (PPI)
///
/// Ref: TS 38.415 Sec. 5.5.3.7
enum class psup_ppi : uint8_t { min = 0, max = MAX_NOF_PPI - 1, invalid = MAX_NOF_PPI };

/// Convert PSUP Paging Policy Indicator to integer.
constexpr inline uint8_t psup_ppi_to_uint(psup_ppi ppi)
{
  return static_cast<uint8_t>(ppi);
}

/// Convert integer to PSUP Paging Policy Indicator.
constexpr inline psup_ppi uint_to_psup_ppi(uint8_t num)
{
  return static_cast<psup_ppi>(num);
}

/// DL PDU SESSION INFORMATION (PDU Type 0)
///
/// Ref: TS 38.415 Sec. 5.2.2.1
struct psup_dl_pdu_session_information {
  /// PDU Type.
  /// The PDU Type indicates the structure of the PDU session UP frame. The field takes the value of the PDU Type it
  /// identifies; i.e. "0" for PDU Type 0. The PDU type is in bit 4 to bit 7 in the first octet of the frame.
  const psup_pdu_type pdu_type = psup_pdu_type::dl_pdu_session_information;
  /// Reflective QoS Indicator (RQI).
  /// This parameter indicates activation of the reflective QoS towards the UE for the transferred packet as described
  /// in clause 5.4.1.1. It is used only in the downlink direction. If RQA (Reflective QoS Activation) has not been
  /// configured for the involved QoS flow, the RQI shall be ignored by the NG-RAN node.
  bool rqi = false;
  /// QoS Flow Identifier (QFI).
  /// When present this parameter indicates the QoS Flow Identifier of the QoS flow to which the transferred packet
  /// belongs.
  qos_flow_id_t qos_flow_id = qos_flow_id_t::invalid;
  /// Paging Policy Indicator (PPI).
  /// When present, the Paging Policy Indicator is used for paging policy differentiation (see 3GPP TS 23.501). This
  /// field applies to PDU sessions of IP type.
  optional<psup_ppi> ppi;
  /// DL Received Time Stamp.
  /// This field indicates the time when the UPF sends the DL PDU Session Information frame with the QMP field set to 1.
  /// It is used only in the downlink direction and encoded in the same format as the 64 - bit timestamp format as
  /// defined in Section 6 of IETF RFC 5905 [6].
  optional<uint64_t> dl_sending_time_stamp;
  /// DL QFI Sequence Number.
  /// This parameter indicates the sequence number as assigned by the UPF/ NG-RAN node associated with a given QoS flow.
  optional<uint32_t> dl_qfi_sn;

  bool operator==(const psup_dl_pdu_session_information& other) const
  {
    return rqi == other.rqi && qos_flow_id == other.qos_flow_id && ppi == other.ppi &&
           dl_sending_time_stamp == other.dl_sending_time_stamp && dl_qfi_sn == other.dl_qfi_sn;
  }
};

/// UL PDU SESSION INFORMATION (PDU Type 1)
///
/// Ref: TS 38.415 Sec. 5.2.2.2
struct psup_ul_pdu_session_information {
  /// PDU Type.
  /// The PDU Type indicates the structure of the PDU session UP frame. The field takes the value of the PDU Type it
  /// identifies; i.e. "0" for PDU Type 0. The PDU type is in bit 4 to bit 7 in the first octet of the frame.
  const psup_pdu_type pdu_type = psup_pdu_type::ul_pdu_session_information;
  /// QoS Flow Identifier (QFI).
  /// When present this parameter indicates the QoS Flow Identifier of the QoS flow to which the transferred packet
  /// belongs.
  qos_flow_id_t qos_flow_id = qos_flow_id_t::invalid;
  /// DL Sending Time Stamp Repeated.
  /// This field indicates the value of the DL Sending Time Stamp field that the NG-RAN has received in the DL PDU
  /// Session Information frame with the QMP field set to 1 for the involved QoS flow. It is used only in the uplink
  /// direction and encoded in the same format as the 64-bit timestamp format as defined in Section 6 of IETF RFC 5905
  /// [6]. The UPF shall, if supported, use this information to calculate DL or RTT delay between the NG-RAN and the UPF
  /// as specified in [5].
  optional<uint64_t> dl_sending_time_stamp_repeated;
  /// DL Received Time Stamp.
  /// This field indicates the time when the NG-RAN node receives the DL PDU Session Information frame with the QMP
  /// field set to 1 for the involved QoS flow. It is used only in the uplink direction and encoded in the same format
  /// as the 64-bit timestamp format as defined in Section 6 of IETF RFC 5905 [6]. The UPF shall, if supported, use
  /// this information to calculate DL or RTT delay between the NG-RAN and the UPF as specified in [5].
  optional<uint64_t> dl_received_time_stamp;
  /// DL Sending Time Stamp.
  /// This field indicates the time when the UPF sends the DL PDU Session Information frame with the QMP field set
  /// to 1. It is used only in the downlink direction and encoded in the same format as the 64-bit timestamp format as
  /// defined in Section 6 of IETF RFC 5905 [6].
  optional<uint64_t> ul_sending_time_stamp;
  /// DL Delay Result.
  /// This field indicates the downlink delay measurement result which is the sum of the delay incurred in NG-RAN
  /// (including the delay at gNB-CU-UP, on F1-U and on gNB-DU) and the delay over Uu interface in milliseconds for the
  /// involved QoS flow. It is used only in the uplink direction and encoded as an Unsigned32 binary integer value. The
  /// UPF shall, if supported, use this information to calculate DL or RTT delay as specified in [5].
  optional<uint32_t> dl_delay_result;
  /// UL Delay Result.
  /// This field indicates the uplnk delay measurement result which is the sum of the delay incurred in NG-RAN
  /// (including the delay at gNB-CU-UP, on F1-U and on gNB-DU) and the delay over Uu interface in milliseconds for the
  /// involved QoS flow. It is used only in the uplink direction and encoded as an Unsigned32 binary integer value. The
  /// UPF shall, if supported, use this information to calculate UL or RTT delay as specified in [5].
  optional<uint32_t> ul_delay_result;
  /// UL QFI Sequence Number.
  /// This parameter indicates the sequence number as assigned by the NG-RAN node associated with a given QoS flow.
  optional<uint32_t> ul_qfi_sn;
};

} // namespace srsran
