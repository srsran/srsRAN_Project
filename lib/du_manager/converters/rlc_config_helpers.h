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

#include "../du_ue/du_bearer.h"
#include "srsran/du_manager/du_manager_params.h"
#include "srsran/f1ap/du/f1ap_du_ue_config.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/rlc/rlc_config.h"
#include "srsran/rlc/rlc_factory.h"

namespace srsran {
namespace srs_du {

struct rlc_bearer_config {
  lcid_t             lcid;
  optional<drb_id_t> drb_id;
  rlc_config         rlc_cfg;

  bool operator==(const rlc_bearer_config& rhs) const
  {
    // TODO: Remaining fields
    return lcid == rhs.lcid and drb_id == rhs.drb_id and rlc_cfg.mode == rhs.rlc_cfg.mode;
  }
};

rlc_mode convert_asn1_f1ap_to_rlc_mode(asn1::f1ap::rlc_mode_e mode);
rlc_mode convert_asn1_f1ap_to_rlc_mode(drb_rlc_mode mode);

/// \brief Generates default SRB RLC configuration for SRBs other than SRB0.
rlc_config make_default_srb_rlc_config();

/// \brief Create configuration for RLC SRB entity.
rlc_entity_creation_message make_rlc_entity_creation_message(du_ue_index_t                            ue_index,
                                                             du_cell_index_t                          pcell_index,
                                                             du_ue_srb&                               bearer,
                                                             const du_manager_params::service_params& du_services);

/// \brief Create configuration for RLC DRB entity.
rlc_entity_creation_message make_rlc_entity_creation_message(du_ue_index_t                            ue_index,
                                                             du_cell_index_t                          pcell_index,
                                                             du_ue_drb&                               bearer,
                                                             const du_manager_params::service_params& du_services);

} // namespace srs_du
} // namespace srsran
