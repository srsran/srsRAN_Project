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

#include "srsgnb/adt/slotted_array.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/cu_cp/du_processor.h"
#include "srsgnb/pdcp/pdcp_entity.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/rrc/rrc_ue.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Additional context of a SRB containing PDCP, i.e. SRB1 and SRB2.
struct cu_srb_pdcp_context {
  std::unique_ptr<pdcp_entity>                 pdcp_bearer;
  std::unique_ptr<pdcp_tx_lower_notifier>      pdcp_tx_notifier;
  std::unique_ptr<pdcp_rx_upper_data_notifier> rrc_rx_notifier;
};

/// Context for a SRB with adapters between DU processor, F1C, RRC and optionally PDCP.
struct cu_srb_context {
  std::unique_ptr<f1c_rrc_message_notifier> rx_notifier         = std::make_unique<f1c_rrc_null_notifier>();
  std::unique_ptr<rrc_pdu_notifier>         rrc_tx_notifier     = std::make_unique<rrc_pdu_null_notifier>();
  std::unique_ptr<rrc_tx_security_notifier> rrc_tx_sec_notifier = nullptr;
  std::unique_ptr<rrc_rx_security_notifier> rrc_rx_sec_notifier = nullptr;
  optional<cu_srb_pdcp_context>             pdcp_context;
};

/// Holds the context of an DRB that is processed outside of the CU-CP.
struct cu_drb_context {
  lcid_t lcid;
  /// TODO: add required fields for the DRB context.
};

struct ue_context {
  ue_index_t      ue_index;
  du_cell_index_t pcell_index;
  rnti_t          c_rnti;

  rrc_ue_interface*                        rrc = nullptr;
  std::array<cu_srb_context, MAX_NOF_SRBS> srbs;
  slotted_vector<cu_drb_context>           drbs;

  std::unique_ptr<rrc_ue_task_scheduler> task_sched;
};

} // namespace srs_cu_cp
} // namespace srsgnb
