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

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/byte_buffer_slice_chain.h"

#include <array>
#include <cstdint>

namespace srsgnb {

constexpr uint8_t nru_max_nof_pdcp_sn_discard_blocks = 1;
constexpr uint8_t nru_max_nof_lost_nru_sn_ranges     = 1;

/// Block of consecutive NR PDCP SNs to be discarded. Part of NR-U DL User Data (PDU Type 0).
///
/// Ref: TS 38.425 Sec. 5.5.2.1, Sec. 5.5.3
struct nru_pdcp_sn_discard_block {
  uint32_t pdcp_sn_start = 0; ///< DL discard NR PDCP PDU SN start
  uint8_t  block_size    = 0; ///< Discarded Block size
};

typedef std::array<nru_pdcp_sn_discard_block, nru_max_nof_pdcp_sn_discard_blocks> nru_pdcp_sn_discard_blocks;

/// Range of consecutive NR-U SNs that have been lost. Part of NR-U DL Data Delivery Status (PDU Type 1).
///
/// Ref: TS 38.425 Sec. 5.5.2.2, Sec. 5.5.3
struct nru_lost_nru_sn_range {
  uint32_t nru_sn_start = 0;
  uint32_t nru_sn_end   = 0;
};

typedef std::array<nru_lost_nru_sn_range, nru_max_nof_lost_nru_sn_ranges> nru_lost_nru_sn_ranges;

/// NR-U DL User Data (PDU Type 0).
///
/// Ref: TS 38.425 Sec. 5.5.2.1, Sec. 5.5.3
struct nru_dl_user_data {
  bool dl_discard_blocks               = false;
  bool dl_flush                        = false;
  bool report_polling                  = false;
  bool assist_info_report_polling_flag = false;
  bool retransmission_flag             = false;

  uint32_t nru_sn                = 0;
  uint32_t dl_discard_pdcp_sn    = 0;
  uint8_t  dl_discard_nof_blocks = 0;

  nru_pdcp_sn_discard_blocks discard_blocks = {};
};

/// NR-U DL Data Delivery Status (PDU Type 1).
///
/// Ref: TS 38.425 Sec. 5.5.2.2, Sec. 5.5.3
struct nru_dl_data_delivery_status {
  bool highest_transmitted_pdcp_sn_ind             = false;
  bool highest_delivered_pdcp_sn_ind               = false;
  bool final_frame_ind                             = false;
  bool lost_packet_report                          = false;
  bool data_rate_ind                               = false;
  bool highest_retransmitted_pdcp_sn_ind           = false;
  bool highest_delivered_retransmitted_pdcp_sn_ind = false;
  bool cause_report                                = false;

  uint8_t desired_buffer_size_for_drb     = 0;
  uint8_t desired_data_rate               = 0;
  uint8_t nof_lost_nru_sn_ranges_reported = 0;

  nru_lost_nru_sn_ranges lost_nru_sn_ranges = {};

  uint32_t highest_successfully_delivered_pdcp_sn     = 0;
  uint32_t highest_transmitted_pdcp_sn                = 0;
  uint32_t cause_value                                = 0;
  uint32_t highest_successfully_retransmitted_pdcp_sn = 0;
  uint32_t highest_retransmitted_pdcp_sn              = 0;
};

/// NR-U Assistance Information (PDU Type 2).
///
/// Ref: TS 38.425 Sec. 5.5.2.3, Sec. 5.5.3
struct nru_assistance_information {};

/// NR-U DL message exchanged from node holding a PDCP entity (CU-UP) to the peer node (DU)
struct nru_dl_message {
  // NR-U Payload
  byte_buffer t_pdu;      ///< Transport PDU, e.g. PDCP PDU
  uint32_t    pdcp_count; ///< PDCP count value of the t_pdu
  // NR-U Information
  nru_dl_user_data dl_user_data; ///< NR-U DL User Data
};

/// NR-U UL message exchanged from node the peer node (DU) to the node holding a PDCP entity (CU-UP)
struct nru_ul_message {
  // NR-U Payload
  byte_buffer_slice_chain t_pdu; ///< Transport PDU, e.g. PDCP PDU
  // NR-U Information
  nru_dl_data_delivery_status data_delivery_status;               ///< NR-U DL Data Delivery Status
  bool                        has_data_delivery_status = false;   ///< Flags the presence of DL Data Delivery Status
  nru_assistance_information  assistance_information;             ///< NR-U Assistance Information
  bool                        has_assistance_information = false; ///< Flags the presence of Assistance Information
};

} // namespace srsgnb
