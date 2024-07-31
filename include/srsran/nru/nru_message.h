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

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/byte_buffer_chain.h"
#include "srsran/adt/static_vector.h"

#include <cstdint>
#include <optional>

namespace srsran {

/// Implementation-specific limit for number of discard blocks used for static allocation.
/// According to TS 38.425 Sec. 5.5.3.11 the value range is {0..244}.
constexpr uint8_t nru_max_nof_pdcp_sn_discard_blocks = 16;
/// Implementation-specific limit for number of lost NR-U SN ranges reported used for static allocation.
/// According to TS 38.425 Sec. 5.5.3.15 the value range is {1..161}.
constexpr uint8_t nru_max_nof_lost_nru_sn_ranges = 2;

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

  bool operator==(const nru_pdcp_sn_discard_block& other) const
  {
    return pdcp_sn_start == other.pdcp_sn_start && block_size == other.block_size;
  }
};

using nru_pdcp_sn_discard_blocks = static_vector<nru_pdcp_sn_discard_block, nru_max_nof_pdcp_sn_discard_blocks>;

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

  bool operator==(const nru_lost_nru_sn_range& other) const
  {
    return nru_sn_start == other.nru_sn_start && nru_sn_end == other.nru_sn_end;
  }
};

using nru_lost_nru_sn_ranges = static_vector<nru_lost_nru_sn_range, nru_max_nof_lost_nru_sn_ranges>;

/// NR-U DL User Data (PDU Type 0).
///
/// Ref: TS 38.425 Sec. 5.5.2.1, Sec. 5.5.3.
struct nru_dl_user_data {
  /// Report polling.
  /// This parameter indicates that the node hosting the NR PDCP entity requests the other side to provide the downlink
  /// delivery status report.
  bool report_polling = false;
  /// Request OutOfSeq Report.
  /// This parameter indicates that the node hosting the NR PDCP entity requests the corresponding node to report the NR
  /// PDCP PDU sequence number successfully delivered out of sequence in downlink delivery status report.
  bool request_out_of_seq_report = false;
  /// Report Delivered.
  /// This parameter indicates the presence of DL report NR PDCP PDU SN.
  bool report_delivered = false;
  /// User data existence flag.
  /// This parameter indicates whether the node hosting the NR PDCP entity has some user data for the concerned data
  /// radio bearer.
  bool user_data_existence_flag = false;
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
  std::optional<uint32_t> dl_discard_pdcp_sn;
  /// Container holding the DL discard NR PDCP PDU SN blocks.
  std::optional<nru_pdcp_sn_discard_blocks> discard_blocks;
  /// DL report NR PDCP PDU SN.
  /// This parameter indicates that the node hosting PDCP entity requests providing the down link delivery status report
  /// when the NR PDCP PDU with this sequence number has been successfully delivered.
  uint32_t dl_report_pdcp_sn = 0;

  bool operator==(const nru_dl_user_data& other) const
  {
    return report_polling == other.report_polling && request_out_of_seq_report == other.request_out_of_seq_report &&
           report_delivered == other.report_delivered && user_data_existence_flag == other.user_data_existence_flag &&
           assist_info_report_polling_flag == other.assist_info_report_polling_flag &&
           retransmission_flag == other.retransmission_flag && nru_sn == other.nru_sn &&
           dl_discard_pdcp_sn == other.dl_discard_pdcp_sn && discard_blocks == other.discard_blocks &&
           dl_report_pdcp_sn == other.dl_report_pdcp_sn;
  }
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
  uint32_t desired_buffer_size_for_drb = 0;
  /// Desired Data Rate.
  /// This parameter indicates the amount of data desired to be received in bytes in a specific amount of time (1 s) for
  /// a specific data radio bearer established for the UE as specified in clause 5.4.2.1.
  std::optional<uint32_t> desired_data_rate;
  /// Container holding the lost NR-U Sequence Number range
  std::optional<nru_lost_nru_sn_ranges> lost_nru_sn_ranges;
  /// Highest successfully delivered NR PDCP Sequence Number.
  /// This parameter indicates feedback about the in-sequence delivery status of NR PDCP PDUs at the corresponding node
  /// towards the UE.
  std::optional<uint32_t> highest_delivered_pdcp_sn;
  /// Highest transmitted NR PDCP Sequence Number.
  /// This parameter indicates the feedback about the transmitted status of NR PDCP PDU sequence at the corresponding
  /// node to the lower layers.
  std::optional<uint32_t> highest_transmitted_pdcp_sn;
  /// Cause Value.
  /// This parameter indicates specific events reported by the corresponding node.
  std::optional<uint8_t> cause_value;
  /// Highest successfully delivered retransmitted NR PDCP Sequence Number.
  /// This parameter indicates feedback about the in-sequence delivery status of NR PDCP PDUs of the retransmission
  /// data at the corresponding node towards the UE.
  std::optional<uint32_t> highest_delivered_retransmitted_pdcp_sn;
  /// Highest retransmitted NR PDCP Sequence Number.
  /// This parameter indicates the feedback about the transmitted status of NR PDCP PDU of the retransmission data at
  /// the corresponding node to the lower layers.
  std::optional<uint32_t> highest_retransmitted_pdcp_sn;

  bool operator==(const nru_dl_data_delivery_status& other) const
  {
    return final_frame_ind == other.final_frame_ind &&
           desired_buffer_size_for_drb == other.desired_buffer_size_for_drb &&
           desired_data_rate == other.desired_data_rate && lost_nru_sn_ranges == other.lost_nru_sn_ranges &&
           highest_delivered_pdcp_sn == other.highest_delivered_pdcp_sn &&
           highest_transmitted_pdcp_sn == other.highest_transmitted_pdcp_sn && cause_value == other.cause_value &&
           highest_delivered_retransmitted_pdcp_sn == other.highest_delivered_retransmitted_pdcp_sn &&
           highest_retransmitted_pdcp_sn == other.highest_retransmitted_pdcp_sn;
  }
};

/// NR-U Assistance Information (PDU Type 2).
///
/// Ref: TS 38.425 Sec. 5.5.2.3, Sec. 5.5.3.
struct nru_assistance_information {
  // TODO: NR-U Assistance Information is not implemented

  bool operator==(const nru_assistance_information& other) const { return true; }
};

/// NR-U DL message exchanged from node holding a PDCP entity (CU-UP) to the peer node (DU).
struct nru_dl_message {
  /// Transport PDU, e.g. PDCP PDU.
  byte_buffer t_pdu;
  /// NR-U DL User Data.
  nru_dl_user_data dl_user_data;

  expected<nru_dl_message> deep_copy()
  {
    nru_dl_message        copy = {};
    expected<byte_buffer> buf  = t_pdu.deep_copy();
    if (not buf.has_value()) {
      return make_unexpected(default_error_t{});
    }
    copy.t_pdu        = std::move(buf.value());
    copy.dl_user_data = dl_user_data;
    return copy;
  }

  bool operator==(const nru_dl_message& other) const
  {
    return t_pdu == other.t_pdu && dl_user_data == other.dl_user_data;
  }
  bool operator!=(const nru_dl_message& other) const { return not(*this == other); }
};

/// NR-U UL message exchanged from node the peer node (DU) to the node holding a PDCP entity (CU-UP).
struct nru_ul_message {
  /// Transport PDU, e.g. PDCP PDU.
  std::optional<byte_buffer_chain> t_pdu;
  /// NR-U DL Data Delivery Status.
  std::optional<nru_dl_data_delivery_status> data_delivery_status;
  /// NR-U Assistance Information.
  std::optional<nru_assistance_information> assistance_information;

  expected<nru_ul_message> deep_copy()
  {
    nru_ul_message copy = {};
    if (t_pdu.has_value()) {
      expected<byte_buffer> buf = t_pdu.value().deep_copy();
      if (not buf.has_value()) {
        return make_unexpected(default_error_t{});
      }
      expected<byte_buffer_chain> chain = byte_buffer_chain::create(std::move(buf.value()));
      if (not chain.has_value()) {
        return make_unexpected(default_error_t{});
      }
      copy.t_pdu = std::move(chain.value());
    }
    if (data_delivery_status.has_value()) {
      copy.data_delivery_status = data_delivery_status.value();
    }
    if (assistance_information.has_value()) {
      copy.assistance_information = assistance_information.value();
    }
    return copy;
  }

  bool operator==(const nru_ul_message& other) const
  {
    return t_pdu == other.t_pdu && data_delivery_status == other.data_delivery_status &&
           assistance_information == other.assistance_information;
  }
  bool operator!=(const nru_ul_message& other) const { return not(*this == other); }
};

} // namespace srsran
