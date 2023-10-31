/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/static_vector.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/up_resource_manager.h"
#include "srsran/rrc/meas_types.h"
#include "srsran/security/security.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief RRC context transfered from one UE object to the other during mobility.
struct rrc_ue_transfer_context {
  security::security_context            sec_context;
  optional<rrc_meas_cfg>                meas_cfg;
  up_context                            up_ctx;
  static_vector<srb_id_t, MAX_NOF_SRBS> srbs; // List of active SRBs (TODO: add PDCP config).
  byte_buffer                           handover_preparation_info;
  bool                                  is_inter_cu_handover = false;
};

/// \brief The UE creation is triggered from the F1AP.
/// It carries an RRC container and the C-RNTI if the DU sent an Initial UL RRC transfer. If the user is created
/// during handover the RNTI is only allocated after the Random Access.
struct cu_cp_ue_creation_message {
  ue_index_t                        ue_index = ue_index_t::invalid;
  nr_cell_global_id_t               cgi;
  uint32_t                          tac;
  byte_buffer                       du_to_cu_rrc_container;
  rnti_t                            c_rnti;
  optional<rrc_ue_transfer_context> rrc_context;
};

} // namespace srs_cu_cp
} // namespace srsran
