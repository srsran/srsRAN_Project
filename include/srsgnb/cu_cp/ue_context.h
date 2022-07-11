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

#include "srsgnb/adt/slot_array.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/cu_cp/srb_pdu_notifier.h"
#include "srsgnb/pdcp/pdcp_entity.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/rrc/rrc_ue_entity_interface.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Holds context for one SRB that is processed within the CU-CP.
struct cu_srb_context {
  lcid_t                            lcid;
  std::unique_ptr<srb_pdu_notifier> rx_notifier;
  std::unique_ptr<srb_pdu_notifier> tx_notifier;
  std::unique_ptr<pdcp_entity>      pdcp_bearer;
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
  byte_buffer     du_to_cu_rrc_container;

  rrc_ue_entity_interface*                 rrc = nullptr;
  slot_vector<cu_srb_context>              srbs;
  slot_vector<cu_drb_context>              drbs;
};

} // namespace srs_cu_cp
} // namespace srsgnb
