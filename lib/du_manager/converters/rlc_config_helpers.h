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

#include "../du_ue/du_bearer.h"
#include "srsgnb/du_manager/du_manager_params.h"
#include "srsgnb/f1c/du/f1ap_du_ue_config.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/rlc/rlc_config.h"
#include "srsgnb/rlc/rlc_factory.h"

namespace srsgnb {
namespace srs_du {

struct rlc_bearer_config {
  lcid_t     lcid;
  drb_id_t   drb_id = drb_id_t::invalid;
  rlc_config rlc_cfg;

  bool operator==(const rlc_bearer_config& rhs) const
  {
    // TODO: Remaining fields
    return lcid == rhs.lcid and drb_id == rhs.drb_id and rlc_cfg.mode == rhs.rlc_cfg.mode;
  }
};

rlc_mode convert_asn1_f1ap_to_rlc_mode(asn1::f1ap::rlc_mode_e mode);

/// \brief Generates default SRB0 RLC configuration.
rlc_config make_default_srb0_rlc_config();

/// \brief Generates default SRB RLC configuration for SRBs other than SRB0.
rlc_config make_default_srb_rlc_config();

rlc_config create_rlc_config(const drb_to_setup& f1_drb_cfg);

rlc_entity_creation_message make_rlc_entity_creation_message(du_ue_index_t                            ue_index,
                                                             du_cell_index_t                          pcell_index,
                                                             du_bearer&                               bearer,
                                                             const du_manager_params::service_params& du_services);

} // namespace srs_du
} // namespace srsgnb