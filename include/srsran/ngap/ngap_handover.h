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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/security/security.h"

namespace srsran {
namespace srs_cu_cp {

struct ngap_ue_source_handover_context {
  std::vector<pdu_session_id_t> pdu_sessions;
  byte_buffer                   transparent_container;
};

struct ngap_handover_preparation_request {
  ue_index_t   ue_index = ue_index_t::invalid;
  unsigned     gnb_id;
  nr_cell_id_t nci;
};

struct ngap_handover_preparation_response {
  // Place-holder for possible return values.
  bool success = false;
};

enum class ngap_handov_type { intra5gs = 0, fivegs_to_eps, eps_to_5gs, fivegs_to_utran };

struct ngap_ue_aggr_max_bit_rate {
  uint64_t ue_aggr_max_bit_rate_dl;
  uint64_t ue_aggr_max_bit_rate_ul;
};

enum class ngap_source_of_ue_activity_behaviour_info { sub_info = 0, stats };

struct ngap_expected_ue_activity_behaviour {
  optional<uint8_t>                                   expected_activity_period;
  optional<uint8_t>                                   expected_idle_period;
  optional<ngap_source_of_ue_activity_behaviour_info> source_of_ue_activity_behaviour_info;
};

enum class ngap_expected_ue_mob { stationary = 0, mobile };

struct ngap_expected_ue_moving_trajectory_item {
  nr_cell_global_id_t ngran_cgi;
  optional<uint16_t>  time_stayed_in_cell;
};

struct ngap_expected_ue_behaviour {
  optional<ngap_expected_ue_activity_behaviour>        expected_ue_activity_behaviour;
  optional<uint8_t>                                    expected_ho_interv;
  ngap_expected_ue_mob                                 expected_ue_mob;
  std::vector<ngap_expected_ue_moving_trajectory_item> expected_ue_moving_trajectory;
};

struct ngap_core_network_assist_info_for_inactive {
  uint8_t                              ue_id_idx_value;
  optional<uint16_t>                   ue_specific_drx;
  uint64_t                             periodic_regist_upd_timer;
  optional<bool>                       mico_mode_ind;
  std::vector<cu_cp_tai>               tai_list_for_inactive;
  optional<ngap_expected_ue_behaviour> expected_ue_behaviour;
};

enum class ngap_trace_depth {
  minimum = 0,
  medium,
  max,
  minimum_without_vendor_specific_ext,
  medium_without_vendor_specific_ext,
  max_without_vendor_specific_ext
};

struct ngap_trace_activation {
  uint64_t         ngran_trace_id;
  uint64_t         interfaces_to_trace;
  ngap_trace_depth trace_depth;
  uint64_t         trace_collection_entity_ip_address;
};

struct ngap_qos_flow_info_item {
  qos_flow_id_t  qos_flow_id = qos_flow_id_t::invalid;
  optional<bool> dl_forwarding;
};

struct ngap_drbs_to_qos_flows_map_item {
  drb_id_t                               drb_id = drb_id_t::invalid;
  std::vector<cu_cp_associated_qos_flow> associated_qos_flow_list;
};

struct ngap_pdu_session_res_info_item {
  pdu_session_id_t                             pdu_session_id = pdu_session_id_t::invalid;
  std::vector<ngap_qos_flow_info_item>         qos_flow_info_list;
  std::vector<ngap_drbs_to_qos_flows_map_item> drbs_to_qos_flows_map_list;
};

struct ngap_erab_info_item {
  uint8_t        erab_id;
  optional<bool> dl_forwarding;
};

enum class ngap_cell_size { verysmall = 0, small, medium, large };

struct ngap_cell_type {
  ngap_cell_size cell_size;
};

struct ngap_last_visited_ngran_cell_info {
  nr_cell_global_id_t global_cell_id;
  ngap_cell_type      cell_type;
  uint16_t            time_ue_stayed_in_cell;
  optional<uint16_t>  time_ue_stayed_in_cell_enhanced_granularity;
  optional<cause_t>   ho_cause_value;
};

struct ngap_last_visited_cell_item {
  ngap_last_visited_ngran_cell_info last_visited_cell_info;
};

struct ngap_source_ngran_node_to_target_ngran_node_transparent_container {
  byte_buffer                                 rrc_container;
  std::vector<ngap_pdu_session_res_info_item> pdu_session_res_info_list;
  std::vector<ngap_erab_info_item>            erab_info_list;
  nr_cell_global_id_t                         target_cell_id;
  optional<uint16_t>                          idx_to_rfsp;
  std::vector<ngap_last_visited_cell_item>    ue_history_info;
};

struct ngap_rat_restrict_item {
  std::string plmn_id;
  uint64_t    rat_restrict_info;
};

struct ngap_forbidden_area_info_item {
  std::string           plmn_id;
  std::vector<unsigned> forbidden_tacs;
};

struct ngap_service_area_info_item {
  std::string           plmn_id;
  std::vector<unsigned> allowed_tacs;
  std::vector<unsigned> not_allowed_tacs;
};

struct ngap_mob_restrict_list {
  std::string                                serving_plmn;
  std::vector<std::string>                   equivalent_plmns;
  std::vector<ngap_rat_restrict_item>        rat_restricts;
  std::vector<ngap_forbidden_area_info_item> forbidden_area_info;
  std::vector<ngap_service_area_info_item>   service_area_info;
};

enum class ngap_event_type {
  direct = 0,
  change_of_serve_cell,
  ue_presence_in_area_of_interest,
  stop_change_of_serve_cell,
  stop_ue_presence_in_area_of_interest,
  cancel_location_report_for_the_ue
};

struct ngap_area_of_interest {
  std::vector<cu_cp_tai>           area_of_interest_tai_list;
  std::vector<nr_cell_global_id_t> area_of_interest_cell_list;
  std::vector<cu_cp_global_gnb_id> area_of_interest_ran_node_list;
};

struct ngap_area_of_interest_item {
  ngap_area_of_interest area_of_interest;
  uint8_t               location_report_ref_id;
};

struct ngap_location_report_request_type {
  ngap_event_type                         event_type;
  bool                                    report_area;
  std::vector<ngap_area_of_interest_item> area_of_interest_list;
  optional<uint8_t>                       location_report_ref_id_to_be_cancelled;
};

enum class ngap_rrc_inactive_transition_report_request {
  subsequent_state_transition_report = 0,
  single_rrc_connected_state_report,
  cancel_report
};

enum class ngap_redirection_voice_fallback { possible = 0, not_possible };

struct ngap_cn_assisted_ran_tuning {
  optional<ngap_expected_ue_behaviour> expected_ue_behaviour;
};

struct ngap_handover_request {
  ue_index_t                                                            ue_index = ue_index_t::invalid;
  ngap_handov_type                                                      handov_type;
  cause_t                                                               cause;
  ngap_ue_aggr_max_bit_rate                                             ue_aggr_max_bit_rate;
  optional<ngap_core_network_assist_info_for_inactive>                  core_network_assist_info_for_inactive;
  security::security_context                                            security_context;
  optional<bool>                                                        new_security_context_ind;
  byte_buffer                                                           nasc;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item> pdu_session_res_setup_list_ho_req;
  std::vector<s_nssai_t>                                                allowed_nssai;
  optional<ngap_trace_activation>                                       trace_activation;
  optional<uint64_t>                                                    masked_imeisv;
  ngap_source_ngran_node_to_target_ngran_node_transparent_container     source_to_target_transparent_container;
  optional<ngap_mob_restrict_list>                                      mob_restrict_list;
  optional<ngap_location_report_request_type>                           location_report_request_type;
  optional<ngap_rrc_inactive_transition_report_request>                 rrc_inactive_transition_report_request;
  guami_t                                                               guami;
  optional<ngap_redirection_voice_fallback>                             redirection_voice_fallback;
  optional<ngap_cn_assisted_ran_tuning>                                 cn_assisted_ran_tuning;
};

} // namespace srs_cu_cp
} // namespace srsran
