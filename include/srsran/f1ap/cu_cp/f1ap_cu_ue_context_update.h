/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/common/f1ap_ue_id.h"
#include "srsran/ran/cause.h"
#include "srsran/ran/cu_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/s_nssai.h"
#include "srsran/rlc/rlc_config.h"
#include <vector>

namespace srsran {
namespace srs_cu_cp {

struct f1ap_dl_up_tnl_info_to_be_setup_item {
  up_transport_layer_info dl_up_tnl_info;
};

enum class f1ap_cell_ul_cfg { none = 0, ul, sul, ul_and_sul };

struct f1ap_cu_to_du_rrc_info_ext_ies_container {
  optional<byte_buffer> ho_prep_info;
  optional<byte_buffer> cell_group_cfg;
  optional<byte_buffer> meas_timing_cfg;
  optional<byte_buffer> ue_assist_info;
  optional<byte_buffer> cg_cfg;
  optional<byte_buffer> ue_assist_info_eutra;
  optional<byte_buffer> location_meas_info;
  optional<byte_buffer> mu_si_m_gap_cfg;
  optional<byte_buffer> sdt_mac_phy_cg_cfg;
  optional<byte_buffer> mb_si_nterest_ind;
  optional<byte_buffer> need_for_gaps_info_nr;
  optional<byte_buffer> need_for_gap_ncsg_info_nr;
  optional<byte_buffer> need_for_gap_ncsg_info_eutra;
  optional<byte_buffer> cfg_restrict_info_daps;
};

struct f1ap_cu_to_du_rrc_info {
  byte_buffer                                        cg_cfg_info;
  byte_buffer                                        ue_cap_rat_container_list;
  byte_buffer                                        meas_cfg;
  optional<f1ap_cu_to_du_rrc_info_ext_ies_container> ie_exts;
};

struct f1ap_candidate_sp_cell_item {
  nr_cell_global_id_t candidate_sp_cell_id;
};

struct f1ap_drx_cycle {
  uint16_t           long_drx_cycle_len;
  optional<uint16_t> short_drx_cycle_len;
  optional<uint8_t>  short_drx_cycle_timer;
};

struct f1ap_scell_to_be_setup_mod_item {
  nr_cell_global_id_t        scell_id;
  uint8_t                    scell_idx;
  optional<f1ap_cell_ul_cfg> scell_ul_cfg;
};

enum class f1ap_dupl_ind { true_value = 0, false_value };

struct f1ap_srbs_to_be_setup_mod_item {
  srb_id_t                srb_id = srb_id_t::nulltype;
  optional<f1ap_dupl_ind> dupl_ind;
};

struct f1ap_eutran_qos {
  uint16_t                       qci;
  cu_cp_alloc_and_retention_prio alloc_and_retention_prio;
  optional<cu_cp_gbr_qos_info>   gbr_qos_info;
};

enum class f1ap_notif_ctrl { active = 0, not_active };

struct f1ap_flows_mapped_to_drb_item {
  qos_flow_id_t                   qos_flow_id = qos_flow_id_t::invalid;
  cu_cp_qos_flow_level_qos_params qos_flow_level_qos_params;
};

struct f1ap_drb_info {
  cu_cp_qos_flow_level_qos_params                                 drb_qos;
  s_nssai_t                                                       s_nssai;
  optional<f1ap_notif_ctrl>                                       notif_ctrl;
  slotted_id_vector<qos_flow_id_t, f1ap_flows_mapped_to_drb_item> flows_mapped_to_drb_list;
};

enum class f1ap_ul_ue_cfg { no_data = 0, shared, only };

struct f1ap_ul_cfg {
  f1ap_ul_ue_cfg ul_ue_cfg;
};

enum class f1ap_dupl_activation { active = 0, inactive };

struct f1ap_drbs_to_be_setup_mod_item {
  drb_id_t                             drb_id = drb_id_t::invalid;
  f1ap_drb_info                        qos_info;
  std::vector<up_transport_layer_info> ul_up_tnl_info_to_be_setup_list;
  srsran::rlc_mode                     rlc_mod;
  optional<f1ap_ul_cfg>                ul_cfg;
  optional<f1ap_dupl_activation>       dupl_activation;
};

struct f1ap_rat_freq_prio_info {
  // choice
  optional<uint16_t> endc;
  optional<uint16_t> ngran;
};

struct f1ap_res_coordination_transfer_info {
  uint64_t m_enb_cell_id;
  // ResourceCoordinationEUTRACellInfo (optional)
};

/// \brief Request from CU to F1AP-CU to start an F1AP "UE Context Setup" procedure, as per TS38.473 8.3.1.
struct f1ap_ue_context_setup_request {
  ue_index_t                                                  ue_index = ue_index_t::invalid;
  nr_cell_global_id_t                                         sp_cell_id;
  uint8_t                                                     serv_cell_idx;
  optional<f1ap_cell_ul_cfg>                                  sp_cell_ul_cfg;
  f1ap_cu_to_du_rrc_info                                      cu_to_du_rrc_info;
  std::vector<f1ap_candidate_sp_cell_item>                    candidate_sp_cell_list; // max size = 64
  optional<f1ap_drx_cycle>                                    drx_cycle;
  byte_buffer                                                 res_coordination_transfer_container;
  std::vector<f1ap_scell_to_be_setup_mod_item>                scell_to_be_setup_list; // max size = 32
  slotted_id_vector<srb_id_t, f1ap_srbs_to_be_setup_mod_item> srbs_to_be_setup_list;  // max size = 8
  slotted_id_vector<drb_id_t, f1ap_drbs_to_be_setup_mod_item> drbs_to_be_setup_list;  // max size = 64
  optional<bool>                                              inactivity_monitoring_request;
  optional<f1ap_rat_freq_prio_info>                           rat_freq_prio_info;
  byte_buffer                                                 rrc_container;
  optional<uint64_t>                                          masked_imeisv;
  optional<std::string>                                       serving_plmn;
  optional<uint64_t>                                          gnb_du_ue_ambr_ul;
  optional<bool>                                              rrc_delivery_status_request;
  optional<f1ap_res_coordination_transfer_info>               res_coordination_transfer_info;
  optional<uint8_t>                                           serving_cell_mo;
  optional<gnb_cu_ue_f1ap_id_t>                               new_gnb_cu_ue_f1ap_id;
  optional<ran_ue_id_t>                                       ran_ue_id;
};

struct f1ap_du_to_cu_rrc_info {
  byte_buffer cell_group_cfg;
  byte_buffer meas_gap_cfg;
  byte_buffer requested_p_max_fr1;
};

struct f1ap_drbs_setup_mod_item {
  drb_id_t                                          drb_id = drb_id_t::invalid;
  optional<lcid_t>                                  lcid   = lcid_t::INVALID_LCID;
  std::vector<f1ap_dl_up_tnl_info_to_be_setup_item> dl_up_tnl_info_to_be_setup_list;
};

struct f1ap_srbs_failed_to_be_setup_mod_item {
  srb_id_t          srb_id = srb_id_t::nulltype;
  optional<cause_t> cause;
};

struct f1ap_drbs_failed_to_be_setup_mod_item {
  drb_id_t          drb_id = drb_id_t::invalid;
  optional<cause_t> cause;
};

struct f1ap_scell_failed_to_setup_mod_item {
  nr_cell_global_id_t scell_id;
  optional<cause_t>   cause;
};

struct f1ap_srbs_setup_mod_item {
  srb_id_t srb_id = srb_id_t::nulltype;
  lcid_t   lcid   = lcid_t::INVALID_LCID;
};

struct f1ap_potential_sp_cell_item {
  nr_cell_global_id_t potential_sp_cell_id;
};

/// \brief Response from F1AP-CU to CU once "UE Context Setup" procedure is complete.
struct f1ap_ue_context_setup_response {
  bool       success  = false;
  ue_index_t ue_index = ue_index_t::invalid;

  // UE Context Setup Response
  f1ap_du_to_cu_rrc_info                                             du_to_cu_rrc_info;
  optional<rnti_t>                                                   c_rnti;
  optional<byte_buffer>                                              res_coordination_transfer_container;
  optional<bool>                                                     full_cfg;
  slotted_id_vector<drb_id_t, f1ap_drbs_setup_mod_item>              drbs_setup_list;
  slotted_id_vector<srb_id_t, f1ap_srbs_failed_to_be_setup_mod_item> srbs_failed_to_be_setup_list; // max size = 8
  slotted_id_vector<drb_id_t, f1ap_drbs_failed_to_be_setup_mod_item> drbs_failed_to_be_setup_list; // max size = 64
  std::vector<f1ap_scell_failed_to_setup_mod_item>                   scell_failed_to_setup_list;   // max size = 32
  optional<bool>                                                     inactivity_monitoring_resp;
  slotted_id_vector<srb_id_t, f1ap_srbs_setup_mod_item>              srbs_setup_list; // max size = 8

  // UE Context Setup Failure
  optional<cause_t>                        cause;
  std::vector<f1ap_potential_sp_cell_item> potential_sp_cell_list; // max size = 64

  // Common
  optional<crit_diagnostics_t> crit_diagnostics;
};

enum class f1ap_tx_action_ind { stop = 0, restart };

enum class f1ap_rrc_recfg_complete_ind { true_value = 0, fail };

struct f1ap_scell_to_be_remd_item {
  nr_cell_global_id_t scell_id;
};

struct f1ap_drbs_to_be_modified_item {
  drb_id_t                             drb_id = drb_id_t::invalid;
  optional<f1ap_drb_info>              qos_info;
  std::vector<up_transport_layer_info> ul_up_tnl_info_to_be_setup_list;
  optional<f1ap_ul_cfg>                ul_cfg;
};

struct f1ap_rlc_fail_ind {
  lcid_t assocated_lcid = lcid_t::INVALID_LCID;
};

/// \brief Request from CU to F1AP-CU to start an F1AP "UE Context Modification" procedure, as per TS38.473 8.3.4.
struct f1ap_ue_context_modification_request {
  ue_index_t                                                  ue_index = ue_index_t::invalid;
  optional<nr_cell_global_id_t>                               sp_cell_id;
  optional<uint8_t>                                           serv_cell_idx;
  optional<f1ap_cell_ul_cfg>                                  sp_cell_ul_cfg;
  optional<f1ap_drx_cycle>                                    drx_cycle;
  optional<f1ap_cu_to_du_rrc_info>                            cu_to_du_rrc_info;
  optional<f1ap_tx_action_ind>                                tx_action_ind;
  byte_buffer                                                 res_coordination_transfer_container;
  optional<f1ap_rrc_recfg_complete_ind>                       rrc_recfg_complete_ind;
  byte_buffer                                                 rrc_container;
  std::vector<f1ap_scell_to_be_setup_mod_item>                scell_to_be_setup_mod_list;
  std::vector<f1ap_scell_to_be_remd_item>                     scell_to_be_remd_list;
  slotted_id_vector<srb_id_t, f1ap_srbs_to_be_setup_mod_item> srbs_to_be_setup_mod_list;
  slotted_id_vector<drb_id_t, f1ap_drbs_to_be_setup_mod_item> drbs_to_be_setup_mod_list;
  slotted_id_vector<drb_id_t, f1ap_drbs_to_be_modified_item>  drbs_to_be_modified_list;
  std::vector<srb_id_t>                                       srbs_to_be_released_list;
  std::vector<drb_id_t>                                       drbs_to_be_released_list;
  optional<bool>                                              inactivity_monitoring_request;
  optional<f1ap_rat_freq_prio_info>                           rat_freq_prio_info;
  optional<bool>                                              drx_cfg_ind;
  optional<f1ap_rlc_fail_ind>                                 rlc_fail_ind;
  byte_buffer                                                 ul_tx_direct_current_list_info;
  optional<bool>                                              gnb_du_cfg_query;
  optional<uint64_t>                                          gnb_du_ue_ambr_ul;
  optional<bool>                                              execute_dupl;
  optional<bool>                                              rrc_delivery_status_request;
  optional<f1ap_res_coordination_transfer_info>               res_coordination_transfer_info;
  optional<uint8_t>                                           serving_cell_mo;
  optional<bool>                                              need_for_gap;
  optional<bool>                                              full_cfg;
};

struct f1ap_associated_scell_item {
  nr_cell_id_t scell_id;
};

/// \brief Response from F1AP-CU to CU once "UE Context Modification" procedure is complete.
struct f1ap_ue_context_modification_response {
  bool success = false;
  // ue context modification response
  byte_buffer                                                        res_coordination_transfer_container;
  f1ap_du_to_cu_rrc_info                                             du_to_cu_rrc_info;
  slotted_id_vector<drb_id_t, f1ap_drbs_setup_mod_item>              drbs_setup_mod_list;
  slotted_id_vector<drb_id_t, f1ap_drbs_setup_mod_item>              drbs_modified_list;
  slotted_id_vector<srb_id_t, f1ap_srbs_failed_to_be_setup_mod_item> srbs_failed_to_be_setup_mod_list;
  slotted_id_vector<drb_id_t, f1ap_drbs_failed_to_be_setup_mod_item> drbs_failed_to_be_setup_mod_list;
  std::vector<f1ap_scell_failed_to_setup_mod_item>                   scell_failed_to_setup_mod_list;
  slotted_id_vector<drb_id_t, f1ap_drbs_failed_to_be_setup_mod_item> drbs_failed_to_be_modified_list;
  optional<bool>                                                     inactivity_monitoring_resp;
  optional<srsran::rnti_t>                                           c_rnti;
  std::vector<f1ap_associated_scell_item>                            associated_scell_list;
  slotted_id_vector<srb_id_t, f1ap_srbs_setup_mod_item>              srbs_setup_mod_list;
  slotted_id_vector<srb_id_t, f1ap_srbs_setup_mod_item>              srbs_modified_list;
  optional<bool>                                                     full_cfg;

  // UE Context Modification Failure
  optional<cause_t> cause;

  // Common
  optional<crit_diagnostics_t> crit_diagnostics;
};

/// \brief Request Command for F1AP UE CONTEXT Release Request.
struct f1ap_ue_context_release_request {
  ue_index_t ue_index;
  cause_t    cause;
};

} // namespace srs_cu_cp
} // namespace srsran
