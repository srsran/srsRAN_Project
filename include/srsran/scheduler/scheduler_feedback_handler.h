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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/static_vector.h"
#include "srsran/adt/variant.h"
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

struct ul_crc_pdu_indication {
  rnti_t rnti;
  /// UE index associated to the UL CRC. In case there is no UE with the respective RNTI (e.g. Msg3), this value is
  /// set to INVALID_DU_UE_INDEX.
  du_ue_index_t   ue_index;
  harq_id_t       harq_id;
  bool            tb_crc_success;
  optional<float> ul_sinr_metric;
};

/// \brief UL HARQ CRC indication for a given UE PDU.
struct ul_crc_indication {
  du_cell_index_t                                               cell_index;
  slot_point                                                    sl_rx;
  static_vector<ul_crc_pdu_indication, MAX_PUSCH_PDUS_PER_SLOT> crcs;
};

/// \brief UCI indication for a given UE.
struct uci_indication {
  struct uci_pdu {
    struct uci_pucch_f0_or_f1_pdu {
      constexpr static size_t                                      NOF_HARQS_PER_UCI = 2;
      bool                                                         sr_detected;
      static_vector<mac_harq_ack_report_status, NOF_HARQS_PER_UCI> harqs;
      /// \brief Metric of channel quality that ranges from -65.534 to 65.534 dBs.
      optional<float> ul_sinr;
    };
    struct uci_pusch_pdu {
      static_vector<mac_harq_ack_report_status, uci_constants::MAX_NOF_HARQ_BITS> harqs;
      optional<csi_report_data>                                                   csi;
    };
    struct uci_pucch_f2_or_f3_or_f4_pdu {
      /// Maximum number of SR bits expected on the PUCCH transmission.
      static const unsigned MAX_SR_PAYLOAD_SIZE_BITS = 4;

      bounded_bitset<MAX_SR_PAYLOAD_SIZE_BITS>                                    sr_info;
      static_vector<mac_harq_ack_report_status, uci_constants::MAX_NOF_HARQ_BITS> harqs;
      optional<csi_report_data>                                                   csi;
      /// \brief Metric of channel quality that ranges from -65.534 to 65.534 dBs.
      optional<float> ul_sinr;
    };

    du_ue_index_t                                                                ue_index;
    rnti_t                                                                       crnti;
    variant<uci_pucch_f0_or_f1_pdu, uci_pusch_pdu, uci_pucch_f2_or_f3_or_f4_pdu> pdu;
  };
  using uci_pdu_list = static_vector<uci_pdu, MAX_UCI_PDUS_PER_UCI_IND>;

  du_cell_index_t cell_index;
  slot_point      slot_rx;
  uci_pdu_list    ucis;
};

struct dl_mac_ce_indication {
  du_ue_index_t ue_index;
  lcid_dl_sch_t ce_lcid;
};

/// \brief Information and context relative to PHR forwarded by MAC.
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
  /// \param phr PHR indication message sent by MAC.
  virtual void handle_ul_phr_indication(const ul_phr_indication_message& phr_ind) = 0;

  /// \brief Command scheduling of DL MAC CE for a given UE.
  /// \param mac_ce DL MAC CE to be scheduled.
  virtual void handle_dl_mac_ce_indication(const dl_mac_ce_indication& mac_ce) = 0;
};

} // namespace srsran
