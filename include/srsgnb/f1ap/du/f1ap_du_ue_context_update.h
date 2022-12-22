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

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/up_transport_layer_info.h"

namespace srsgnb {
namespace srs_du {

/// \brief Possible modes for an DRB RLC entity.
enum class drb_rlc_mode { am = 0, um_bidir, um_unidir_ul, um_unidir_dl };

/// \brief DRB to be setup in the UE context.
struct f1ap_drb_to_setup {
  drb_id_t                             drb_id;
  optional<lcid_t>                     lcid;
  drb_rlc_mode                         mode;
  std::vector<up_transport_layer_info> uluptnl_info_list;
};

/// \brief SCell to be setup in the UE context.
struct f1ap_scell_to_setup {
  serv_cell_index_t serv_cell_index;
  du_cell_index_t   cell_index;
};

/// \brief DRB that was setup successfully in the F1AP UE context.
struct f1ap_drb_setup {
  drb_id_t                             drb_id;
  optional<lcid_t>                     lcid;
  std::vector<up_transport_layer_info> dluptnl_info_list;
};

/// \brief Request from DU F1AP to DU manager to modify existing UE configuration.
struct f1ap_ue_context_update_request {
  du_ue_index_t                    ue_index;
  std::vector<srb_id_t>            srbs_to_setup;
  std::vector<f1ap_drb_to_setup>   drbs_to_setup;
  std::vector<f1ap_scell_to_setup> scells_to_setup;
  std::vector<serv_cell_index_t>   scells_to_rem;
};

/// \brief Response from DU manager to DU F1AP with the result of the UE context update.
struct f1ap_ue_context_update_response {
  bool                        result;
  std::vector<f1ap_drb_setup> drbs_setup;
  byte_buffer                 du_to_cu_rrc_container;
};

} // namespace srs_du
} // namespace srsgnb
