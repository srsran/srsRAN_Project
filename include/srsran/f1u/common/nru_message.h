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

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/byte_buffer_chain.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/static_vector.h"

#include <cstdint>

namespace srsran {

constexpr uint8_t nru_max_nof_pdcp_sn_discard_blocks = 16;
constexpr uint8_t nru_max_nof_lost_nru_sn_ranges     = 1;

/// Block of consecutive NR PDCP SNs to be discarded. Part of NR-U DL User Data (PDU Type 0).
///
/// Ref: TS 38.425 Sec. 5.5.2.1, Sec. 5.5.3.
struct nru_pdcp_sn_discard_block {
  /// DL discard NR PDCP PDU SN start.
  /// This parameter indicates the starting SN of a downlink NR PDCP PDU block to be discarded.
  uint32_t pdcp_sn_start = 0;
  /// Discarded Block size.
  /// This parameter indicates the number of NR PDCP PDUs counted from the starting SN to be discarded.
  uint8_t block_size = 0;
};

typedef static_vector<nru_pdcp_sn_discard_block, nru_max_nof_pdcp_sn_discard_blocks> nru_pdcp_sn_discard_blocks;

/// Range of consecutive NR-U SNs that have been lost. Part of NR-U DL Data Delivery Status (PDU Type 1).
///
/// Ref: TS 38.425 Sec. 5.5.2.2, Sec. 5.5.3.
struct nru_lost_nru_sn_range {
  /// Start of lost NR-U Sequence Number range.
  /// This parameter indicates the start of an NR-U sequence number range reported to be lost.
  uint32_t nru_sn_start = 0;
  /// End of lost NR-U Sequence Number range.
  /// This parameter indicates the end of an NR-U sequence number range reported to be lost.
  uint32_t nru_sn_end = 0;
};

typedef static_vector<nru_lost_nru_sn_range, nru_max_nof_lost_nru_sn_ranges> nru_lost_nru_sn_ranges;

/// NR-U DL User Data (PDU Type 0).
///
/// Ref: TS 38.425 Sec. 5.5.2.1, Sec. 5.5.3.
struct nru_dl_user_data {
  /// Report polling.
  /// This parameter indicates that the node hosting the NR PDCP entity requests the other side to provide the downlink
  /// delivery status report.
  bool report_polling = false;
  /// Assistance Information Report Polling Flag.
  /// This parameter indicates that the node hosting the NR PDCP entity requests the corresponding node to send an
  /// ASSISTANCE INFORMATION DATA PDU.
  bool assist_info_report_polling_flag = false;
  /// Retransmission flag.
  /// This parameter indicates whether the NR PDCP PDU is a retransmission NR-U packet sent by the node hosting the NR
  /// PDCP to the corresponding node.
  bool retransmission_flag = false;
  /// NR-U Sequence Number.
  /// This parameter indicates the NR-U sequence number as assigned by the node hosting the NR PDCP entity.
  uint32_t nru_sn = 0;
  /// DL discard NR PDCP PDU SN.
  /// This parameter indicates the downlink NR discard PDCP PDU sequence number up to and including which all the NR
  /// PDCP PDUs should be discarded.
  optional<uint32_t> dl_discard_pdcp_sn;
  /// Container holding the DL discard NR PDCP PDU SN blocks.
  optional<nru_pdcp_sn_discard_blocks> discard_blocks;
};

/// NR-U DL Data Delivery Status (PDU Type 1).
///
/// Ref: TS 38.425 Sec. 5.5.2.2, Sec. 5.5.3.
struct nru_dl_data_delivery_status {
  /// Final Frame Indication.
  /// This parameter indicates whether the frame is the last DL status report as described in clause 5.4.2.1.
  bool final_frame_ind = false;
  /// Desired buffer size for the data radio bearer.
  /// This parameter indicates the desired buffer size in bytes for the concerned data radio bearer as specified in
  /// clause 5.4.2.1.
  uint8_t desired_buffer_size_for_drb = 0;
  /// Desired Data Rate.
  /// This parameter indicates the amount of data desired to be received in bytes in a specific amount of time (1 s) for
  /// a specific data radio bearer established for the UE as specified in clause 5.4.2.1.
  optional<uint8_t> desired_data_rate;
  /// Container holding the lost NR-U Sequence Number range
  optional<nru_lost_nru_sn_ranges> lost_nru_sn_ranges;
  /// Highest successfully delivered NR PDCP Sequence Number.
  /// This parameter indicates feedback about the in-sequence delivery status of NR PDCP PDUs at the corresponding node
  /// towards the UE.
  optional<uint32_t> highest_delivered_pdcp_sn;
  /// Highest transmitted NR PDCP Sequence Number.
  /// This parameter indicates the feedback about the transmitted status of NR PDCP PDU sequence at the corresponding
  /// node to the lower layers.
  optional<uint32_t> highest_transmitted_pdcp_sn;
  /// Cause Value.
  /// This parameter indicates specific events reported by the corresponding node.
  optional<uint32_t> cause_value;
  /// Highest successfully delivered retransmitted NR PDCP Sequence Number.
  /// This parameter indicates feedback about the in-sequence delivery status of NR PDCP PDUs of the retransmission
  /// data at the corresponding node towards the UE.
  optional<uint32_t> highest_delivered_retransmitted_pdcp_sn;
  /// Highest retransmitted NR PDCP Sequence Number.
  /// This parameter indicates the feedback about the transmitted status of NR PDCP PDU of the retransmission data at
  /// the corresponding node to the lower layers.
  optional<uint32_t> highest_retransmitted_pdcp_sn;
};

/// NR-U Assistance Information (PDU Type 2).
///
/// Ref: TS 38.425 Sec. 5.5.2.3, Sec. 5.5.3.
struct nru_assistance_information {};

/// NR-U DL message exchanged from node holding a PDCP entity (CU-UP) to the peer node (DU).
struct nru_dl_message {
  /// Transport PDU, e.g. PDCP PDU.
  byte_buffer t_pdu;
  /// PDCP Sequence Number of the t_pdu.
  optional<uint32_t> pdcp_sn;
  /// NR-U DL User Data.
  nru_dl_user_data dl_user_data;
};

/// NR-U UL message exchanged from node the peer node (DU) to the node holding a PDCP entity (CU-UP).
struct nru_ul_message {
  /// Transport PDU, e.g. PDCP PDU.
  byte_buffer_chain t_pdu;
  /// NR-U DL Data Delivery Status.
  optional<nru_dl_data_delivery_status> data_delivery_status;
  /// NR-U Assistance Information.
  optional<nru_assistance_information> assistance_information;
};

} // namespace srsran
