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

#include "srsran/asn1/f1ap/f1ap_ies.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/common/f1ap_types.h"
#include "srsran/ran/cause.h"
#include "srsran/ran/cu_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/s_nssai.h"
#include <vector>

namespace srsran {
namespace srs_cu_cp {

struct f1ap_dl_up_tnl_info_to_be_setup_item {
  up_transport_layer_info dl_up_tnl_info;
};

enum class f1ap_cell_ul_cfg { none = 0, ul, sul, ul_and_sul };

struct f1ap_cu_to_du_rrc_info {
  byte_buffer cg_cfg_info;
  byte_buffer ue_cap_rat_container_list;
  byte_buffer meas_cfg;
};

struct f1ap_candidate_sp_cell_item {
  nr_cell_global_id_t candidate_sp_cell_id;
};

struct f1ap_drx_cycle {
  uint16_t           long_drx_cycle_len;
  optional<uint16_t> short_drx_cycle_len;
  optional<uint8_t>  short_drx_cycle_timer;
};

struct f1ap_scell_to_be_setup_item {
  nr_cell_global_id_t scell_id;
  uint8_t             scell_idx;
  f1ap_cell_ul_cfg    scell_ul_cfg;
};

enum class f1ap_dupl_ind { true_value = 0, false_value };

struct f1ap_srbs_to_be_setup_item {
  uint8_t                 srb_id = 0;
  optional<f1ap_dupl_ind> dupl_ind;
};

struct f1ap_eutran_qos {
  uint16_t                       qci;
  cu_cp_alloc_and_retention_prio alloc_and_retention_prio;
  optional<cu_cp_gbr_qos_info>   gbr_qos_info;
};

struct f1ap_qos_info {
  // choice
  optional<f1ap_eutran_qos> eutran_qos;
};

enum class f1ap_ul_ue_cfg { no_data = 0, shared, only };

struct f1ap_ul_cfg {
  f1ap_ul_ue_cfg ul_ue_cfg;
};

enum class f1ap_dupl_activation { active = 0, inactive };

struct f1ap_drbs_to_be_setup_item {
  drb_id_t                                          drb_id;
  f1ap_qos_info                                     qos_info;
  std::vector<f1ap_dl_up_tnl_info_to_be_setup_item> ul_up_tnl_info_to_be_setup_list;
  rlc_mode                                          rlc_mod;
  optional<f1ap_ul_cfg>                             ul_cfg;
  optional<f1ap_dupl_activation>                    dupl_activation;
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

enum class f1ap_trace_depth {
  minimum = 0,
  medium,
  max,
  minimum_without_vendor_specific_ext,
  medium_without_vendor_specific_ext,
  max_without_vendor_specific_ext
};

struct f1ap_trace_activation {
  uint64_t         trace_id;
  uint64_t         interfaces_to_trace;
  f1ap_trace_depth trace_depth;
  uint64_t         trace_collection_entity_ip_address;
};

enum class f1ap_ch_otrigger_inter_du { cho_initiation = 0, cho_replace };

struct f1ap_conditional_inter_du_mob_info {
  f1ap_ch_otrigger_inter_du cho_trigger;
  gnb_du_ue_f1ap_id_t       target_gnb_du_ue_f1ap_id;
};

enum class f1ap_f1c_path_nsa { lte = 0, nr, both };

struct f1ap_f1c_transfer_path {
  f1ap_f1c_path_nsa f1_cp_ath_nsa;
};

enum class f1ap_f1c_path_nr_dc { mcg = 0, scg, both };

struct f1ap_f1c_transfer_path_nr_dc {
  f1ap_f1c_path_nr_dc f1c_path_nr_dc;
};

enum class f1ap_mdt_polluted_meas_ind { id_c = 0, no_id_c };

enum class f1ap_scg_activation_request { activ_scg = 0, deactiv_scg };

struct f1ap_cg_sdt_session_info {
  gnb_cu_ue_f1ap_id_t gnb_cu_ue_f1ap_id;
  gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id;
};

enum class f1ap_five_g_pro_se_authorization { authorized = 0, not_authorized };

struct f1ap_five_g_pro_se_authorized {
  optional<f1ap_five_g_pro_se_authorization> five_g_pro_se_direct_discovery;
  optional<f1ap_five_g_pro_se_authorization> five_g_pro_se_direct_communication;
  optional<f1ap_five_g_pro_se_authorization> five_g_pro_se_layer2_ue_to_network_relay;
  optional<f1ap_five_g_pro_se_authorization> five_g_pro_se_layer3_ue_to_network_relay;
  optional<f1ap_five_g_pro_se_authorization> five_g_pro_se_layer2_remote_ue;
};

struct f1ap_uu_rlc_ch_qos_info {
  // choice
  optional<cu_cp_qos_flow_level_qos_params> uu_rlc_ch_qos;
  optional<srb_id_t>                        uu_ctrl_plane_traffic_type;
};

struct f1ap_uu_rlc_ch_to_be_setup_item {
  uint16_t                uu_rlc_ch_id;
  f1ap_uu_rlc_ch_qos_info uu_rlc_ch_qos_info;
  rlc_mode                rlc_mod;
};

struct f1ap_path_switch_cfg {
  uint32_t target_relay_ue_id;
  uint16_t remote_ue_local_id;
  uint16_t t420;
};

struct f1ap_gnb_du_ue_slice_max_bit_rate_item {
  s_nssai_t snssai;
  uint64_t  ue_slice_max_bit_rate_ul;
};

struct f1ap_mbs_session_id {
  uint64_t           tmgi;
  optional<uint64_t> nid;
};

struct f1ap_multicast_mbs_session_list_item {
  f1ap_mbs_session_id mbs_session_id;
};

struct f1ap_mrb_progress_info {
  // choice
  optional<uint16_t> pdcp_sn12;
  optional<uint32_t> pdcp_sn18;
};

struct f1ap_ue_multicast_mrbs_to_be_setup_item {
  uint16_t                         mrb_id;
  optional<bool>                   mbs_ptp_retx_tunnel_required;
  optional<f1ap_mrb_progress_info> mbs_ptp_forwarding_required_info;
};

struct f1ap_serving_cell_mo_list_item {
  uint8_t  serving_cell_mo;
  uint32_t ssb_freq;
};

/// \brief Request from CU to F1AP-CU to start an F1AP "UE Context Setup" procedure, as per TS38.473 8.3.1.
struct f1ap_ue_context_setup_request {
  ue_index_t ue_index = ue_index_t::invalid;

  nr_cell_global_id_t                           sp_cell_id;
  uint8_t                                       serv_cell_idx;
  optional<f1ap_cell_ul_cfg>                    sp_cell_ul_cfg;
  f1ap_cu_to_du_rrc_info                        cu_to_du_rrc_info;
  optional<f1ap_candidate_sp_cell_item>         candidate_sp_cell_list; // max size = 64
  optional<f1ap_drx_cycle>                      drx_cycle;
  byte_buffer                                   res_coordination_transfer_container;
  std::vector<f1ap_scell_to_be_setup_item>      scell_to_be_setup_list; // max size = 32
  std::vector<f1ap_srbs_to_be_setup_item>       srbs_to_be_setup_list;  // max size = 8
  std::vector<f1ap_drbs_to_be_setup_item>       drbs_to_be_setup_list;  // max size = 64
  optional<bool>                                inactivity_monitoring_request;
  optional<f1ap_rat_freq_prio_info>             rat_freq_prio_info;
  byte_buffer                                   rrc_container;
  optional<uint64_t>                            masked_imeisv;
  optional<std::string>                         serving_plmn;
  optional<uint64_t>                            gnb_du_ue_ambr_ul;
  optional<bool>                                rrc_delivery_status_request;
  optional<f1ap_res_coordination_transfer_info> res_coordination_transfer_info;
  optional<uint8_t>                             serving_cell_mo;
  optional<uint64_t>                            new_gnb_cu_ue_f1ap_id;
  optional<ran_ue_id_t>                         ran_ue_id;
  optional<f1ap_trace_activation>               trace_activation;
  optional<uint64_t>                            add_rrm_prio_idx;

  // BHChannels-ToBeSetup-List
  // cfg_bap_address
  // NRV2XServicesAuthorized
  // LTEV2XServicesAuthorized
  // NRUESidelinkAggregateMaximumBitrate
  // LTEUESidelinkAggregateMaximumBitrate
  // pc5_link_ambr
  // SLDRBs-ToBeSetup-List

  optional<f1ap_conditional_inter_du_mob_info> conditional_inter_du_mob_info;
  std::vector<uint64_t>                        management_based_mdt_plmn_list; // max size = 16
  optional<uint64_t>                           serving_n_id;
  optional<f1ap_f1c_transfer_path>             f1_c_transfer_path;
  optional<f1ap_f1c_transfer_path_nr_dc>       f1_c_transfer_path_nr_dc;
  optional<f1ap_mdt_polluted_meas_ind>         mdt_polluted_meas_ind;
  optional<f1ap_scg_activation_request>        scg_activation_request;
  optional<f1ap_cg_sdt_session_info>           cg_sdt_session_info_old;
  optional<f1ap_five_g_pro_se_authorized>      five_g_pro_se_authorized;

  // NRUESidelinkAggregateMaximumBitrate
  // five_g_pro_se_pc5_link_ambr

  std::vector<f1ap_uu_rlc_ch_to_be_setup_item> uu_rlc_ch_to_be_setup_list; // max size = 32

  // PC5RLCChannelToBeSetupList

  optional<f1ap_path_switch_cfg>                       path_switch_cfg;
  std::vector<f1ap_gnb_du_ue_slice_max_bit_rate_item>  gnb_du_ue_slice_max_bit_rate_list;  // max size = 8
  std::vector<f1ap_multicast_mbs_session_list_item>    multicast_mbs_session_setup_list;   // max size = 256
  std::vector<f1ap_ue_multicast_mrbs_to_be_setup_item> ue_multicast_mrbs_to_be_setup_list; // max size = 64
  std::vector<f1ap_serving_cell_mo_list_item>          serving_cell_mo_list;               // max size = 16
};

struct f1ap_drbs_setup_item {
  drb_id_t                                          drb_id;
  optional<lcid_t>                                  lcid;
  std::vector<f1ap_dl_up_tnl_info_to_be_setup_item> dl_up_tnl_info_to_be_setup_list;
};

struct f1ap_srbs_failed_to_be_setup_item {
  srb_id_t          srb_id;
  optional<cause_t> cause;
};

struct f1ap_drbs_failed_to_be_setup_item {
  drb_id_t          drb_id;
  optional<cause_t> cause;
};

struct f1ap_scell_failed_to_setup_mod_item {
  nr_cell_global_id_t scell_id;
  optional<cause_t>   cause;
};

struct f1ap_srbs_setup_item {
  srb_id_t srb_id;
  lcid_t   lcid;
};

struct f1ap_bh_chs_setup_item {
  uint64_t bh_rlc_ch_id;
};

struct f1ap_bh_chs_failed_to_be_setup_item {
  uint64_t          bh_rlc_ch_id;
  optional<cause_t> cause;
};

struct f1ap_sl_drbs_setup_item {
  uint16_t sl_drb_id;
};

struct f1ap_sl_drbs_failed_to_be_setup_item {
  uint16_t          sl_drb_id;
  optional<cause_t> cause;
};

enum class f1ap_scg_activation_status { scg_activ = 0, scg_deactiv };

struct f1ap_uu_rlc_ch_setup_item {
  uint64_t uu_rlc_ch_id;
};

struct f1ap_uu_rlc_ch_failed_to_be_setup_item {
  uint64_t          uu_rlc_ch_id;
  optional<cause_t> cause;
};

struct f1ap_pc5_rlc_ch_setup_item {
  uint16_t           pc5_rlc_ch_id;
  optional<uint16_t> remote_ue_local_id;
};

struct f1ap_pc5_rlc_ch_failed_to_be_setup_item {
  uint16_t           pc5_rlc_ch_id;
  optional<uint16_t> remote_ue_local_id;
  optional<cause_t>  cause;
};

struct f1ap_serving_cell_mo_encoded_in_cgc_item {
  uint8_t serving_cell_mo;
};

struct f1ap_potential_sp_cell_item {
  nr_cell_global_id_t potential_sp_cell_id;
};

/// \brief Response from F1AP-CU to CU once "UE Context Setup" procedure is complete.
struct f1ap_ue_context_setup_response {
  bool success = false;

  // UE Context Setup Response
  f1ap_cu_to_du_rrc_info                            cu_to_du_rrc_info;
  optional<rnti_t>                                  c_rnti;
  optional<byte_buffer>                             res_coordination_transfer_container;
  optional<bool>                                    full_cfg;
  std::vector<f1ap_drbs_setup_item>                 drbs_setup_list;
  std::vector<f1ap_srbs_failed_to_be_setup_item>    srbs_failed_to_be_setup_list; // max size = 8
  std::vector<f1ap_drbs_failed_to_be_setup_item>    drbs_failed_to_be_setup_list; // max size = 64
  std::vector<f1ap_scell_failed_to_setup_mod_item>  scell_failed_to_setup_list;   // max size = 32
  optional<bool>                                    inactivity_monitoring_resp;
  std::vector<f1ap_srbs_setup_item>                 srbs_setup_list;                 // max size = 8
  std::vector<f1ap_bh_chs_setup_item>               bh_chs_setup_list;               // max size = 65536
  std::vector<f1ap_bh_chs_failed_to_be_setup_item>  bh_chs_failed_to_be_setup_list;  // max size = 65536
  std::vector<f1ap_sl_drbs_setup_item>              sl_drbs_setup_list;              // max size = 512
  std::vector<f1ap_sl_drbs_failed_to_be_setup_item> sl_drbs_failed_to_be_setup_list; // max size = 512

  optional<f1ap_scg_activation_status>                  scg_activation_status;
  std::vector<f1ap_uu_rlc_ch_setup_item>                uu_rlc_ch_setup_list;                // max size = 32
  std::vector<f1ap_uu_rlc_ch_failed_to_be_setup_item>   uu_rlc_ch_failed_to_be_setup_list;   // max size = 32
  std::vector<f1ap_pc5_rlc_ch_setup_item>               pc5_rlc_ch_setup_list;               // max size = 512
  std::vector<f1ap_pc5_rlc_ch_failed_to_be_setup_item>  pc5_rlc_ch_failed_to_be_setup_list;  // max size = 512
  std::vector<f1ap_serving_cell_mo_encoded_in_cgc_item> serving_cell_mo_encoded_in_cgc_list; // max size = 8

  // UE Context Setup Failure
  optional<cause_t>                        cause;
  std::vector<f1ap_potential_sp_cell_item> potential_sp_cell_list; // max size = 64

  // Common
  optional<crit_diagnostics_t>  crit_diagnostics;
  optional<nr_cell_global_id_t> requested_target_cell_global_id;
};

/// \brief Response from F1AP-CU to CU once "UE Context Setup" procedure is complete.
struct f1a_p_ue_context_setup_response {
  asn1::f1ap::ue_context_setup_resp_s response;
  asn1::f1ap::ue_context_setup_fail_s failure;
  bool                                success = false;
};

struct f1ap_scell_to_be_setup_mod_item {
  nr_cell_global_id_t   scell_id;
  uint8_t               scell_idx;
  optional<std::string> scell_ul_cfg;
};

struct f1ap_scell_to_be_remd_item {
  nr_cell_global_id_t scell_id;
};

struct f1ap_srbs_to_be_setup_mod_item {
  srb_id_t              srb_id                   = srb_id_t::nulltype;
  bool                  reestablish_pdcp_present = false;
  optional<std::string> dupl_ind;
};

struct f1ap_flows_mapped_to_drb_item {
  qos_flow_id_t                   qos_flow_id = qos_flow_id_t::invalid;
  cu_cp_qos_flow_level_qos_params qos_flow_level_qos_params;
};

struct f1ap_drb_info {
  cu_cp_qos_flow_level_qos_params                                 drb_qos;
  s_nssai_t                                                       s_nssai;
  optional<std::string>                                           notif_ctrl;
  slotted_id_vector<qos_flow_id_t, f1ap_flows_mapped_to_drb_item> flows_mapped_to_drb_list;
};

struct f1ap_drbs_to_be_setup_mod_item {
  drb_id_t                             drb_id = drb_id_t::invalid;
  f1ap_drb_info                        qos_info;
  std::vector<up_transport_layer_info> ul_up_tnl_info_to_be_setup_list;
  srsran::rlc_mode                     rlc_mod;
  optional<f1ap_ul_cfg>                ul_cfg;
  optional<std::string>                dupl_activation;
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
  optional<std::string>                                       sp_cell_ul_cfg;
  optional<f1ap_drx_cycle>                                    drx_cycle;
  optional<f1ap_cu_to_du_rrc_info>                            cu_to_du_rrc_info;
  optional<std::string>                                       tx_action_ind;
  byte_buffer                                                 res_coordination_transfer_container;
  optional<std::string>                                       rrc_recfg_complete_ind;
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

struct f1ap_du_to_cu_rrc_info {
  byte_buffer cell_group_cfg;
  byte_buffer meas_gap_cfg;
  byte_buffer requested_p_max_fr1;
};

struct f1ap_drbs_setup_modified_item {
  drb_id_t                                          drb_id = drb_id_t::invalid;
  optional<lcid_t>                                  lcid   = lcid_t::INVALID_LCID;
  std::vector<f1ap_dl_up_tnl_info_to_be_setup_item> dl_up_tnl_info_to_be_setup_list;
};

struct f1ap_srbs_failed_to_be_setup_mod_item {
  srb_id_t          srb_id = srb_id_t::nulltype;
  optional<cause_t> cause;
};

struct f1ap_drbs_failed_to_be_setup_modified_item {
  drb_id_t          drb_id = drb_id_t::invalid;
  optional<cause_t> cause;
};

struct f1ap_associated_scell_item {
  nr_cell_id_t scell_id;
};

struct f1ap_srbs_setup_modified_item {
  srb_id_t srb_id = srb_id_t::nulltype;
  lcid_t   lcid   = lcid_t::INVALID_LCID;
};

/// \brief Response from F1AP-CU to CU once "UE Context Modification" procedure is complete.
struct f1ap_ue_context_modification_response {
  bool success = false;
  // ue context modification response
  byte_buffer                                                             res_coordination_transfer_container;
  f1ap_du_to_cu_rrc_info                                                  du_to_cu_rrc_info;
  slotted_id_vector<drb_id_t, f1ap_drbs_setup_modified_item>              drbs_setup_mod_list;
  slotted_id_vector<drb_id_t, f1ap_drbs_setup_modified_item>              drbs_modified_list;
  slotted_id_vector<srb_id_t, f1ap_srbs_failed_to_be_setup_mod_item>      srbs_failed_to_be_setup_mod_list;
  slotted_id_vector<drb_id_t, f1ap_drbs_failed_to_be_setup_modified_item> drbs_failed_to_be_setup_mod_list;
  std::vector<f1ap_scell_failed_to_setup_mod_item>                        scell_failed_to_setup_mod_list;
  slotted_id_vector<drb_id_t, f1ap_drbs_failed_to_be_setup_modified_item> drbs_failed_to_be_modified_list;
  optional<bool>                                                          inactivity_monitoring_resp;
  optional<srsran::rnti_t>                                                c_rnti;
  std::vector<f1ap_associated_scell_item>                                 associated_scell_list;
  slotted_id_vector<srb_id_t, f1ap_srbs_setup_modified_item>              srbs_setup_mod_list;
  slotted_id_vector<srb_id_t, f1ap_srbs_setup_modified_item>              srbs_modified_list;
  optional<bool>                                                          full_cfg;

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
