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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/static_vector.h"
#include "srsran/mac/bsr_format.h"
#include "srsran/mac/lcid_dl_sch.h"
#include "srsran/mac/phr_report.h"
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/phy_time_unit.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/ran/slot_point.h"
#include "srsran/ran/uci/uci_constants.h"
#include "srsran/scheduler/harq_id.h"
#include <variant>

namespace srsran {

/// UL Buffer Status Report for a single logical channel group.
struct ul_bsr_lcg_report {
  lcg_id_t lcg_id;
  uint32_t nof_bytes;
};

using ul_bsr_lcg_report_list = static_vector<ul_bsr_lcg_report, MAX_NOF_LCGS>;

/// \brief UL Buffer Status Report.
/// \remark See ORAN WG8, 9.2.3.2.18 UL Buffer Status Report Indication.
struct ul_bsr_indication_message {
  du_cell_index_t        cell_index;
  du_ue_index_t          ue_index;
  rnti_t                 crnti;
  bsr_format             type;
  ul_bsr_lcg_report_list reported_lcgs;
};

/// CRC indication PDU.
struct ul_crc_pdu_indication {
  rnti_t rnti;
  /// UE index associated to the UL CRC. In case there is no UE with the respective RNTI (e.g. Msg3), this value is
  /// set to INVALID_DU_UE_INDEX.
  du_ue_index_t ue_index;
  /// HARQ process identifier.
  harq_id_t harq_id;
  /// If true, transport block has been successfully decoded, otherwise false.
  bool tb_crc_success;
  /// PUSCH SINR value in dB.
  std::optional<float> ul_sinr_dB;
  /// PUSCH RSRP value in dBFS.
  std::optional<float> ul_rsrp_dBFS;
  /// Timing Advance Offset measured for the UE.
  std::optional<phy_time_unit> time_advance_offset;
};

/// UL HARQ CRC indication for a given UE PDU.
struct ul_crc_indication {
  du_cell_index_t                                               cell_index;
  slot_point                                                    sl_rx;
  static_vector<ul_crc_pdu_indication, MAX_PUSCH_PDUS_PER_SLOT> crcs;
};

/// UCI indication for a given UE.
struct uci_indication {
  struct uci_pdu {
    /// UCI carried in PUCCH Format0 or Format1.
    struct uci_pucch_f0_or_f1_pdu {
      static constexpr size_t NOF_HARQS_PER_UCI = 2;

      /// Set to true if a SR is detected, otherwise false.
      bool sr_detected;
      /// HARQ bits.
      static_vector<mac_harq_ack_report_status, NOF_HARQS_PER_UCI> harqs;
      /// Metric of channel quality in dB.
      std::optional<float> ul_sinr_dB;
      /// Timing Advance Offset measured for the UE.
      std::optional<phy_time_unit> time_advance_offset;
    };

    /// UCI multiplexed in the PUSCH.
    struct uci_pusch_pdu {
      /// HARQ bits.
      static_vector<mac_harq_ack_report_status, uci_constants::MAX_NOF_HARQ_BITS> harqs;
      /// CSI report.
      std::optional<csi_report_data> csi;
    };

    /// UCI carried in PUCCH Format2, Format3 or Format4.
    struct uci_pucch_f2_or_f3_or_f4_pdu {
      /// Maximum number of SR bits expected on the PUCCH transmission.
      static constexpr unsigned MAX_SR_PAYLOAD_SIZE_BITS = 4;

      /// SR bits.
      bounded_bitset<MAX_SR_PAYLOAD_SIZE_BITS> sr_info;
      /// HARQ bits.
      static_vector<mac_harq_ack_report_status, uci_constants::MAX_NOF_HARQ_BITS> harqs;
      /// CSI report.
      std::optional<csi_report_data> csi;
      /// Metric of channel quality in dB.
      std::optional<float> ul_sinr_dB;
      /// Timing Advance Offset measured for the UE.
      std::optional<phy_time_unit> time_advance_offset;
    };

    du_ue_index_t ue_index;
    /// RNTI value corresponding to the UE that generated this PDU.
    rnti_t crnti;
    /// UCI PDU multiplexed either in the PUSCH or encoded in the PUCCH.
    std::variant<uci_pucch_f0_or_f1_pdu, uci_pusch_pdu, uci_pucch_f2_or_f3_or_f4_pdu> pdu;
  };

  using uci_pdu_list = static_vector<uci_pdu, MAX_UCI_PDUS_PER_UCI_IND>;

  // Note: user-defined ctor to avoid zero-initialization of uci_pdu_list.
  uci_indication() {}

  du_cell_index_t cell_index;
  slot_point      slot_rx;
  uci_pdu_list    ucis;
};

struct dl_mac_ce_indication {
  du_ue_index_t ue_index;
  lcid_dl_sch_t ce_lcid;
};

/// Information and context relative to PHR forwarded by MAC.
struct ul_phr_indication_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  rnti_t          rnti;
  phr_report      phr;
};

class scheduler_feedback_handler
{
public:
  virtual ~scheduler_feedback_handler()                                       = default;
  virtual void handle_ul_bsr_indication(const ul_bsr_indication_message& bsr) = 0;
  virtual void handle_crc_indication(const ul_crc_indication& crc)            = 0;
  virtual void handle_uci_indication(const uci_indication& uci)               = 0;

  /// \brief Handles PHR indication sent by MAC.
  ///
  /// \param phr PHR indication message sent by MAC.
  virtual void handle_ul_phr_indication(const ul_phr_indication_message& phr_ind) = 0;

  /// \brief Command scheduling of DL MAC CE for a given UE.
  ///
  /// \param mac_ce DL MAC CE to be scheduled.
  virtual void handle_dl_mac_ce_indication(const dl_mac_ce_indication& mac_ce) = 0;
};

} // namespace srsran
