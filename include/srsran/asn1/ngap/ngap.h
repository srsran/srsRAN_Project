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

/*******************************************************************************
 *
 *                     3GPP TS ASN1 NGAP v17.4.0 (2023-03)
 *
 ******************************************************************************/

#pragma once

#include "common.h"
#include "ngap_pdu_contents.h"

namespace asn1 {
namespace ngap {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// NGAP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF NGAP-ELEMENTARY-PROCEDURE
struct ngap_elem_procs_o {
  // InitiatingMessage ::= OPEN TYPE
  struct init_msg_c {
    struct types_opts {
      enum options {
        amf_cfg_upd,
        broadcast_session_mod_request,
        broadcast_session_release_request,
        broadcast_session_setup_request,
        distribution_setup_request,
        distribution_release_request,
        ho_cancel,
        ho_required,
        ho_request,
        init_context_setup_request,
        multicast_session_activation_request,
        multicast_session_deactivation_request,
        multicast_session_upd_request,
        ng_reset,
        ng_setup_request,
        path_switch_request,
        pdu_session_res_modify_request,
        pdu_session_res_modify_ind,
        pdu_session_res_release_cmd,
        pdu_session_res_setup_request,
        pws_cancel_request,
        ran_cfg_upd,
        ue_context_mod_request,
        ue_context_release_cmd,
        ue_context_resume_request,
        ue_context_suspend_request,
        ue_radio_cap_check_request,
        ue_radio_cap_id_map_request,
        write_replace_warning_request,
        amf_cp_relocation_ind,
        amf_status_ind,
        broadcast_session_release_required,
        cell_traffic_trace,
        conn_establishment_ind,
        deactiv_trace,
        dl_nas_transport,
        dl_non_ue_associated_nrppa_transport,
        dl_ran_cfg_transfer,
        dl_ran_early_status_transfer,
        dl_ran_status_transfer,
        dl_rim_info_transfer,
        dl_ue_associated_nrppa_transport,
        error_ind,
        ho_notify,
        ho_success,
        init_ue_msg,
        location_report,
        location_report_ctrl,
        location_report_fail_ind,
        multicast_group_paging,
        nas_non_delivery_ind,
        overload_start,
        overload_stop,
        paging,
        pdu_session_res_notify,
        private_msg,
        pws_fail_ind,
        pws_restart_ind,
        ran_cp_relocation_ind,
        reroute_nas_request,
        retrieve_ue_info,
        rrc_inactive_transition_report,
        secondary_rat_data_usage_report,
        trace_fail_ind,
        trace_start,
        ue_context_release_request,
        ue_info_transfer,
        ue_radio_cap_info_ind,
        ue_tnla_binding_release_request,
        ul_nas_transport,
        ul_non_ue_associated_nrppa_transport,
        ul_ran_cfg_transfer,
        ul_ran_early_status_transfer,
        ul_ran_status_transfer,
        ul_rim_info_transfer,
        ul_ue_associated_nrppa_transport,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    init_msg_c() = default;
    init_msg_c(const init_msg_c& other);
    init_msg_c& operator=(const init_msg_c& other);
    ~init_msg_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    amf_cfg_upd_s&                                  amf_cfg_upd();
    broadcast_session_mod_request_s&                broadcast_session_mod_request();
    broadcast_session_release_request_s&            broadcast_session_release_request();
    broadcast_session_setup_request_s&              broadcast_session_setup_request();
    distribution_setup_request_s&                   distribution_setup_request();
    distribution_release_request_s&                 distribution_release_request();
    ho_cancel_s&                                    ho_cancel();
    ho_required_s&                                  ho_required();
    ho_request_s&                                   ho_request();
    init_context_setup_request_s&                   init_context_setup_request();
    multicast_session_activation_request_s&         multicast_session_activation_request();
    multicast_session_deactivation_request_s&       multicast_session_deactivation_request();
    multicast_session_upd_request_s&                multicast_session_upd_request();
    ng_reset_s&                                     ng_reset();
    ng_setup_request_s&                             ng_setup_request();
    path_switch_request_s&                          path_switch_request();
    pdu_session_res_modify_request_s&               pdu_session_res_modify_request();
    pdu_session_res_modify_ind_s&                   pdu_session_res_modify_ind();
    pdu_session_res_release_cmd_s&                  pdu_session_res_release_cmd();
    pdu_session_res_setup_request_s&                pdu_session_res_setup_request();
    pws_cancel_request_s&                           pws_cancel_request();
    ran_cfg_upd_s&                                  ran_cfg_upd();
    ue_context_mod_request_s&                       ue_context_mod_request();
    ue_context_release_cmd_s&                       ue_context_release_cmd();
    ue_context_resume_request_s&                    ue_context_resume_request();
    ue_context_suspend_request_s&                   ue_context_suspend_request();
    ue_radio_cap_check_request_s&                   ue_radio_cap_check_request();
    ue_radio_cap_id_map_request_s&                  ue_radio_cap_id_map_request();
    write_replace_warning_request_s&                write_replace_warning_request();
    amf_cp_relocation_ind_s&                        amf_cp_relocation_ind();
    amf_status_ind_s&                               amf_status_ind();
    broadcast_session_release_required_s&           broadcast_session_release_required();
    cell_traffic_trace_s&                           cell_traffic_trace();
    conn_establishment_ind_s&                       conn_establishment_ind();
    deactiv_trace_s&                                deactiv_trace();
    dl_nas_transport_s&                             dl_nas_transport();
    dl_non_ue_associated_nrppa_transport_s&         dl_non_ue_associated_nrppa_transport();
    dl_ran_cfg_transfer_s&                          dl_ran_cfg_transfer();
    dl_ran_early_status_transfer_s&                 dl_ran_early_status_transfer();
    dl_ran_status_transfer_s&                       dl_ran_status_transfer();
    dl_rim_info_transfer_s&                         dl_rim_info_transfer();
    dl_ue_associated_nrppa_transport_s&             dl_ue_associated_nrppa_transport();
    error_ind_s&                                    error_ind();
    ho_notify_s&                                    ho_notify();
    ho_success_s&                                   ho_success();
    init_ue_msg_s&                                  init_ue_msg();
    location_report_s&                              location_report();
    location_report_ctrl_s&                         location_report_ctrl();
    location_report_fail_ind_s&                     location_report_fail_ind();
    multicast_group_paging_s&                       multicast_group_paging();
    nas_non_delivery_ind_s&                         nas_non_delivery_ind();
    overload_start_s&                               overload_start();
    overload_stop_s&                                overload_stop();
    paging_s&                                       paging();
    pdu_session_res_notify_s&                       pdu_session_res_notify();
    private_msg_s&                                  private_msg();
    pws_fail_ind_s&                                 pws_fail_ind();
    pws_restart_ind_s&                              pws_restart_ind();
    ran_cp_relocation_ind_s&                        ran_cp_relocation_ind();
    reroute_nas_request_s&                          reroute_nas_request();
    retrieve_ue_info_s&                             retrieve_ue_info();
    rrc_inactive_transition_report_s&               rrc_inactive_transition_report();
    secondary_rat_data_usage_report_s&              secondary_rat_data_usage_report();
    trace_fail_ind_s&                               trace_fail_ind();
    trace_start_s&                                  trace_start();
    ue_context_release_request_s&                   ue_context_release_request();
    ue_info_transfer_s&                             ue_info_transfer();
    ue_radio_cap_info_ind_s&                        ue_radio_cap_info_ind();
    ue_tnla_binding_release_request_s&              ue_tnla_binding_release_request();
    ul_nas_transport_s&                             ul_nas_transport();
    ul_non_ue_associated_nrppa_transport_s&         ul_non_ue_associated_nrppa_transport();
    ul_ran_cfg_transfer_s&                          ul_ran_cfg_transfer();
    ul_ran_early_status_transfer_s&                 ul_ran_early_status_transfer();
    ul_ran_status_transfer_s&                       ul_ran_status_transfer();
    ul_rim_info_transfer_s&                         ul_rim_info_transfer();
    ul_ue_associated_nrppa_transport_s&             ul_ue_associated_nrppa_transport();
    const amf_cfg_upd_s&                            amf_cfg_upd() const;
    const broadcast_session_mod_request_s&          broadcast_session_mod_request() const;
    const broadcast_session_release_request_s&      broadcast_session_release_request() const;
    const broadcast_session_setup_request_s&        broadcast_session_setup_request() const;
    const distribution_setup_request_s&             distribution_setup_request() const;
    const distribution_release_request_s&           distribution_release_request() const;
    const ho_cancel_s&                              ho_cancel() const;
    const ho_required_s&                            ho_required() const;
    const ho_request_s&                             ho_request() const;
    const init_context_setup_request_s&             init_context_setup_request() const;
    const multicast_session_activation_request_s&   multicast_session_activation_request() const;
    const multicast_session_deactivation_request_s& multicast_session_deactivation_request() const;
    const multicast_session_upd_request_s&          multicast_session_upd_request() const;
    const ng_reset_s&                               ng_reset() const;
    const ng_setup_request_s&                       ng_setup_request() const;
    const path_switch_request_s&                    path_switch_request() const;
    const pdu_session_res_modify_request_s&         pdu_session_res_modify_request() const;
    const pdu_session_res_modify_ind_s&             pdu_session_res_modify_ind() const;
    const pdu_session_res_release_cmd_s&            pdu_session_res_release_cmd() const;
    const pdu_session_res_setup_request_s&          pdu_session_res_setup_request() const;
    const pws_cancel_request_s&                     pws_cancel_request() const;
    const ran_cfg_upd_s&                            ran_cfg_upd() const;
    const ue_context_mod_request_s&                 ue_context_mod_request() const;
    const ue_context_release_cmd_s&                 ue_context_release_cmd() const;
    const ue_context_resume_request_s&              ue_context_resume_request() const;
    const ue_context_suspend_request_s&             ue_context_suspend_request() const;
    const ue_radio_cap_check_request_s&             ue_radio_cap_check_request() const;
    const ue_radio_cap_id_map_request_s&            ue_radio_cap_id_map_request() const;
    const write_replace_warning_request_s&          write_replace_warning_request() const;
    const amf_cp_relocation_ind_s&                  amf_cp_relocation_ind() const;
    const amf_status_ind_s&                         amf_status_ind() const;
    const broadcast_session_release_required_s&     broadcast_session_release_required() const;
    const cell_traffic_trace_s&                     cell_traffic_trace() const;
    const conn_establishment_ind_s&                 conn_establishment_ind() const;
    const deactiv_trace_s&                          deactiv_trace() const;
    const dl_nas_transport_s&                       dl_nas_transport() const;
    const dl_non_ue_associated_nrppa_transport_s&   dl_non_ue_associated_nrppa_transport() const;
    const dl_ran_cfg_transfer_s&                    dl_ran_cfg_transfer() const;
    const dl_ran_early_status_transfer_s&           dl_ran_early_status_transfer() const;
    const dl_ran_status_transfer_s&                 dl_ran_status_transfer() const;
    const dl_rim_info_transfer_s&                   dl_rim_info_transfer() const;
    const dl_ue_associated_nrppa_transport_s&       dl_ue_associated_nrppa_transport() const;
    const error_ind_s&                              error_ind() const;
    const ho_notify_s&                              ho_notify() const;
    const ho_success_s&                             ho_success() const;
    const init_ue_msg_s&                            init_ue_msg() const;
    const location_report_s&                        location_report() const;
    const location_report_ctrl_s&                   location_report_ctrl() const;
    const location_report_fail_ind_s&               location_report_fail_ind() const;
    const multicast_group_paging_s&                 multicast_group_paging() const;
    const nas_non_delivery_ind_s&                   nas_non_delivery_ind() const;
    const overload_start_s&                         overload_start() const;
    const overload_stop_s&                          overload_stop() const;
    const paging_s&                                 paging() const;
    const pdu_session_res_notify_s&                 pdu_session_res_notify() const;
    const private_msg_s&                            private_msg() const;
    const pws_fail_ind_s&                           pws_fail_ind() const;
    const pws_restart_ind_s&                        pws_restart_ind() const;
    const ran_cp_relocation_ind_s&                  ran_cp_relocation_ind() const;
    const reroute_nas_request_s&                    reroute_nas_request() const;
    const retrieve_ue_info_s&                       retrieve_ue_info() const;
    const rrc_inactive_transition_report_s&         rrc_inactive_transition_report() const;
    const secondary_rat_data_usage_report_s&        secondary_rat_data_usage_report() const;
    const trace_fail_ind_s&                         trace_fail_ind() const;
    const trace_start_s&                            trace_start() const;
    const ue_context_release_request_s&             ue_context_release_request() const;
    const ue_info_transfer_s&                       ue_info_transfer() const;
    const ue_radio_cap_info_ind_s&                  ue_radio_cap_info_ind() const;
    const ue_tnla_binding_release_request_s&        ue_tnla_binding_release_request() const;
    const ul_nas_transport_s&                       ul_nas_transport() const;
    const ul_non_ue_associated_nrppa_transport_s&   ul_non_ue_associated_nrppa_transport() const;
    const ul_ran_cfg_transfer_s&                    ul_ran_cfg_transfer() const;
    const ul_ran_early_status_transfer_s&           ul_ran_early_status_transfer() const;
    const ul_ran_status_transfer_s&                 ul_ran_status_transfer() const;
    const ul_rim_info_transfer_s&                   ul_rim_info_transfer() const;
    const ul_ue_associated_nrppa_transport_s&       ul_ue_associated_nrppa_transport() const;

  private:
    types type_;
    choice_buffer_t<amf_cfg_upd_s,
                    amf_cp_relocation_ind_s,
                    amf_status_ind_s,
                    broadcast_session_mod_request_s,
                    broadcast_session_release_request_s,
                    broadcast_session_release_required_s,
                    broadcast_session_setup_request_s,
                    cell_traffic_trace_s,
                    conn_establishment_ind_s,
                    deactiv_trace_s,
                    distribution_release_request_s,
                    distribution_setup_request_s,
                    dl_nas_transport_s,
                    dl_non_ue_associated_nrppa_transport_s,
                    dl_ran_cfg_transfer_s,
                    dl_ran_early_status_transfer_s,
                    dl_ran_status_transfer_s,
                    dl_rim_info_transfer_s,
                    dl_ue_associated_nrppa_transport_s,
                    error_ind_s,
                    ho_cancel_s,
                    ho_notify_s,
                    ho_request_s,
                    ho_required_s,
                    ho_success_s,
                    init_context_setup_request_s,
                    init_ue_msg_s,
                    location_report_ctrl_s,
                    location_report_fail_ind_s,
                    location_report_s,
                    multicast_group_paging_s,
                    multicast_session_activation_request_s,
                    multicast_session_deactivation_request_s,
                    multicast_session_upd_request_s,
                    nas_non_delivery_ind_s,
                    ng_reset_s,
                    ng_setup_request_s,
                    overload_start_s,
                    overload_stop_s,
                    paging_s,
                    path_switch_request_s,
                    pdu_session_res_modify_ind_s,
                    pdu_session_res_modify_request_s,
                    pdu_session_res_notify_s,
                    pdu_session_res_release_cmd_s,
                    pdu_session_res_setup_request_s,
                    private_msg_s,
                    pws_cancel_request_s,
                    pws_fail_ind_s,
                    pws_restart_ind_s,
                    ran_cfg_upd_s,
                    ran_cp_relocation_ind_s,
                    reroute_nas_request_s,
                    retrieve_ue_info_s,
                    rrc_inactive_transition_report_s,
                    secondary_rat_data_usage_report_s,
                    trace_fail_ind_s,
                    trace_start_s,
                    ue_context_mod_request_s,
                    ue_context_release_cmd_s,
                    ue_context_release_request_s,
                    ue_context_resume_request_s,
                    ue_context_suspend_request_s,
                    ue_info_transfer_s,
                    ue_radio_cap_check_request_s,
                    ue_radio_cap_id_map_request_s,
                    ue_radio_cap_info_ind_s,
                    ue_tnla_binding_release_request_s,
                    ul_nas_transport_s,
                    ul_non_ue_associated_nrppa_transport_s,
                    ul_ran_cfg_transfer_s,
                    ul_ran_early_status_transfer_s,
                    ul_ran_status_transfer_s,
                    ul_rim_info_transfer_s,
                    ul_ue_associated_nrppa_transport_s,
                    write_replace_warning_request_s>
        c;

    void destroy_();
  };
  // SuccessfulOutcome ::= OPEN TYPE
  struct successful_outcome_c {
    struct types_opts {
      enum options {
        amf_cfg_upd_ack,
        broadcast_session_mod_resp,
        broadcast_session_release_resp,
        broadcast_session_setup_resp,
        distribution_setup_resp,
        distribution_release_resp,
        ho_cancel_ack,
        ho_cmd,
        ho_request_ack,
        init_context_setup_resp,
        multicast_session_activation_resp,
        multicast_session_deactivation_resp,
        multicast_session_upd_resp,
        ng_reset_ack,
        ng_setup_resp,
        path_switch_request_ack,
        pdu_session_res_modify_resp,
        pdu_session_res_modify_confirm,
        pdu_session_res_release_resp,
        pdu_session_res_setup_resp,
        pws_cancel_resp,
        ran_cfg_upd_ack,
        ue_context_mod_resp,
        ue_context_release_complete,
        ue_context_resume_resp,
        ue_context_suspend_resp,
        ue_radio_cap_check_resp,
        ue_radio_cap_id_map_resp,
        write_replace_warning_resp,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    successful_outcome_c() = default;
    successful_outcome_c(const successful_outcome_c& other);
    successful_outcome_c& operator=(const successful_outcome_c& other);
    ~successful_outcome_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    amf_cfg_upd_ack_s&                           amf_cfg_upd_ack();
    broadcast_session_mod_resp_s&                broadcast_session_mod_resp();
    broadcast_session_release_resp_s&            broadcast_session_release_resp();
    broadcast_session_setup_resp_s&              broadcast_session_setup_resp();
    distribution_setup_resp_s&                   distribution_setup_resp();
    distribution_release_resp_s&                 distribution_release_resp();
    ho_cancel_ack_s&                             ho_cancel_ack();
    ho_cmd_s&                                    ho_cmd();
    ho_request_ack_s&                            ho_request_ack();
    init_context_setup_resp_s&                   init_context_setup_resp();
    multicast_session_activation_resp_s&         multicast_session_activation_resp();
    multicast_session_deactivation_resp_s&       multicast_session_deactivation_resp();
    multicast_session_upd_resp_s&                multicast_session_upd_resp();
    ng_reset_ack_s&                              ng_reset_ack();
    ng_setup_resp_s&                             ng_setup_resp();
    path_switch_request_ack_s&                   path_switch_request_ack();
    pdu_session_res_modify_resp_s&               pdu_session_res_modify_resp();
    pdu_session_res_modify_confirm_s&            pdu_session_res_modify_confirm();
    pdu_session_res_release_resp_s&              pdu_session_res_release_resp();
    pdu_session_res_setup_resp_s&                pdu_session_res_setup_resp();
    pws_cancel_resp_s&                           pws_cancel_resp();
    ran_cfg_upd_ack_s&                           ran_cfg_upd_ack();
    ue_context_mod_resp_s&                       ue_context_mod_resp();
    ue_context_release_complete_s&               ue_context_release_complete();
    ue_context_resume_resp_s&                    ue_context_resume_resp();
    ue_context_suspend_resp_s&                   ue_context_suspend_resp();
    ue_radio_cap_check_resp_s&                   ue_radio_cap_check_resp();
    ue_radio_cap_id_map_resp_s&                  ue_radio_cap_id_map_resp();
    write_replace_warning_resp_s&                write_replace_warning_resp();
    const amf_cfg_upd_ack_s&                     amf_cfg_upd_ack() const;
    const broadcast_session_mod_resp_s&          broadcast_session_mod_resp() const;
    const broadcast_session_release_resp_s&      broadcast_session_release_resp() const;
    const broadcast_session_setup_resp_s&        broadcast_session_setup_resp() const;
    const distribution_setup_resp_s&             distribution_setup_resp() const;
    const distribution_release_resp_s&           distribution_release_resp() const;
    const ho_cancel_ack_s&                       ho_cancel_ack() const;
    const ho_cmd_s&                              ho_cmd() const;
    const ho_request_ack_s&                      ho_request_ack() const;
    const init_context_setup_resp_s&             init_context_setup_resp() const;
    const multicast_session_activation_resp_s&   multicast_session_activation_resp() const;
    const multicast_session_deactivation_resp_s& multicast_session_deactivation_resp() const;
    const multicast_session_upd_resp_s&          multicast_session_upd_resp() const;
    const ng_reset_ack_s&                        ng_reset_ack() const;
    const ng_setup_resp_s&                       ng_setup_resp() const;
    const path_switch_request_ack_s&             path_switch_request_ack() const;
    const pdu_session_res_modify_resp_s&         pdu_session_res_modify_resp() const;
    const pdu_session_res_modify_confirm_s&      pdu_session_res_modify_confirm() const;
    const pdu_session_res_release_resp_s&        pdu_session_res_release_resp() const;
    const pdu_session_res_setup_resp_s&          pdu_session_res_setup_resp() const;
    const pws_cancel_resp_s&                     pws_cancel_resp() const;
    const ran_cfg_upd_ack_s&                     ran_cfg_upd_ack() const;
    const ue_context_mod_resp_s&                 ue_context_mod_resp() const;
    const ue_context_release_complete_s&         ue_context_release_complete() const;
    const ue_context_resume_resp_s&              ue_context_resume_resp() const;
    const ue_context_suspend_resp_s&             ue_context_suspend_resp() const;
    const ue_radio_cap_check_resp_s&             ue_radio_cap_check_resp() const;
    const ue_radio_cap_id_map_resp_s&            ue_radio_cap_id_map_resp() const;
    const write_replace_warning_resp_s&          write_replace_warning_resp() const;

  private:
    types type_;
    choice_buffer_t<amf_cfg_upd_ack_s,
                    broadcast_session_mod_resp_s,
                    broadcast_session_release_resp_s,
                    broadcast_session_setup_resp_s,
                    distribution_release_resp_s,
                    distribution_setup_resp_s,
                    ho_cancel_ack_s,
                    ho_cmd_s,
                    ho_request_ack_s,
                    init_context_setup_resp_s,
                    multicast_session_activation_resp_s,
                    multicast_session_deactivation_resp_s,
                    multicast_session_upd_resp_s,
                    ng_reset_ack_s,
                    ng_setup_resp_s,
                    path_switch_request_ack_s,
                    pdu_session_res_modify_confirm_s,
                    pdu_session_res_modify_resp_s,
                    pdu_session_res_release_resp_s,
                    pdu_session_res_setup_resp_s,
                    pws_cancel_resp_s,
                    ran_cfg_upd_ack_s,
                    ue_context_mod_resp_s,
                    ue_context_release_complete_s,
                    ue_context_resume_resp_s,
                    ue_context_suspend_resp_s,
                    ue_radio_cap_check_resp_s,
                    ue_radio_cap_id_map_resp_s,
                    write_replace_warning_resp_s>
        c;

    void destroy_();
  };
  // UnsuccessfulOutcome ::= OPEN TYPE
  struct unsuccessful_outcome_c {
    struct types_opts {
      enum options {
        amf_cfg_upd_fail,
        broadcast_session_mod_fail,
        broadcast_session_setup_fail,
        distribution_setup_fail,
        ho_prep_fail,
        ho_fail,
        init_context_setup_fail,
        multicast_session_activation_fail,
        multicast_session_upd_fail,
        ng_setup_fail,
        path_switch_request_fail,
        ran_cfg_upd_fail,
        ue_context_mod_fail,
        ue_context_resume_fail,
        ue_context_suspend_fail,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    unsuccessful_outcome_c() = default;
    unsuccessful_outcome_c(const unsuccessful_outcome_c& other);
    unsuccessful_outcome_c& operator=(const unsuccessful_outcome_c& other);
    ~unsuccessful_outcome_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    amf_cfg_upd_fail_s&                        amf_cfg_upd_fail();
    broadcast_session_mod_fail_s&              broadcast_session_mod_fail();
    broadcast_session_setup_fail_s&            broadcast_session_setup_fail();
    distribution_setup_fail_s&                 distribution_setup_fail();
    ho_prep_fail_s&                            ho_prep_fail();
    ho_fail_s&                                 ho_fail();
    init_context_setup_fail_s&                 init_context_setup_fail();
    multicast_session_activation_fail_s&       multicast_session_activation_fail();
    multicast_session_upd_fail_s&              multicast_session_upd_fail();
    ng_setup_fail_s&                           ng_setup_fail();
    path_switch_request_fail_s&                path_switch_request_fail();
    ran_cfg_upd_fail_s&                        ran_cfg_upd_fail();
    ue_context_mod_fail_s&                     ue_context_mod_fail();
    ue_context_resume_fail_s&                  ue_context_resume_fail();
    ue_context_suspend_fail_s&                 ue_context_suspend_fail();
    const amf_cfg_upd_fail_s&                  amf_cfg_upd_fail() const;
    const broadcast_session_mod_fail_s&        broadcast_session_mod_fail() const;
    const broadcast_session_setup_fail_s&      broadcast_session_setup_fail() const;
    const distribution_setup_fail_s&           distribution_setup_fail() const;
    const ho_prep_fail_s&                      ho_prep_fail() const;
    const ho_fail_s&                           ho_fail() const;
    const init_context_setup_fail_s&           init_context_setup_fail() const;
    const multicast_session_activation_fail_s& multicast_session_activation_fail() const;
    const multicast_session_upd_fail_s&        multicast_session_upd_fail() const;
    const ng_setup_fail_s&                     ng_setup_fail() const;
    const path_switch_request_fail_s&          path_switch_request_fail() const;
    const ran_cfg_upd_fail_s&                  ran_cfg_upd_fail() const;
    const ue_context_mod_fail_s&               ue_context_mod_fail() const;
    const ue_context_resume_fail_s&            ue_context_resume_fail() const;
    const ue_context_suspend_fail_s&           ue_context_suspend_fail() const;

  private:
    types type_;
    choice_buffer_t<amf_cfg_upd_fail_s,
                    broadcast_session_mod_fail_s,
                    broadcast_session_setup_fail_s,
                    distribution_setup_fail_s,
                    ho_fail_s,
                    ho_prep_fail_s,
                    init_context_setup_fail_s,
                    multicast_session_activation_fail_s,
                    multicast_session_upd_fail_s,
                    ng_setup_fail_s,
                    path_switch_request_fail_s,
                    ran_cfg_upd_fail_s,
                    ue_context_mod_fail_s,
                    ue_context_resume_fail_s,
                    ue_context_suspend_fail_s>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint16_t               idx_to_proc_code(uint32_t idx);
  static bool                   is_proc_code_valid(const uint16_t& proc_code);
  static init_msg_c             get_init_msg(const uint16_t& proc_code);
  static successful_outcome_c   get_successful_outcome(const uint16_t& proc_code);
  static unsuccessful_outcome_c get_unsuccessful_outcome(const uint16_t& proc_code);
  static crit_e                 get_crit(const uint16_t& proc_code);
};

// InitiatingMessage ::= SEQUENCE{{NGAP-ELEMENTARY-PROCEDURE}}
struct init_msg_s {
  uint16_t                      proc_code = 0;
  crit_e                        crit;
  ngap_elem_procs_o::init_msg_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// SuccessfulOutcome ::= SEQUENCE{{NGAP-ELEMENTARY-PROCEDURE}}
struct successful_outcome_s {
  uint16_t                                proc_code = 0;
  crit_e                                  crit;
  ngap_elem_procs_o::successful_outcome_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// UnsuccessfulOutcome ::= SEQUENCE{{NGAP-ELEMENTARY-PROCEDURE}}
struct unsuccessful_outcome_s {
  uint16_t                                  proc_code = 0;
  crit_e                                    crit;
  ngap_elem_procs_o::unsuccessful_outcome_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// NGAP-PDU ::= CHOICE
struct ngap_pdu_c {
  struct types_opts {
    enum options { init_msg, successful_outcome, unsuccessful_outcome, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  ngap_pdu_c() = default;
  ngap_pdu_c(const ngap_pdu_c& other);
  ngap_pdu_c& operator=(const ngap_pdu_c& other);
  ~ngap_pdu_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  init_msg_s& init_msg()
  {
    assert_choice_type(types::init_msg, type_, "NGAP-PDU");
    return c.get<init_msg_s>();
  }
  successful_outcome_s& successful_outcome()
  {
    assert_choice_type(types::successful_outcome, type_, "NGAP-PDU");
    return c.get<successful_outcome_s>();
  }
  unsuccessful_outcome_s& unsuccessful_outcome()
  {
    assert_choice_type(types::unsuccessful_outcome, type_, "NGAP-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  const init_msg_s& init_msg() const
  {
    assert_choice_type(types::init_msg, type_, "NGAP-PDU");
    return c.get<init_msg_s>();
  }
  const successful_outcome_s& successful_outcome() const
  {
    assert_choice_type(types::successful_outcome, type_, "NGAP-PDU");
    return c.get<successful_outcome_s>();
  }
  const unsuccessful_outcome_s& unsuccessful_outcome() const
  {
    assert_choice_type(types::unsuccessful_outcome, type_, "NGAP-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  init_msg_s&             set_init_msg();
  successful_outcome_s&   set_successful_outcome();
  unsuccessful_outcome_s& set_unsuccessful_outcome();

private:
  types                                                                     type_;
  choice_buffer_t<init_msg_s, successful_outcome_s, unsuccessful_outcome_s> c;

  void destroy_();
};

// ProtocolIE-FieldPair{NGAP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE{{NGAP-PROTOCOL-IES-PAIR}}
template <class ies_set_paramT_>
struct protocol_ie_field_pair_s {
  uint32_t                                 id = 0;
  crit_e                                   first_crit;
  typename ies_set_paramT_::first_value_c  first_value;
  crit_e                                   second_crit;
  typename ies_set_paramT_::second_value_c second_value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint32_t& id_);
};

// ProtocolIE-ContainerPair{NGAP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE (SIZE (0..65535)) OF ProtocolIE-FieldPair
template <class ies_set_paramT_>
using protocol_ie_container_pair_l = dyn_seq_of<protocol_ie_field_pair_s<ies_set_paramT_>, 0, 65535, true>;

} // namespace ngap
} // namespace asn1

extern template struct asn1::protocol_ie_field_s<asn1::ngap::amf_cp_relocation_ind_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::plmn_support_item_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::amf_cfg_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::amf_cfg_upd_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::amf_cfg_upd_fail_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::assist_data_for_paging_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::broadcast_plmn_item_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::broadcast_session_mod_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::broadcast_session_mod_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::broadcast_session_mod_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::broadcast_session_release_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::broadcast_session_release_required_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::broadcast_session_release_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::broadcast_session_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::broadcast_session_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::broadcast_session_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::cell_traffic_trace_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::conn_establishment_ind_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::core_network_assist_info_for_inactive_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::deactiv_trace_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::distribution_release_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::distribution_release_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::distribution_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::distribution_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::distribution_setup_resp_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::mob_restrict_list_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::dl_nas_transport_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::dl_non_ue_associated_nrppa_transport_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::dl_ran_cfg_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::dl_ran_early_status_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::dl_ran_status_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::dl_ue_associated_nrppa_transport_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::dyn_5qi_descriptor_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::erab_info_item_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::error_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ho_cancel_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ho_cancel_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ho_cmd_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::ho_cmd_transfer_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ho_fail_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::user_location_info_nr_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ho_notify_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ho_prep_fail_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::m1_cfg_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::m6_cfg_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::trace_activation_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ho_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ho_request_ack_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::ho_request_ack_transfer_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ho_required_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ho_success_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::init_context_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::init_context_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::init_context_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::init_ue_msg_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::pdu_session_res_modify_item_mod_req_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::supported_ta_item_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::location_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::location_report_ctrl_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::location_report_fail_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::multicast_group_paging_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::multicast_session_activation_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::multicast_session_activation_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::multicast_session_activation_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::multicast_session_deactivation_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::multicast_session_deactivation_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::multicast_session_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::multicast_session_upd_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::multicast_session_upd_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::nas_non_delivery_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ng_reset_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ng_reset_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ng_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ng_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ng_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::overload_start_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pdu_session_res_modify_confirm_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pdu_session_res_modify_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pdu_session_res_modify_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pdu_session_res_modify_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pdu_session_res_notify_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pdu_session_res_release_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pdu_session_res_release_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pdu_session_res_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pdu_session_res_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pws_cancel_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pws_cancel_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pws_fail_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pws_restart_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::paging_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::path_switch_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::path_switch_request_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::path_switch_request_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ran_cp_relocation_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ran_cfg_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ran_cfg_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::rrc_inactive_transition_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::reroute_nas_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::secondary_rat_data_usage_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::trace_fail_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::trace_start_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_mod_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_mod_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_mod_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_release_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_release_complete_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_release_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_resume_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_resume_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_resume_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_suspend_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_suspend_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_suspend_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_info_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_radio_cap_check_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_radio_cap_check_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_radio_cap_id_map_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_radio_cap_info_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_tnla_binding_release_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ul_nas_transport_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ul_non_ue_associated_nrppa_transport_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ul_ran_cfg_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ul_ran_early_status_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ul_ran_status_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ul_ue_associated_nrppa_transport_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::write_replace_warning_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::write_replace_warning_resp_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::non_dyn_5qi_descriptor_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::qos_flow_level_qos_params_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::mbs_session_setup_or_mod_request_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::multicast_session_upd_request_transfer_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::qos_flow_info_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::pdu_session_res_modify_confirm_transfer_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::pdu_session_res_modify_ind_transfer_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::qos_flow_add_or_modify_request_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::ul_ngu_up_tnl_modify_item_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pdu_session_res_modify_request_transfer_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::pdu_session_res_modify_resp_transfer_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::pdu_session_res_notify_transfer_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::qos_flow_setup_request_item_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pdu_session_res_setup_request_transfer_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::pdu_session_res_setup_resp_transfer_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::qos_flow_params_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::path_switch_request_ack_transfer_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::ngap::path_switch_request_transfer_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<
    asn1::ngap::source_ngran_node_to_target_ngran_node_transparent_container_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<
    asn1::ngap::target_ngran_node_to_source_ngran_node_transparent_container_ext_ies_o>;
