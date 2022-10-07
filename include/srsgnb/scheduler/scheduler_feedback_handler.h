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
#include "srsgnb/mac/bsr_format.h"
#include "srsgnb/mac/lcid_dl_sch.h"
#include "srsgnb/mac/mac_constants.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/phy_time_unit.h"
#include "srsgnb/ran/rnti.h"

namespace srsgnb {

/// \brief Scheduling Request.
/// \remark See ORAN WG8, 9.2.3.2.17 Scheduling Request Indication.
struct sr_indication_message {
  du_cell_index_t   cell_index;
  du_ue_index_t     ue_index;
  rnti_t            crnti;
  bounded_bitset<8> sr_payload;
};

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
  uint8_t       harq_id;
  bool          tb_crc_success;
};

/// \brief UL HARQ CRC indication for a given UE PDU.
struct ul_crc_indication {
  du_cell_index_t                                            cell_index;
  static_vector<ul_crc_pdu_indication, MAX_UL_PDUS_PER_SLOT> crcs;
};

struct dl_mac_ce_indication {
  du_ue_index_t ue_index;
  lcid_dl_sch_t ce_lcid;
};

class scheduler_feedback_handler
{
public:
  virtual ~scheduler_feedback_handler()                                       = default;
  virtual void handle_sr_indication(const sr_indication_message& sr)          = 0;
  virtual void handle_ul_bsr_indication(const ul_bsr_indication_message& bsr) = 0;
  virtual void handle_crc_indication(const ul_crc_indication& crc)            = 0;

  /// \brief Command scheduling of DL MAC CE for a given UE.
  virtual void handle_dl_mac_ce_indication(const dl_mac_ce_indication& mac_ce) = 0;
};

} // namespace srsgnb
