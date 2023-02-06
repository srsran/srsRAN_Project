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

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/adt/variant.h"
#include "srsgnb/mac/bsr_format.h"
#include "srsgnb/mac/lcid_dl_sch.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/phy_time_unit.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/slot_pdu_capacity_constants.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/ran/uci/uci_constants.h"
#include "srsgnb/scheduler/harq_id.h"

namespace srsgnb {

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
  du_ue_index_t ue_index;
  harq_id_t     harq_id;
  bool          tb_crc_success;
  double        ul_sinr_metric;
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
      constexpr static size_t                NOF_HARQS_PER_UCI = 2;
      bool                                   sr_detected;
      static_vector<bool, NOF_HARQS_PER_UCI> harqs;
    };
    struct uci_pusch_pdu {
      bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS>               harqs;
      bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS> csi_part1;
      bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS> csi_part2;
    };

    du_ue_index_t                                  ue_index;
    rnti_t                                         crnti;
    variant<uci_pucch_f0_or_f1_pdu, uci_pusch_pdu> pdu;
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

class scheduler_feedback_handler
{
public:
  virtual ~scheduler_feedback_handler()                                       = default;
  virtual void handle_ul_bsr_indication(const ul_bsr_indication_message& bsr) = 0;
  virtual void handle_crc_indication(const ul_crc_indication& crc)            = 0;
  virtual void handle_uci_indication(const uci_indication& uci)               = 0;

  /// \brief Command scheduling of DL MAC CE for a given UE.
  virtual void handle_dl_mac_ce_indication(const dl_mac_ce_indication& mac_ce) = 0;
};

} // namespace srsgnb
