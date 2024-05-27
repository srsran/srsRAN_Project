/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
  std::optional<rrc_meas_cfg>           meas_cfg;
  up_context                            up_ctx;
  static_vector<srb_id_t, MAX_NOF_SRBS> srbs; // List of active SRBs (TODO: add PDCP config).
  byte_buffer                           handover_preparation_info;
  bool                                  is_inter_cu_handover = false;
};

} // namespace srs_cu_cp
} // namespace srsran
