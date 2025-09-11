/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/adt/byte_buffer.h"
#include "srsran/f1ap/ue_context_management_configs.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/serv_cell_index.h"

namespace srsran {
namespace srs_du {

/// \brief F1AP sends this request to the DU to create a new UE context. This happens in the particular case
/// of a F1AP UE Context Setup Request received without associated logical F1-connection.
struct f1ap_ue_context_creation_request {
  du_ue_index_t   ue_index;
  du_cell_index_t pcell_index;
};

/// \brief Response from the DU back to the F1AP with the created UE index.
struct f1ap_ue_context_creation_response {
  bool result;
  /// C-RNTI allocated during the UE creation, that the F1AP can send to the CU-CP in its response.
  rnti_t crnti;
};

/// \brief SCell to be setup in the UE context.
struct f1ap_scell_to_setup {
  serv_cell_index_t serv_cell_index;
  du_cell_index_t   cell_index;
};

struct f1ap_serving_cell_mo_list_item {
  uint8_t  serving_cell_mo;
  uint32_t ssb_freq;
};

/// \brief Request from DU F1AP to DU manager to modify existing UE configuration.
struct f1ap_ue_context_update_request {
  du_ue_index_t                                              ue_index;
  std::optional<nr_cell_global_id_t>                         spcell_id;
  std::optional<uint8_t>                                     serving_cell_mo;
  std::optional<std::vector<f1ap_serving_cell_mo_list_item>> serving_cell_mo_list;
  /// New SRBs to setup.
  std::vector<srb_id_t> srbs_to_setup;
  /// List of new DRBs to setup.
  std::vector<f1ap_drb_to_setup> drbs_to_setup;
  /// List of DRBs to modify.
  std::vector<f1ap_drb_to_modify> drbs_to_mod;
  /// List of DRBs to remove.
  std::vector<drb_id_t>            drbs_to_rem;
  std::vector<f1ap_scell_to_setup> scells_to_setup;
  std::vector<serv_cell_index_t>   scells_to_rem;
  /// \brief If true, the gnb-DU shall generate a cell group configuration using full configuration. Otherwise, delta,
  /// should be used.
  bool full_config_required;
  /// \brief measConfig selected by the CU-CP. If non-empty, the gnb-DU shall deduce which changes to measConfig need
  /// to be applied as per TS 38.473, 8.3.1.2.
  byte_buffer meas_cfg;
  /// \brief Optional HO preparation information. If present, the gnb-DU shall proceed with a reconfiguration with sync
  /// as defined in TS 38.331, and TS 38.473, 8.3.1.2.
  byte_buffer ho_prep_info;
  /// \brief If a source cell group config is included, the gnb-DU shall generate a cell group configuration using
  /// full configuration. Otherwise, delta configuration is allowed, as per TS 38.473, 8.3.1.2.
  byte_buffer source_cell_group_cfg;
  /// Container with the UE-CapabilityRAT-ContainerList, as per TS 38.331.
  byte_buffer ue_cap_rat_list;
  /// Indiction that the CU-CP has received the RRC reconfiguration complete.
  bool rrc_recfg_complete_ind;
};

/// \brief Response from DU manager to DU F1AP with the result of the UE context update.
struct f1ap_ue_context_update_response {
  bool result;
  /// List of DRBs that were successfully setup.
  std::vector<f1ap_drb_setupmod> drbs_setup;
  /// List of DRBs that were successfully modified.
  std::vector<f1ap_drb_setupmod> drbs_mod;
  /// List of DRBs that failed to be setup.
  std::vector<f1ap_drb_failed_to_setupmod> failed_drbs_setups;
  /// List of DRBs that failed to be modified.
  std::vector<f1ap_drb_failed_to_setupmod> failed_drb_mods;
  /// List of servingCellMOs that have been encoded in CellGroupConfig IE.
  std::vector<uint8_t> serving_cell_mo_encoded_in_cgc_list;
  byte_buffer          cell_group_cfg;
  byte_buffer          meas_gap_cfg;
  bool                 full_config_present = false;
};

/// \brief Request Command for F1AP UE CONTEXT Release Request.
struct f1ap_ue_context_release_request {
  enum class cause_type { rlf_mac, rlf_rlc, other };

  du_ue_index_t ue_index;
  cause_type    cause;
};

/// \brief Request Command for F1AP UE CONTEXT Modification Required.
struct f1ap_ue_context_modification_required {};

} // namespace srs_du
} // namespace srsran
