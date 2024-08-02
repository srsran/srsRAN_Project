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
#include "srsran/pdcp/pdcp_sn_size.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/five_qi.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/qos/qos_info.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/s_nssai.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/rlc/rlc_mode.h"

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

/// \brief DRB to be setup or modify in the UE context.
struct f1ap_drb_config_request {
  drb_id_t              drb_id;
  std::optional<lcid_t> lcid;
  /// \brief RLC mode. If it is a new bearer to setup, this field is present. If it is an existing bearer that needs
  /// to be modified, this field is absent.
  std::optional<rlc_mode> mode;
  pdcp_sn_size            pdcp_sn_len = pdcp_sn_size::invalid;
  five_qi_t               five_qi;
  uint8_t                 arp_priority_level;
  s_nssai_t               s_nssai;
  /// GBR flow information is present only for GBR QoS flows. See TS 38.473, clause 9.3.1.45.
  std::optional<gbr_qos_info_t>        gbr_flow_info;
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
  std::optional<lcid_t>                lcid;
  std::vector<up_transport_layer_info> dluptnl_info_list;
};

/// \brief Request from DU F1AP to DU manager to modify existing UE configuration.
struct f1ap_ue_context_update_request {
  du_ue_index_t         ue_index;
  std::vector<srb_id_t> srbs_to_setup;
  /// List of DRBs to either setup or modify.
  std::vector<f1ap_drb_config_request> drbs_to_setupmod;
  std::vector<drb_id_t>                drbs_to_rem;
  std::vector<f1ap_scell_to_setup>     scells_to_setup;
  std::vector<serv_cell_index_t>       scells_to_rem;
  /// \brief If true, the gnb-DU shall generate a cell group configuration using full configuration. Otherwise, delta,
  /// should be used.
  bool full_config_required;
  /// \brief Optional HO preparation information. If present, the gnb-DU shall proceed with a reconfiguration with sync
  /// as defined in TS 38.331, and TS 38.473, 8.3.1.2.
  byte_buffer ho_prep_info;
  /// \brief If a source cell group config is included, the gnb-DU shall generate a cell group configuration using
  /// full configuration. Otherwise, delta configuration is allowed, as per TS 38.473, 8.3.1.2.
  byte_buffer source_cell_group_cfg;
};

/// \brief Response from DU manager to DU F1AP with the result of the UE context update.
struct f1ap_ue_context_update_response {
  bool                        result;
  std::vector<f1ap_drb_setup> drbs_setup;
  std::vector<drb_id_t>       drbs_failed_to_setup;
  byte_buffer                 du_to_cu_rrc_container;
  bool                        full_config_present = false;
};

/// \brief Handled causes for RLF.
enum class rlf_cause { max_mac_kos_reached, max_rlc_retxs_reached, rlc_protocol_failure };

/// \brief Request Command for F1AP UE CONTEXT Release Request.
struct f1ap_ue_context_release_request {
  du_ue_index_t ue_index;
  rlf_cause     cause;
};

/// \brief Request Command for F1AP UE CONTEXT Modification Required.
struct f1ap_ue_context_modification_required {};

} // namespace srs_du
} // namespace srsran
