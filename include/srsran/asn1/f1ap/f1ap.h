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
 *                     3GPP TS ASN1 F1AP v17.4.1 (2023-04)
 *
 ******************************************************************************/

#pragma once

#include "common.h"
#include "f1ap_pdu_contents.h"

namespace asn1 {
namespace f1ap {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// F1AP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF F1AP-ELEMENTARY-PROCEDURE
struct f1ap_elem_procs_o {
  // InitiatingMessage ::= OPEN TYPE
  struct init_msg_c {
    struct types_opts {
      enum options {
        reset,
        f1_setup_request,
        gnb_du_cfg_upd,
        gnb_cu_cfg_upd,
        ue_context_setup_request,
        ue_context_release_cmd,
        ue_context_mod_request,
        ue_context_mod_required,
        write_replace_warning_request,
        pws_cancel_request,
        gnb_du_res_coordination_request,
        f1_removal_request,
        bap_map_cfg,
        gnb_du_res_cfg,
        iab_tnl_address_request,
        iab_up_cfg_upd_request,
        res_status_request,
        positioning_meas_request,
        trp_info_request,
        positioning_info_request,
        positioning_activation_request,
        e_c_id_meas_initiation_request,
        broadcast_context_setup_request,
        broadcast_context_release_cmd,
        broadcast_context_mod_request,
        multicast_context_setup_request,
        multicast_context_release_cmd,
        multicast_context_mod_request,
        multicast_distribution_setup_request,
        multicast_distribution_release_cmd,
        pdc_meas_initiation_request,
        prs_cfg_request,
        meas_precfg_required,
        error_ind,
        ue_context_release_request,
        dl_rrc_msg_transfer,
        ul_rrc_msg_transfer,
        ue_inactivity_notif,
        private_msg,
        init_ul_rrc_msg_transfer,
        sys_info_delivery_cmd,
        paging,
        notify,
        pws_restart_ind,
        pws_fail_ind,
        gnb_du_status_ind,
        rrc_delivery_report,
        network_access_rate_reduction,
        trace_start,
        deactiv_trace,
        du_cu_radio_info_transfer,
        cu_du_radio_info_transfer,
        res_status_upd,
        access_and_mob_ind,
        ref_time_info_report_ctrl,
        ref_time_info_report,
        access_success,
        cell_traffic_trace,
        positioning_assist_info_ctrl,
        positioning_assist_info_feedback,
        positioning_meas_report,
        positioning_meas_abort,
        positioning_meas_fail_ind,
        positioning_meas_upd,
        positioning_deactivation,
        e_c_id_meas_fail_ind,
        e_c_id_meas_report,
        e_c_id_meas_termination_cmd,
        positioning_info_upd,
        multicast_group_paging,
        broadcast_context_release_request,
        multicast_context_release_request,
        pdc_meas_report,
        pdc_meas_termination_cmd,
        pdc_meas_fail_ind,
        meas_activation,
        qo_e_info_transfer,
        pos_sys_info_delivery_cmd,
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
    reset_s&                                      reset();
    f1_setup_request_s&                           f1_setup_request();
    gnb_du_cfg_upd_s&                             gnb_du_cfg_upd();
    gnb_cu_cfg_upd_s&                             gnb_cu_cfg_upd();
    ue_context_setup_request_s&                   ue_context_setup_request();
    ue_context_release_cmd_s&                     ue_context_release_cmd();
    ue_context_mod_request_s&                     ue_context_mod_request();
    ue_context_mod_required_s&                    ue_context_mod_required();
    write_replace_warning_request_s&              write_replace_warning_request();
    pws_cancel_request_s&                         pws_cancel_request();
    gnb_du_res_coordination_request_s&            gnb_du_res_coordination_request();
    f1_removal_request_s&                         f1_removal_request();
    bap_map_cfg_s&                                bap_map_cfg();
    gnb_du_res_cfg_s&                             gnb_du_res_cfg();
    iab_tnl_address_request_s&                    iab_tnl_address_request();
    iab_up_cfg_upd_request_s&                     iab_up_cfg_upd_request();
    res_status_request_s&                         res_status_request();
    positioning_meas_request_s&                   positioning_meas_request();
    trp_info_request_s&                           trp_info_request();
    positioning_info_request_s&                   positioning_info_request();
    positioning_activation_request_s&             positioning_activation_request();
    e_c_id_meas_initiation_request_s&             e_c_id_meas_initiation_request();
    broadcast_context_setup_request_s&            broadcast_context_setup_request();
    broadcast_context_release_cmd_s&              broadcast_context_release_cmd();
    broadcast_context_mod_request_s&              broadcast_context_mod_request();
    multicast_context_setup_request_s&            multicast_context_setup_request();
    multicast_context_release_cmd_s&              multicast_context_release_cmd();
    multicast_context_mod_request_s&              multicast_context_mod_request();
    multicast_distribution_setup_request_s&       multicast_distribution_setup_request();
    multicast_distribution_release_cmd_s&         multicast_distribution_release_cmd();
    pdc_meas_initiation_request_s&                pdc_meas_initiation_request();
    prs_cfg_request_s&                            prs_cfg_request();
    meas_precfg_required_s&                       meas_precfg_required();
    error_ind_s&                                  error_ind();
    ue_context_release_request_s&                 ue_context_release_request();
    dl_rrc_msg_transfer_s&                        dl_rrc_msg_transfer();
    ul_rrc_msg_transfer_s&                        ul_rrc_msg_transfer();
    ue_inactivity_notif_s&                        ue_inactivity_notif();
    private_msg_s&                                private_msg();
    init_ul_rrc_msg_transfer_s&                   init_ul_rrc_msg_transfer();
    sys_info_delivery_cmd_s&                      sys_info_delivery_cmd();
    paging_s&                                     paging();
    notify_s&                                     notify();
    pws_restart_ind_s&                            pws_restart_ind();
    pws_fail_ind_s&                               pws_fail_ind();
    gnb_du_status_ind_s&                          gnb_du_status_ind();
    rrc_delivery_report_s&                        rrc_delivery_report();
    network_access_rate_reduction_s&              network_access_rate_reduction();
    trace_start_s&                                trace_start();
    deactiv_trace_s&                              deactiv_trace();
    du_cu_radio_info_transfer_s&                  du_cu_radio_info_transfer();
    cu_du_radio_info_transfer_s&                  cu_du_radio_info_transfer();
    res_status_upd_s&                             res_status_upd();
    access_and_mob_ind_s&                         access_and_mob_ind();
    ref_time_info_report_ctrl_s&                  ref_time_info_report_ctrl();
    ref_time_info_report_s&                       ref_time_info_report();
    access_success_s&                             access_success();
    cell_traffic_trace_s&                         cell_traffic_trace();
    positioning_assist_info_ctrl_s&               positioning_assist_info_ctrl();
    positioning_assist_info_feedback_s&           positioning_assist_info_feedback();
    positioning_meas_report_s&                    positioning_meas_report();
    positioning_meas_abort_s&                     positioning_meas_abort();
    positioning_meas_fail_ind_s&                  positioning_meas_fail_ind();
    positioning_meas_upd_s&                       positioning_meas_upd();
    positioning_deactivation_s&                   positioning_deactivation();
    e_c_id_meas_fail_ind_s&                       e_c_id_meas_fail_ind();
    e_c_id_meas_report_s&                         e_c_id_meas_report();
    e_c_id_meas_termination_cmd_s&                e_c_id_meas_termination_cmd();
    positioning_info_upd_s&                       positioning_info_upd();
    multicast_group_paging_s&                     multicast_group_paging();
    broadcast_context_release_request_s&          broadcast_context_release_request();
    multicast_context_release_request_s&          multicast_context_release_request();
    pdc_meas_report_s&                            pdc_meas_report();
    pdc_meas_termination_cmd_s&                   pdc_meas_termination_cmd();
    pdc_meas_fail_ind_s&                          pdc_meas_fail_ind();
    meas_activation_s&                            meas_activation();
    qo_e_info_transfer_s&                         qo_e_info_transfer();
    pos_sys_info_delivery_cmd_s&                  pos_sys_info_delivery_cmd();
    const reset_s&                                reset() const;
    const f1_setup_request_s&                     f1_setup_request() const;
    const gnb_du_cfg_upd_s&                       gnb_du_cfg_upd() const;
    const gnb_cu_cfg_upd_s&                       gnb_cu_cfg_upd() const;
    const ue_context_setup_request_s&             ue_context_setup_request() const;
    const ue_context_release_cmd_s&               ue_context_release_cmd() const;
    const ue_context_mod_request_s&               ue_context_mod_request() const;
    const ue_context_mod_required_s&              ue_context_mod_required() const;
    const write_replace_warning_request_s&        write_replace_warning_request() const;
    const pws_cancel_request_s&                   pws_cancel_request() const;
    const gnb_du_res_coordination_request_s&      gnb_du_res_coordination_request() const;
    const f1_removal_request_s&                   f1_removal_request() const;
    const bap_map_cfg_s&                          bap_map_cfg() const;
    const gnb_du_res_cfg_s&                       gnb_du_res_cfg() const;
    const iab_tnl_address_request_s&              iab_tnl_address_request() const;
    const iab_up_cfg_upd_request_s&               iab_up_cfg_upd_request() const;
    const res_status_request_s&                   res_status_request() const;
    const positioning_meas_request_s&             positioning_meas_request() const;
    const trp_info_request_s&                     trp_info_request() const;
    const positioning_info_request_s&             positioning_info_request() const;
    const positioning_activation_request_s&       positioning_activation_request() const;
    const e_c_id_meas_initiation_request_s&       e_c_id_meas_initiation_request() const;
    const broadcast_context_setup_request_s&      broadcast_context_setup_request() const;
    const broadcast_context_release_cmd_s&        broadcast_context_release_cmd() const;
    const broadcast_context_mod_request_s&        broadcast_context_mod_request() const;
    const multicast_context_setup_request_s&      multicast_context_setup_request() const;
    const multicast_context_release_cmd_s&        multicast_context_release_cmd() const;
    const multicast_context_mod_request_s&        multicast_context_mod_request() const;
    const multicast_distribution_setup_request_s& multicast_distribution_setup_request() const;
    const multicast_distribution_release_cmd_s&   multicast_distribution_release_cmd() const;
    const pdc_meas_initiation_request_s&          pdc_meas_initiation_request() const;
    const prs_cfg_request_s&                      prs_cfg_request() const;
    const meas_precfg_required_s&                 meas_precfg_required() const;
    const error_ind_s&                            error_ind() const;
    const ue_context_release_request_s&           ue_context_release_request() const;
    const dl_rrc_msg_transfer_s&                  dl_rrc_msg_transfer() const;
    const ul_rrc_msg_transfer_s&                  ul_rrc_msg_transfer() const;
    const ue_inactivity_notif_s&                  ue_inactivity_notif() const;
    const private_msg_s&                          private_msg() const;
    const init_ul_rrc_msg_transfer_s&             init_ul_rrc_msg_transfer() const;
    const sys_info_delivery_cmd_s&                sys_info_delivery_cmd() const;
    const paging_s&                               paging() const;
    const notify_s&                               notify() const;
    const pws_restart_ind_s&                      pws_restart_ind() const;
    const pws_fail_ind_s&                         pws_fail_ind() const;
    const gnb_du_status_ind_s&                    gnb_du_status_ind() const;
    const rrc_delivery_report_s&                  rrc_delivery_report() const;
    const network_access_rate_reduction_s&        network_access_rate_reduction() const;
    const trace_start_s&                          trace_start() const;
    const deactiv_trace_s&                        deactiv_trace() const;
    const du_cu_radio_info_transfer_s&            du_cu_radio_info_transfer() const;
    const cu_du_radio_info_transfer_s&            cu_du_radio_info_transfer() const;
    const res_status_upd_s&                       res_status_upd() const;
    const access_and_mob_ind_s&                   access_and_mob_ind() const;
    const ref_time_info_report_ctrl_s&            ref_time_info_report_ctrl() const;
    const ref_time_info_report_s&                 ref_time_info_report() const;
    const access_success_s&                       access_success() const;
    const cell_traffic_trace_s&                   cell_traffic_trace() const;
    const positioning_assist_info_ctrl_s&         positioning_assist_info_ctrl() const;
    const positioning_assist_info_feedback_s&     positioning_assist_info_feedback() const;
    const positioning_meas_report_s&              positioning_meas_report() const;
    const positioning_meas_abort_s&               positioning_meas_abort() const;
    const positioning_meas_fail_ind_s&            positioning_meas_fail_ind() const;
    const positioning_meas_upd_s&                 positioning_meas_upd() const;
    const positioning_deactivation_s&             positioning_deactivation() const;
    const e_c_id_meas_fail_ind_s&                 e_c_id_meas_fail_ind() const;
    const e_c_id_meas_report_s&                   e_c_id_meas_report() const;
    const e_c_id_meas_termination_cmd_s&          e_c_id_meas_termination_cmd() const;
    const positioning_info_upd_s&                 positioning_info_upd() const;
    const multicast_group_paging_s&               multicast_group_paging() const;
    const broadcast_context_release_request_s&    broadcast_context_release_request() const;
    const multicast_context_release_request_s&    multicast_context_release_request() const;
    const pdc_meas_report_s&                      pdc_meas_report() const;
    const pdc_meas_termination_cmd_s&             pdc_meas_termination_cmd() const;
    const pdc_meas_fail_ind_s&                    pdc_meas_fail_ind() const;
    const meas_activation_s&                      meas_activation() const;
    const qo_e_info_transfer_s&                   qo_e_info_transfer() const;
    const pos_sys_info_delivery_cmd_s&            pos_sys_info_delivery_cmd() const;

  private:
    types type_;
    choice_buffer_t<access_and_mob_ind_s,
                    access_success_s,
                    bap_map_cfg_s,
                    broadcast_context_mod_request_s,
                    broadcast_context_release_cmd_s,
                    broadcast_context_release_request_s,
                    broadcast_context_setup_request_s,
                    cell_traffic_trace_s,
                    cu_du_radio_info_transfer_s,
                    deactiv_trace_s,
                    dl_rrc_msg_transfer_s,
                    du_cu_radio_info_transfer_s,
                    e_c_id_meas_fail_ind_s,
                    e_c_id_meas_initiation_request_s,
                    e_c_id_meas_report_s,
                    e_c_id_meas_termination_cmd_s,
                    error_ind_s,
                    f1_removal_request_s,
                    f1_setup_request_s,
                    gnb_cu_cfg_upd_s,
                    gnb_du_cfg_upd_s,
                    gnb_du_res_cfg_s,
                    gnb_du_res_coordination_request_s,
                    gnb_du_status_ind_s,
                    iab_tnl_address_request_s,
                    iab_up_cfg_upd_request_s,
                    init_ul_rrc_msg_transfer_s,
                    meas_activation_s,
                    meas_precfg_required_s,
                    multicast_context_mod_request_s,
                    multicast_context_release_cmd_s,
                    multicast_context_release_request_s,
                    multicast_context_setup_request_s,
                    multicast_distribution_release_cmd_s,
                    multicast_distribution_setup_request_s,
                    multicast_group_paging_s,
                    network_access_rate_reduction_s,
                    notify_s,
                    paging_s,
                    pdc_meas_fail_ind_s,
                    pdc_meas_initiation_request_s,
                    pdc_meas_report_s,
                    pdc_meas_termination_cmd_s,
                    pos_sys_info_delivery_cmd_s,
                    positioning_activation_request_s,
                    positioning_assist_info_ctrl_s,
                    positioning_assist_info_feedback_s,
                    positioning_deactivation_s,
                    positioning_info_request_s,
                    positioning_info_upd_s,
                    positioning_meas_abort_s,
                    positioning_meas_fail_ind_s,
                    positioning_meas_report_s,
                    positioning_meas_request_s,
                    positioning_meas_upd_s,
                    private_msg_s,
                    prs_cfg_request_s,
                    pws_cancel_request_s,
                    pws_fail_ind_s,
                    pws_restart_ind_s,
                    qo_e_info_transfer_s,
                    ref_time_info_report_ctrl_s,
                    ref_time_info_report_s,
                    res_status_request_s,
                    res_status_upd_s,
                    reset_s,
                    rrc_delivery_report_s,
                    sys_info_delivery_cmd_s,
                    trace_start_s,
                    trp_info_request_s,
                    ue_context_mod_request_s,
                    ue_context_mod_required_s,
                    ue_context_release_cmd_s,
                    ue_context_release_request_s,
                    ue_context_setup_request_s,
                    ue_inactivity_notif_s,
                    ul_rrc_msg_transfer_s,
                    write_replace_warning_request_s>
        c;

    void destroy_();
  };
  // SuccessfulOutcome ::= OPEN TYPE
  struct successful_outcome_c {
    struct types_opts {
      enum options {
        reset_ack,
        f1_setup_resp,
        gnb_du_cfg_upd_ack,
        gnb_cu_cfg_upd_ack,
        ue_context_setup_resp,
        ue_context_release_complete,
        ue_context_mod_resp,
        ue_context_mod_confirm,
        write_replace_warning_resp,
        pws_cancel_resp,
        gnb_du_res_coordination_resp,
        f1_removal_resp,
        bap_map_cfg_ack,
        gnb_du_res_cfg_ack,
        iab_tnl_address_resp,
        iab_up_cfg_upd_resp,
        res_status_resp,
        positioning_meas_resp,
        trp_info_resp,
        positioning_info_resp,
        positioning_activation_resp,
        e_c_id_meas_initiation_resp,
        broadcast_context_setup_resp,
        broadcast_context_release_complete,
        broadcast_context_mod_resp,
        multicast_context_setup_resp,
        multicast_context_release_complete,
        multicast_context_mod_resp,
        multicast_distribution_setup_resp,
        multicast_distribution_release_complete,
        pdc_meas_initiation_resp,
        prs_cfg_resp,
        meas_precfg_confirm,
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
    reset_ack_s&                                     reset_ack();
    f1_setup_resp_s&                                 f1_setup_resp();
    gnb_du_cfg_upd_ack_s&                            gnb_du_cfg_upd_ack();
    gnb_cu_cfg_upd_ack_s&                            gnb_cu_cfg_upd_ack();
    ue_context_setup_resp_s&                         ue_context_setup_resp();
    ue_context_release_complete_s&                   ue_context_release_complete();
    ue_context_mod_resp_s&                           ue_context_mod_resp();
    ue_context_mod_confirm_s&                        ue_context_mod_confirm();
    write_replace_warning_resp_s&                    write_replace_warning_resp();
    pws_cancel_resp_s&                               pws_cancel_resp();
    gnb_du_res_coordination_resp_s&                  gnb_du_res_coordination_resp();
    f1_removal_resp_s&                               f1_removal_resp();
    bap_map_cfg_ack_s&                               bap_map_cfg_ack();
    gnb_du_res_cfg_ack_s&                            gnb_du_res_cfg_ack();
    iab_tnl_address_resp_s&                          iab_tnl_address_resp();
    iab_up_cfg_upd_resp_s&                           iab_up_cfg_upd_resp();
    res_status_resp_s&                               res_status_resp();
    positioning_meas_resp_s&                         positioning_meas_resp();
    trp_info_resp_s&                                 trp_info_resp();
    positioning_info_resp_s&                         positioning_info_resp();
    positioning_activation_resp_s&                   positioning_activation_resp();
    e_c_id_meas_initiation_resp_s&                   e_c_id_meas_initiation_resp();
    broadcast_context_setup_resp_s&                  broadcast_context_setup_resp();
    broadcast_context_release_complete_s&            broadcast_context_release_complete();
    broadcast_context_mod_resp_s&                    broadcast_context_mod_resp();
    multicast_context_setup_resp_s&                  multicast_context_setup_resp();
    multicast_context_release_complete_s&            multicast_context_release_complete();
    multicast_context_mod_resp_s&                    multicast_context_mod_resp();
    multicast_distribution_setup_resp_s&             multicast_distribution_setup_resp();
    multicast_distribution_release_complete_s&       multicast_distribution_release_complete();
    pdc_meas_initiation_resp_s&                      pdc_meas_initiation_resp();
    prs_cfg_resp_s&                                  prs_cfg_resp();
    meas_precfg_confirm_s&                           meas_precfg_confirm();
    const reset_ack_s&                               reset_ack() const;
    const f1_setup_resp_s&                           f1_setup_resp() const;
    const gnb_du_cfg_upd_ack_s&                      gnb_du_cfg_upd_ack() const;
    const gnb_cu_cfg_upd_ack_s&                      gnb_cu_cfg_upd_ack() const;
    const ue_context_setup_resp_s&                   ue_context_setup_resp() const;
    const ue_context_release_complete_s&             ue_context_release_complete() const;
    const ue_context_mod_resp_s&                     ue_context_mod_resp() const;
    const ue_context_mod_confirm_s&                  ue_context_mod_confirm() const;
    const write_replace_warning_resp_s&              write_replace_warning_resp() const;
    const pws_cancel_resp_s&                         pws_cancel_resp() const;
    const gnb_du_res_coordination_resp_s&            gnb_du_res_coordination_resp() const;
    const f1_removal_resp_s&                         f1_removal_resp() const;
    const bap_map_cfg_ack_s&                         bap_map_cfg_ack() const;
    const gnb_du_res_cfg_ack_s&                      gnb_du_res_cfg_ack() const;
    const iab_tnl_address_resp_s&                    iab_tnl_address_resp() const;
    const iab_up_cfg_upd_resp_s&                     iab_up_cfg_upd_resp() const;
    const res_status_resp_s&                         res_status_resp() const;
    const positioning_meas_resp_s&                   positioning_meas_resp() const;
    const trp_info_resp_s&                           trp_info_resp() const;
    const positioning_info_resp_s&                   positioning_info_resp() const;
    const positioning_activation_resp_s&             positioning_activation_resp() const;
    const e_c_id_meas_initiation_resp_s&             e_c_id_meas_initiation_resp() const;
    const broadcast_context_setup_resp_s&            broadcast_context_setup_resp() const;
    const broadcast_context_release_complete_s&      broadcast_context_release_complete() const;
    const broadcast_context_mod_resp_s&              broadcast_context_mod_resp() const;
    const multicast_context_setup_resp_s&            multicast_context_setup_resp() const;
    const multicast_context_release_complete_s&      multicast_context_release_complete() const;
    const multicast_context_mod_resp_s&              multicast_context_mod_resp() const;
    const multicast_distribution_setup_resp_s&       multicast_distribution_setup_resp() const;
    const multicast_distribution_release_complete_s& multicast_distribution_release_complete() const;
    const pdc_meas_initiation_resp_s&                pdc_meas_initiation_resp() const;
    const prs_cfg_resp_s&                            prs_cfg_resp() const;
    const meas_precfg_confirm_s&                     meas_precfg_confirm() const;

  private:
    types type_;
    choice_buffer_t<bap_map_cfg_ack_s,
                    broadcast_context_mod_resp_s,
                    broadcast_context_release_complete_s,
                    broadcast_context_setup_resp_s,
                    e_c_id_meas_initiation_resp_s,
                    f1_removal_resp_s,
                    f1_setup_resp_s,
                    gnb_cu_cfg_upd_ack_s,
                    gnb_du_cfg_upd_ack_s,
                    gnb_du_res_cfg_ack_s,
                    gnb_du_res_coordination_resp_s,
                    iab_tnl_address_resp_s,
                    iab_up_cfg_upd_resp_s,
                    meas_precfg_confirm_s,
                    multicast_context_mod_resp_s,
                    multicast_context_release_complete_s,
                    multicast_context_setup_resp_s,
                    multicast_distribution_release_complete_s,
                    multicast_distribution_setup_resp_s,
                    pdc_meas_initiation_resp_s,
                    positioning_activation_resp_s,
                    positioning_info_resp_s,
                    positioning_meas_resp_s,
                    prs_cfg_resp_s,
                    pws_cancel_resp_s,
                    res_status_resp_s,
                    reset_ack_s,
                    trp_info_resp_s,
                    ue_context_mod_confirm_s,
                    ue_context_mod_resp_s,
                    ue_context_release_complete_s,
                    ue_context_setup_resp_s,
                    write_replace_warning_resp_s>
        c;

    void destroy_();
  };
  // UnsuccessfulOutcome ::= OPEN TYPE
  struct unsuccessful_outcome_c {
    struct types_opts {
      enum options {
        f1_setup_fail,
        gnb_du_cfg_upd_fail,
        gnb_cu_cfg_upd_fail,
        ue_context_setup_fail,
        ue_context_mod_fail,
        ue_context_mod_refuse,
        f1_removal_fail,
        bap_map_cfg_fail,
        gnb_du_res_cfg_fail,
        iab_tnl_address_fail,
        iab_up_cfg_upd_fail,
        res_status_fail,
        positioning_meas_fail,
        trp_info_fail,
        positioning_info_fail,
        positioning_activation_fail,
        e_c_id_meas_initiation_fail,
        broadcast_context_setup_fail,
        broadcast_context_mod_fail,
        multicast_context_setup_fail,
        multicast_context_mod_fail,
        multicast_distribution_setup_fail,
        pdc_meas_initiation_fail,
        prs_cfg_fail,
        meas_precfg_refuse,
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
    f1_setup_fail_s&                           f1_setup_fail();
    gnb_du_cfg_upd_fail_s&                     gnb_du_cfg_upd_fail();
    gnb_cu_cfg_upd_fail_s&                     gnb_cu_cfg_upd_fail();
    ue_context_setup_fail_s&                   ue_context_setup_fail();
    ue_context_mod_fail_s&                     ue_context_mod_fail();
    ue_context_mod_refuse_s&                   ue_context_mod_refuse();
    f1_removal_fail_s&                         f1_removal_fail();
    bap_map_cfg_fail_s&                        bap_map_cfg_fail();
    gnb_du_res_cfg_fail_s&                     gnb_du_res_cfg_fail();
    iab_tnl_address_fail_s&                    iab_tnl_address_fail();
    iab_up_cfg_upd_fail_s&                     iab_up_cfg_upd_fail();
    res_status_fail_s&                         res_status_fail();
    positioning_meas_fail_s&                   positioning_meas_fail();
    trp_info_fail_s&                           trp_info_fail();
    positioning_info_fail_s&                   positioning_info_fail();
    positioning_activation_fail_s&             positioning_activation_fail();
    e_c_id_meas_initiation_fail_s&             e_c_id_meas_initiation_fail();
    broadcast_context_setup_fail_s&            broadcast_context_setup_fail();
    broadcast_context_mod_fail_s&              broadcast_context_mod_fail();
    multicast_context_setup_fail_s&            multicast_context_setup_fail();
    multicast_context_mod_fail_s&              multicast_context_mod_fail();
    multicast_distribution_setup_fail_s&       multicast_distribution_setup_fail();
    pdc_meas_initiation_fail_s&                pdc_meas_initiation_fail();
    prs_cfg_fail_s&                            prs_cfg_fail();
    meas_precfg_refuse_s&                      meas_precfg_refuse();
    const f1_setup_fail_s&                     f1_setup_fail() const;
    const gnb_du_cfg_upd_fail_s&               gnb_du_cfg_upd_fail() const;
    const gnb_cu_cfg_upd_fail_s&               gnb_cu_cfg_upd_fail() const;
    const ue_context_setup_fail_s&             ue_context_setup_fail() const;
    const ue_context_mod_fail_s&               ue_context_mod_fail() const;
    const ue_context_mod_refuse_s&             ue_context_mod_refuse() const;
    const f1_removal_fail_s&                   f1_removal_fail() const;
    const bap_map_cfg_fail_s&                  bap_map_cfg_fail() const;
    const gnb_du_res_cfg_fail_s&               gnb_du_res_cfg_fail() const;
    const iab_tnl_address_fail_s&              iab_tnl_address_fail() const;
    const iab_up_cfg_upd_fail_s&               iab_up_cfg_upd_fail() const;
    const res_status_fail_s&                   res_status_fail() const;
    const positioning_meas_fail_s&             positioning_meas_fail() const;
    const trp_info_fail_s&                     trp_info_fail() const;
    const positioning_info_fail_s&             positioning_info_fail() const;
    const positioning_activation_fail_s&       positioning_activation_fail() const;
    const e_c_id_meas_initiation_fail_s&       e_c_id_meas_initiation_fail() const;
    const broadcast_context_setup_fail_s&      broadcast_context_setup_fail() const;
    const broadcast_context_mod_fail_s&        broadcast_context_mod_fail() const;
    const multicast_context_setup_fail_s&      multicast_context_setup_fail() const;
    const multicast_context_mod_fail_s&        multicast_context_mod_fail() const;
    const multicast_distribution_setup_fail_s& multicast_distribution_setup_fail() const;
    const pdc_meas_initiation_fail_s&          pdc_meas_initiation_fail() const;
    const prs_cfg_fail_s&                      prs_cfg_fail() const;
    const meas_precfg_refuse_s&                meas_precfg_refuse() const;

  private:
    types type_;
    choice_buffer_t<bap_map_cfg_fail_s,
                    broadcast_context_mod_fail_s,
                    broadcast_context_setup_fail_s,
                    e_c_id_meas_initiation_fail_s,
                    f1_removal_fail_s,
                    f1_setup_fail_s,
                    gnb_cu_cfg_upd_fail_s,
                    gnb_du_cfg_upd_fail_s,
                    gnb_du_res_cfg_fail_s,
                    iab_tnl_address_fail_s,
                    iab_up_cfg_upd_fail_s,
                    meas_precfg_refuse_s,
                    multicast_context_mod_fail_s,
                    multicast_context_setup_fail_s,
                    multicast_distribution_setup_fail_s,
                    pdc_meas_initiation_fail_s,
                    positioning_activation_fail_s,
                    positioning_info_fail_s,
                    positioning_meas_fail_s,
                    prs_cfg_fail_s,
                    res_status_fail_s,
                    trp_info_fail_s,
                    ue_context_mod_fail_s,
                    ue_context_mod_refuse_s,
                    ue_context_setup_fail_s>
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

// F1AP-PDU-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
using f1ap_pdu_ext_ies_o = protocol_ies_empty_o;

// InitiatingMessage ::= SEQUENCE{{F1AP-ELEMENTARY-PROCEDURE}}
struct init_msg_s {
  uint16_t                      proc_code = 0;
  crit_e                        crit;
  f1ap_elem_procs_o::init_msg_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// SuccessfulOutcome ::= SEQUENCE{{F1AP-ELEMENTARY-PROCEDURE}}
struct successful_outcome_s {
  uint16_t                                proc_code = 0;
  crit_e                                  crit;
  f1ap_elem_procs_o::successful_outcome_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// UnsuccessfulOutcome ::= SEQUENCE{{F1AP-ELEMENTARY-PROCEDURE}}
struct unsuccessful_outcome_s {
  uint16_t                                  proc_code = 0;
  crit_e                                    crit;
  f1ap_elem_procs_o::unsuccessful_outcome_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// F1AP-PDU ::= CHOICE
struct f1ap_pdu_c {
  struct types_opts {
    enum options { init_msg, successful_outcome, unsuccessful_outcome, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  f1ap_pdu_c() = default;
  f1ap_pdu_c(const f1ap_pdu_c& other);
  f1ap_pdu_c& operator=(const f1ap_pdu_c& other);
  ~f1ap_pdu_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  init_msg_s& init_msg()
  {
    assert_choice_type(types::init_msg, type_, "F1AP-PDU");
    return c.get<init_msg_s>();
  }
  successful_outcome_s& successful_outcome()
  {
    assert_choice_type(types::successful_outcome, type_, "F1AP-PDU");
    return c.get<successful_outcome_s>();
  }
  unsuccessful_outcome_s& unsuccessful_outcome()
  {
    assert_choice_type(types::unsuccessful_outcome, type_, "F1AP-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "F1AP-PDU");
    return c.get<protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>>();
  }
  const init_msg_s& init_msg() const
  {
    assert_choice_type(types::init_msg, type_, "F1AP-PDU");
    return c.get<init_msg_s>();
  }
  const successful_outcome_s& successful_outcome() const
  {
    assert_choice_type(types::successful_outcome, type_, "F1AP-PDU");
    return c.get<successful_outcome_s>();
  }
  const unsuccessful_outcome_s& unsuccessful_outcome() const
  {
    assert_choice_type(types::unsuccessful_outcome, type_, "F1AP-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  const protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "F1AP-PDU");
    return c.get<protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>>();
  }
  init_msg_s&                                         set_init_msg();
  successful_outcome_s&                               set_successful_outcome();
  unsuccessful_outcome_s&                             set_unsuccessful_outcome();
  protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<init_msg_s,
                  protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>,
                  successful_outcome_s,
                  unsuccessful_outcome_s>
      c;

  void destroy_();
};

// ProtocolIE-FieldPair{F1AP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE{{F1AP-PROTOCOL-IES-PAIR}}
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

// ProtocolIE-ContainerPair{F1AP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE (SIZE (0..65535)) OF ProtocolIE-FieldPair
template <class ies_set_paramT_>
using protocol_ie_container_pair_l = dyn_seq_of<protocol_ie_field_pair_s<ies_set_paramT_>, 0, 65535, true>;

} // namespace f1ap
} // namespace asn1

extern template struct asn1::protocol_ie_field_s<asn1::f1ap::access_and_mob_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::access_success_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::sul_info_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::gnb_du_cell_res_cfg_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::iab_du_cell_res_cfg_fdd_info_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::iab_du_cell_res_cfg_tdd_info_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::add_path_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::ba_player_bh_rlc_ch_map_info_item_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::bap_map_cfg_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::bap_map_cfg_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::bap_map_cfg_fail_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::dyn_5qi_descriptor_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::non_dyn_5qi_descriptor_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::qos_flow_level_qos_params_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::bplmn_id_info_item_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::broadcast_context_mod_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::broadcast_context_mod_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::broadcast_context_mod_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::broadcast_context_release_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::broadcast_context_release_complete_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::broadcast_context_release_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::broadcast_context_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::broadcast_context_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::broadcast_context_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::cu_du_radio_info_transfer_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::cu_to_du_rrc_info_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::rr_status_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::cell_traffic_trace_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::cells_to_be_activ_list_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::iab_mt_cell_list_item_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::dl_rrc_msg_transfer_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::flows_mapped_to_drb_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::drbs_modified_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::ul_up_tnl_info_to_be_setup_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::drbs_required_to_be_modified_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::drbs_setup_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::drbs_setup_mod_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::drbs_to_be_modified_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::drbs_to_be_setup_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::drbs_to_be_setup_mod_item_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::du_cu_radio_info_transfer_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::du_to_cu_rrc_info_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::deactiv_trace_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::e_c_id_meas_fail_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::e_c_id_meas_initiation_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::e_c_id_meas_initiation_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::e_c_id_meas_initiation_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::e_c_id_meas_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::e_c_id_meas_termination_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::error_ind_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::extended_served_plmns_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::fdd_info_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::gnb_rx_tx_time_diff_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::served_plmns_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::tdd_info_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::ul_rtoa_meas_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::gnb_du_sys_info_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::pos_meas_result_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::served_cell_info_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::paging_cell_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::srbs_to_be_setup_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::srbs_to_be_setup_mod_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::trp_meas_request_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::trp_meas_upd_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::pws_sys_info_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::trace_activation_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::f1_removal_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::f1_removal_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::f1_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::f1_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::f1_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_cu_cfg_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_cu_cfg_upd_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_cu_cfg_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_cfg_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_cfg_upd_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_cfg_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_res_cfg_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_res_cfg_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_res_cfg_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_res_coordination_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_res_coordination_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_status_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::iab_tnl_address_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::iab_tnl_address_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::iab_tnl_address_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::iab_up_cfg_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::iab_up_cfg_upd_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::iab_up_cfg_upd_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::init_ul_rrc_msg_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::meas_activation_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::meas_precfg_confirm_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::meas_precfg_refuse_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::meas_precfg_required_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_context_mod_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_context_mod_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_context_mod_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_context_release_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_context_release_complete_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_context_release_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_context_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_context_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_context_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_distribution_release_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_distribution_release_complete_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_distribution_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_distribution_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_distribution_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_group_paging_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::network_access_rate_reduction_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::notify_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pdc_meas_fail_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pdc_meas_initiation_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pdc_meas_initiation_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pdc_meas_initiation_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pdc_meas_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pdc_meas_termination_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::prs_cfg_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::prs_cfg_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::prs_cfg_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pws_cancel_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pws_cancel_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pws_fail_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pws_restart_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::paging_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pos_sys_info_delivery_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_activation_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_activation_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_activation_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_assist_info_ctrl_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_assist_info_feedback_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_deactivation_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_info_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_info_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_info_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_info_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_meas_abort_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_meas_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_meas_fail_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_meas_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_meas_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_meas_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_meas_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::qo_e_info_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::rrc_delivery_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ref_time_info_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ref_time_info_report_ctrl_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::reset_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::reset_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::res_status_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::res_status_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::res_status_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::res_status_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::sys_info_delivery_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::trp_info_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::trp_info_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::trp_info_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::trace_start_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_mod_confirm_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_mod_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_mod_refuse_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_mod_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_mod_required_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_mod_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_release_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_release_complete_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_release_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_inactivity_notif_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ul_rrc_msg_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::write_replace_warning_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::write_replace_warning_resp_ies_o>;
