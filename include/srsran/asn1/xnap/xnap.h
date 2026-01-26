/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
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
 *                     3GPP TS ASN1 XNAP v17.4.0 (2023-03)
 *
 ******************************************************************************/

#pragma once

#include "srsran/asn1/asn1_ap_utils.h"

namespace asn1 {
namespace xnap {

struct ho_request_ies_container;
using ho_request_s = elementary_procedure_option<ho_request_ies_container>;
struct retrieve_ue_context_request_ies_container;
using retrieve_ue_context_request_s = elementary_procedure_option<retrieve_ue_context_request_ies_container>;
struct sn_ode_addition_request_ies_container;
using sn_ode_addition_request_s = elementary_procedure_option<sn_ode_addition_request_ies_container>;
struct sn_ode_mod_request_ies_container;
using sn_ode_mod_request_s = elementary_procedure_option<sn_ode_mod_request_ies_container>;
struct sn_ode_mod_required_ies_container;
using sn_ode_mod_required_s = elementary_procedure_option<sn_ode_mod_required_ies_container>;
struct sn_ode_release_request_ies_container;
using sn_ode_release_request_s = elementary_procedure_option<sn_ode_release_request_ies_container>;
struct sn_ode_release_required_ies_container;
using sn_ode_release_required_s = elementary_procedure_option<sn_ode_release_required_ies_container>;
struct sn_ode_change_required_ies_container;
using sn_ode_change_required_s = elementary_procedure_option<sn_ode_change_required_ies_container>;
struct xn_removal_request_ies_container;
using xn_removal_request_s = elementary_procedure_option<xn_removal_request_ies_container>;
struct xn_setup_request_ies_container;
using xn_setup_request_s = elementary_procedure_option<xn_setup_request_ies_container>;
struct ngran_node_cfg_upd_ies_container;
using ngran_node_cfg_upd_s = elementary_procedure_option<ngran_node_cfg_upd_ies_container>;
struct e_utra_nr_cell_res_coordination_request_ies_container;
using e_utra_nr_cell_res_coordination_request_s =
    elementary_procedure_option<e_utra_nr_cell_res_coordination_request_ies_container>;
struct cell_activation_request_ies_container;
using cell_activation_request_s = elementary_procedure_option<cell_activation_request_ies_container>;
struct reset_request_ies_container;
using reset_request_s = elementary_procedure_option<reset_request_ies_container>;
struct res_status_request_ies_container;
using res_status_request_s = elementary_procedure_option<res_status_request_ies_container>;
struct mob_change_request_ies_container;
using mob_change_request_s = elementary_procedure_option<mob_change_request_ies_container>;
struct iab_transport_migration_management_request_ies_container;
using iab_transport_migration_management_request_s =
    elementary_procedure_option<iab_transport_migration_management_request_ies_container>;
struct iab_transport_migration_mod_request_ies_container;
using iab_transport_migration_mod_request_s =
    elementary_procedure_option<iab_transport_migration_mod_request_ies_container>;
struct iab_res_coordination_request_ies_container;
using iab_res_coordination_request_s = elementary_procedure_option<iab_res_coordination_request_ies_container>;
struct partial_ue_context_transfer_ies_container;
using partial_ue_context_transfer_s = elementary_procedure_option<partial_ue_context_transfer_ies_container>;
struct ho_request_ack_ies_container;
using ho_request_ack_s = elementary_procedure_option<ho_request_ack_ies_container>;
struct retrieve_ue_context_resp_ies_container;
using retrieve_ue_context_resp_s = elementary_procedure_option<retrieve_ue_context_resp_ies_container>;
struct sn_ode_addition_request_ack_ies_container;
using sn_ode_addition_request_ack_s = elementary_procedure_option<sn_ode_addition_request_ack_ies_container>;
struct sn_ode_mod_request_ack_ies_container;
using sn_ode_mod_request_ack_s = elementary_procedure_option<sn_ode_mod_request_ack_ies_container>;
struct sn_ode_mod_confirm_ies_container;
using sn_ode_mod_confirm_s = elementary_procedure_option<sn_ode_mod_confirm_ies_container>;
struct sn_ode_release_request_ack_ies_container;
using sn_ode_release_request_ack_s = elementary_procedure_option<sn_ode_release_request_ack_ies_container>;
struct sn_ode_release_confirm_ies_container;
using sn_ode_release_confirm_s = elementary_procedure_option<sn_ode_release_confirm_ies_container>;
struct sn_ode_change_confirm_ies_container;
using sn_ode_change_confirm_s = elementary_procedure_option<sn_ode_change_confirm_ies_container>;
struct xn_removal_resp_ies_container;
using xn_removal_resp_s = elementary_procedure_option<xn_removal_resp_ies_container>;
struct xn_setup_resp_ies_container;
using xn_setup_resp_s = elementary_procedure_option<xn_setup_resp_ies_container>;
struct ngran_node_cfg_upd_ack_ies_container;
using ngran_node_cfg_upd_ack_s = elementary_procedure_option<ngran_node_cfg_upd_ack_ies_container>;
struct e_utra_nr_cell_res_coordination_resp_ies_container;
using e_utra_nr_cell_res_coordination_resp_s =
    elementary_procedure_option<e_utra_nr_cell_res_coordination_resp_ies_container>;
struct cell_activation_resp_ies_container;
using cell_activation_resp_s = elementary_procedure_option<cell_activation_resp_ies_container>;
struct reset_resp_ies_container;
using reset_resp_s = elementary_procedure_option<reset_resp_ies_container>;
struct res_status_resp_ies_container;
using res_status_resp_s = elementary_procedure_option<res_status_resp_ies_container>;
struct mob_change_ack_ies_container;
using mob_change_ack_s = elementary_procedure_option<mob_change_ack_ies_container>;
struct iab_transport_migration_management_resp_ies_container;
using iab_transport_migration_management_resp_s =
    elementary_procedure_option<iab_transport_migration_management_resp_ies_container>;
struct iab_transport_migration_mod_resp_ies_container;
using iab_transport_migration_mod_resp_s = elementary_procedure_option<iab_transport_migration_mod_resp_ies_container>;
struct iab_res_coordination_resp_ies_container;
using iab_res_coordination_resp_s = elementary_procedure_option<iab_res_coordination_resp_ies_container>;
struct partial_ue_context_transfer_ack_ies_container;
using partial_ue_context_transfer_ack_s = elementary_procedure_option<partial_ue_context_transfer_ack_ies_container>;
struct ho_prep_fail_ies_container;
using ho_prep_fail_s = elementary_procedure_option<ho_prep_fail_ies_container>;
struct retrieve_ue_context_fail_ies_container;
using retrieve_ue_context_fail_s = elementary_procedure_option<retrieve_ue_context_fail_ies_container>;
struct sn_ode_addition_request_reject_ies_container;
using sn_ode_addition_request_reject_s = elementary_procedure_option<sn_ode_addition_request_reject_ies_container>;
struct sn_ode_mod_request_reject_ies_container;
using sn_ode_mod_request_reject_s = elementary_procedure_option<sn_ode_mod_request_reject_ies_container>;
struct sn_ode_mod_refuse_ies_container;
using sn_ode_mod_refuse_s = elementary_procedure_option<sn_ode_mod_refuse_ies_container>;
struct sn_ode_release_reject_ies_container;
using sn_ode_release_reject_s = elementary_procedure_option<sn_ode_release_reject_ies_container>;
struct sn_ode_change_refuse_ies_container;
using sn_ode_change_refuse_s = elementary_procedure_option<sn_ode_change_refuse_ies_container>;
struct xn_removal_fail_ies_container;
using xn_removal_fail_s = elementary_procedure_option<xn_removal_fail_ies_container>;
struct xn_setup_fail_ies_container;
using xn_setup_fail_s = elementary_procedure_option<xn_setup_fail_ies_container>;
struct ngran_node_cfg_upd_fail_ies_container;
using ngran_node_cfg_upd_fail_s = elementary_procedure_option<ngran_node_cfg_upd_fail_ies_container>;
struct cell_activation_fail_ies_container;
using cell_activation_fail_s = elementary_procedure_option<cell_activation_fail_ies_container>;
struct res_status_fail_ies_container;
using res_status_fail_s = elementary_procedure_option<res_status_fail_ies_container>;
struct mob_change_fail_ies_container;
using mob_change_fail_s = elementary_procedure_option<mob_change_fail_ies_container>;
struct iab_transport_migration_management_reject_ies_container;
using iab_transport_migration_management_reject_s =
    elementary_procedure_option<iab_transport_migration_management_reject_ies_container>;
struct partial_ue_context_transfer_fail_ies_container;
using partial_ue_context_transfer_fail_s = elementary_procedure_option<partial_ue_context_transfer_fail_ies_container>;
struct sn_status_transfer_ies_container;
using sn_status_transfer_s = elementary_procedure_option<sn_status_transfer_ies_container>;
struct ho_cancel_ies_container;
using ho_cancel_s = elementary_procedure_option<ho_cancel_ies_container>;
struct ran_paging_ies_container;
using ran_paging_s = elementary_procedure_option<ran_paging_ies_container>;
struct xn_u_address_ind_ies_container;
using xn_u_address_ind_s = elementary_procedure_option<xn_u_address_ind_ies_container>;
struct ue_context_release_ies_container;
using ue_context_release_s = elementary_procedure_option<ue_context_release_ies_container>;
struct sn_ode_recfg_complete_ies_container;
using sn_ode_recfg_complete_s = elementary_procedure_option<sn_ode_recfg_complete_ies_container>;
struct sn_ode_counter_check_request_ies_container;
using sn_ode_counter_check_request_s = elementary_procedure_option<sn_ode_counter_check_request_ies_container>;
struct rrc_transfer_ies_container;
using rrc_transfer_s = elementary_procedure_option<rrc_transfer_ies_container>;
struct error_ind_ies_container;
using error_ind_s = elementary_procedure_option<error_ind_ies_container>;
struct private_msg_s;
struct notif_ctrl_ind_ies_container;
using notif_ctrl_ind_s = elementary_procedure_option<notif_ctrl_ind_ies_container>;
struct activity_notif_ies_container;
using activity_notif_s = elementary_procedure_option<activity_notif_ies_container>;
struct secondary_rat_data_usage_report_ies_container;
using secondary_rat_data_usage_report_s = elementary_procedure_option<secondary_rat_data_usage_report_ies_container>;
struct deactiv_trace_ies_container;
using deactiv_trace_s = elementary_procedure_option<deactiv_trace_ies_container>;
struct trace_start_ies_container;
using trace_start_s = elementary_procedure_option<trace_start_ies_container>;
struct ho_success_ies_container;
using ho_success_s = elementary_procedure_option<ho_success_ies_container>;
struct conditional_ho_cancel_ies_container;
using conditional_ho_cancel_s = elementary_procedure_option<conditional_ho_cancel_ies_container>;
struct early_status_transfer_ies_container;
using early_status_transfer_s = elementary_procedure_option<early_status_transfer_ies_container>;
struct fail_ind_ies_o;
using fail_ind_s = elementary_procedure_option<protocol_ie_container_l<fail_ind_ies_o>>;
struct ho_report_ies_container;
using ho_report_s = elementary_procedure_option<ho_report_ies_container>;
struct res_status_upd_ies_container;
using res_status_upd_s = elementary_procedure_option<res_status_upd_ies_container>;
struct access_and_mob_ind_ies_container;
using access_and_mob_ind_s = elementary_procedure_option<access_and_mob_ind_ies_container>;
struct cell_traffic_trace_ies_container;
using cell_traffic_trace_s = elementary_procedure_option<cell_traffic_trace_ies_container>;
struct ran_multicast_group_paging_ies_container;
using ran_multicast_group_paging_s = elementary_procedure_option<ran_multicast_group_paging_ies_container>;
struct scg_fail_info_report_ies_container;
using scg_fail_info_report_s = elementary_procedure_option<scg_fail_info_report_ies_container>;
struct scg_fail_transfer_ies_container;
using scg_fail_transfer_s = elementary_procedure_option<scg_fail_transfer_ies_container>;
struct f1_c_traffic_transfer_ies_container;
using f1_c_traffic_transfer_s = elementary_procedure_option<f1_c_traffic_transfer_ies_container>;
struct retrieve_ue_context_confirm_ies_container;
using retrieve_ue_context_confirm_s = elementary_procedure_option<retrieve_ue_context_confirm_ies_container>;
struct cp_c_cancel_ies_container;
using cp_c_cancel_s = elementary_procedure_option<cp_c_cancel_ies_container>;

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// ProtocolIE-FieldPair{XNAP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE{{XNAP-PROTOCOL-IES-PAIR}}
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

// ProtocolIE-ContainerPair{XNAP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE (SIZE (0..65535)) OF ProtocolIE-FieldPair
template <class ies_set_paramT_>
using protocol_ie_container_pair_l = dyn_seq_of<protocol_ie_field_pair_s<ies_set_paramT_>, 0, 65535, true>;

// XNAP-ELEMENTARY-PROCEDURES-CLASS-1 ::= OBJECT SET OF XNAP-ELEMENTARY-PROCEDURE
struct xnap_elem_procs_class_1_o {
  // InitiatingMessage ::= OPEN TYPE
  struct init_msg_c {
    struct types_opts {
      enum options {
        ho_request,
        retrieve_ue_context_request,
        sn_ode_addition_request,
        sn_ode_mod_request,
        sn_ode_mod_required,
        sn_ode_release_request,
        sn_ode_release_required,
        sn_ode_change_required,
        xn_removal_request,
        xn_setup_request,
        ngran_node_cfg_upd,
        e_utra_nr_cell_res_coordination_request,
        cell_activation_request,
        reset_request,
        res_status_request,
        mob_change_request,
        iab_transport_migration_management_request,
        iab_transport_migration_mod_request,
        iab_res_coordination_request,
        partial_ue_context_transfer,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    init_msg_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ho_request_s&                                       ho_request();
    retrieve_ue_context_request_s&                      retrieve_ue_context_request();
    sn_ode_addition_request_s&                          sn_ode_addition_request();
    sn_ode_mod_request_s&                               sn_ode_mod_request();
    sn_ode_mod_required_s&                              sn_ode_mod_required();
    sn_ode_release_request_s&                           sn_ode_release_request();
    sn_ode_release_required_s&                          sn_ode_release_required();
    sn_ode_change_required_s&                           sn_ode_change_required();
    xn_removal_request_s&                               xn_removal_request();
    xn_setup_request_s&                                 xn_setup_request();
    ngran_node_cfg_upd_s&                               ngran_node_cfg_upd();
    e_utra_nr_cell_res_coordination_request_s&          e_utra_nr_cell_res_coordination_request();
    cell_activation_request_s&                          cell_activation_request();
    reset_request_s&                                    reset_request();
    res_status_request_s&                               res_status_request();
    mob_change_request_s&                               mob_change_request();
    iab_transport_migration_management_request_s&       iab_transport_migration_management_request();
    iab_transport_migration_mod_request_s&              iab_transport_migration_mod_request();
    iab_res_coordination_request_s&                     iab_res_coordination_request();
    partial_ue_context_transfer_s&                      partial_ue_context_transfer();
    const ho_request_s&                                 ho_request() const;
    const retrieve_ue_context_request_s&                retrieve_ue_context_request() const;
    const sn_ode_addition_request_s&                    sn_ode_addition_request() const;
    const sn_ode_mod_request_s&                         sn_ode_mod_request() const;
    const sn_ode_mod_required_s&                        sn_ode_mod_required() const;
    const sn_ode_release_request_s&                     sn_ode_release_request() const;
    const sn_ode_release_required_s&                    sn_ode_release_required() const;
    const sn_ode_change_required_s&                     sn_ode_change_required() const;
    const xn_removal_request_s&                         xn_removal_request() const;
    const xn_setup_request_s&                           xn_setup_request() const;
    const ngran_node_cfg_upd_s&                         ngran_node_cfg_upd() const;
    const e_utra_nr_cell_res_coordination_request_s&    e_utra_nr_cell_res_coordination_request() const;
    const cell_activation_request_s&                    cell_activation_request() const;
    const reset_request_s&                              reset_request() const;
    const res_status_request_s&                         res_status_request() const;
    const mob_change_request_s&                         mob_change_request() const;
    const iab_transport_migration_management_request_s& iab_transport_migration_management_request() const;
    const iab_transport_migration_mod_request_s&        iab_transport_migration_mod_request() const;
    const iab_res_coordination_request_s&               iab_res_coordination_request() const;
    const partial_ue_context_transfer_s&                partial_ue_context_transfer() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  // SuccessfulOutcome ::= OPEN TYPE
  struct successful_outcome_c {
    struct types_opts {
      enum options {
        ho_request_ack,
        retrieve_ue_context_resp,
        sn_ode_addition_request_ack,
        sn_ode_mod_request_ack,
        sn_ode_mod_confirm,
        sn_ode_release_request_ack,
        sn_ode_release_confirm,
        sn_ode_change_confirm,
        xn_removal_resp,
        xn_setup_resp,
        ngran_node_cfg_upd_ack,
        e_utra_nr_cell_res_coordination_resp,
        cell_activation_resp,
        reset_resp,
        res_status_resp,
        mob_change_ack,
        iab_transport_migration_management_resp,
        iab_transport_migration_mod_resp,
        iab_res_coordination_resp,
        partial_ue_context_transfer_ack,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    successful_outcome_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ho_request_ack_s&                                ho_request_ack();
    retrieve_ue_context_resp_s&                      retrieve_ue_context_resp();
    sn_ode_addition_request_ack_s&                   sn_ode_addition_request_ack();
    sn_ode_mod_request_ack_s&                        sn_ode_mod_request_ack();
    sn_ode_mod_confirm_s&                            sn_ode_mod_confirm();
    sn_ode_release_request_ack_s&                    sn_ode_release_request_ack();
    sn_ode_release_confirm_s&                        sn_ode_release_confirm();
    sn_ode_change_confirm_s&                         sn_ode_change_confirm();
    xn_removal_resp_s&                               xn_removal_resp();
    xn_setup_resp_s&                                 xn_setup_resp();
    ngran_node_cfg_upd_ack_s&                        ngran_node_cfg_upd_ack();
    e_utra_nr_cell_res_coordination_resp_s&          e_utra_nr_cell_res_coordination_resp();
    cell_activation_resp_s&                          cell_activation_resp();
    reset_resp_s&                                    reset_resp();
    res_status_resp_s&                               res_status_resp();
    mob_change_ack_s&                                mob_change_ack();
    iab_transport_migration_management_resp_s&       iab_transport_migration_management_resp();
    iab_transport_migration_mod_resp_s&              iab_transport_migration_mod_resp();
    iab_res_coordination_resp_s&                     iab_res_coordination_resp();
    partial_ue_context_transfer_ack_s&               partial_ue_context_transfer_ack();
    const ho_request_ack_s&                          ho_request_ack() const;
    const retrieve_ue_context_resp_s&                retrieve_ue_context_resp() const;
    const sn_ode_addition_request_ack_s&             sn_ode_addition_request_ack() const;
    const sn_ode_mod_request_ack_s&                  sn_ode_mod_request_ack() const;
    const sn_ode_mod_confirm_s&                      sn_ode_mod_confirm() const;
    const sn_ode_release_request_ack_s&              sn_ode_release_request_ack() const;
    const sn_ode_release_confirm_s&                  sn_ode_release_confirm() const;
    const sn_ode_change_confirm_s&                   sn_ode_change_confirm() const;
    const xn_removal_resp_s&                         xn_removal_resp() const;
    const xn_setup_resp_s&                           xn_setup_resp() const;
    const ngran_node_cfg_upd_ack_s&                  ngran_node_cfg_upd_ack() const;
    const e_utra_nr_cell_res_coordination_resp_s&    e_utra_nr_cell_res_coordination_resp() const;
    const cell_activation_resp_s&                    cell_activation_resp() const;
    const reset_resp_s&                              reset_resp() const;
    const res_status_resp_s&                         res_status_resp() const;
    const mob_change_ack_s&                          mob_change_ack() const;
    const iab_transport_migration_management_resp_s& iab_transport_migration_management_resp() const;
    const iab_transport_migration_mod_resp_s&        iab_transport_migration_mod_resp() const;
    const iab_res_coordination_resp_s&               iab_res_coordination_resp() const;
    const partial_ue_context_transfer_ack_s&         partial_ue_context_transfer_ack() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  // UnsuccessfulOutcome ::= OPEN TYPE
  struct unsuccessful_outcome_c {
    struct types_opts {
      enum options {
        ho_prep_fail,
        retrieve_ue_context_fail,
        sn_ode_addition_request_reject,
        sn_ode_mod_request_reject,
        sn_ode_mod_refuse,
        sn_ode_release_reject,
        sn_ode_change_refuse,
        xn_removal_fail,
        xn_setup_fail,
        ngran_node_cfg_upd_fail,
        cell_activation_fail,
        res_status_fail,
        mob_change_fail,
        iab_transport_migration_management_reject,
        partial_ue_context_transfer_fail,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    unsuccessful_outcome_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ho_prep_fail_s&                                    ho_prep_fail();
    retrieve_ue_context_fail_s&                        retrieve_ue_context_fail();
    sn_ode_addition_request_reject_s&                  sn_ode_addition_request_reject();
    sn_ode_mod_request_reject_s&                       sn_ode_mod_request_reject();
    sn_ode_mod_refuse_s&                               sn_ode_mod_refuse();
    sn_ode_release_reject_s&                           sn_ode_release_reject();
    sn_ode_change_refuse_s&                            sn_ode_change_refuse();
    xn_removal_fail_s&                                 xn_removal_fail();
    xn_setup_fail_s&                                   xn_setup_fail();
    ngran_node_cfg_upd_fail_s&                         ngran_node_cfg_upd_fail();
    cell_activation_fail_s&                            cell_activation_fail();
    res_status_fail_s&                                 res_status_fail();
    mob_change_fail_s&                                 mob_change_fail();
    iab_transport_migration_management_reject_s&       iab_transport_migration_management_reject();
    partial_ue_context_transfer_fail_s&                partial_ue_context_transfer_fail();
    const ho_prep_fail_s&                              ho_prep_fail() const;
    const retrieve_ue_context_fail_s&                  retrieve_ue_context_fail() const;
    const sn_ode_addition_request_reject_s&            sn_ode_addition_request_reject() const;
    const sn_ode_mod_request_reject_s&                 sn_ode_mod_request_reject() const;
    const sn_ode_mod_refuse_s&                         sn_ode_mod_refuse() const;
    const sn_ode_release_reject_s&                     sn_ode_release_reject() const;
    const sn_ode_change_refuse_s&                      sn_ode_change_refuse() const;
    const xn_removal_fail_s&                           xn_removal_fail() const;
    const xn_setup_fail_s&                             xn_setup_fail() const;
    const ngran_node_cfg_upd_fail_s&                   ngran_node_cfg_upd_fail() const;
    const cell_activation_fail_s&                      cell_activation_fail() const;
    const res_status_fail_s&                           res_status_fail() const;
    const mob_change_fail_s&                           mob_change_fail() const;
    const iab_transport_migration_management_reject_s& iab_transport_migration_management_reject() const;
    const partial_ue_context_transfer_fail_s&          partial_ue_context_transfer_fail() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint16_t               idx_to_proc_code(uint32_t idx);
  static bool                   is_proc_code_valid(const uint16_t& proc_code);
  static init_msg_c             get_init_msg(const uint16_t& proc_code);
  static successful_outcome_c   get_successful_outcome(const uint16_t& proc_code);
  static unsuccessful_outcome_c get_unsuccessful_outcome(const uint16_t& proc_code);
  static crit_e                 get_crit(const uint16_t& proc_code);
};

// XNAP-ELEMENTARY-PROCEDURES-CLASS-2 ::= OBJECT SET OF XNAP-ELEMENTARY-PROCEDURE
struct xnap_elem_procs_class_2_o {
  // InitiatingMessage ::= OPEN TYPE
  struct init_msg_c {
    struct types_opts {
      enum options {
        sn_status_transfer,
        ho_cancel,
        ran_paging,
        xn_u_address_ind,
        ue_context_release,
        sn_ode_recfg_complete,
        sn_ode_counter_check_request,
        rrc_transfer,
        error_ind,
        private_msg,
        notif_ctrl_ind,
        activity_notif,
        secondary_rat_data_usage_report,
        deactiv_trace,
        trace_start,
        ho_success,
        conditional_ho_cancel,
        early_status_transfer,
        fail_ind,
        ho_report,
        res_status_upd,
        access_and_mob_ind,
        cell_traffic_trace,
        ran_multicast_group_paging,
        scg_fail_info_report,
        scg_fail_transfer,
        f1_c_traffic_transfer,
        retrieve_ue_context_confirm,
        cp_c_cancel,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    init_msg_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sn_status_transfer_s&                    sn_status_transfer();
    ho_cancel_s&                             ho_cancel();
    ran_paging_s&                            ran_paging();
    xn_u_address_ind_s&                      xn_u_address_ind();
    ue_context_release_s&                    ue_context_release();
    sn_ode_recfg_complete_s&                 sn_ode_recfg_complete();
    sn_ode_counter_check_request_s&          sn_ode_counter_check_request();
    rrc_transfer_s&                          rrc_transfer();
    error_ind_s&                             error_ind();
    private_msg_s&                           private_msg();
    notif_ctrl_ind_s&                        notif_ctrl_ind();
    activity_notif_s&                        activity_notif();
    secondary_rat_data_usage_report_s&       secondary_rat_data_usage_report();
    deactiv_trace_s&                         deactiv_trace();
    trace_start_s&                           trace_start();
    ho_success_s&                            ho_success();
    conditional_ho_cancel_s&                 conditional_ho_cancel();
    early_status_transfer_s&                 early_status_transfer();
    fail_ind_s&                              fail_ind();
    ho_report_s&                             ho_report();
    res_status_upd_s&                        res_status_upd();
    access_and_mob_ind_s&                    access_and_mob_ind();
    cell_traffic_trace_s&                    cell_traffic_trace();
    ran_multicast_group_paging_s&            ran_multicast_group_paging();
    scg_fail_info_report_s&                  scg_fail_info_report();
    scg_fail_transfer_s&                     scg_fail_transfer();
    f1_c_traffic_transfer_s&                 f1_c_traffic_transfer();
    retrieve_ue_context_confirm_s&           retrieve_ue_context_confirm();
    cp_c_cancel_s&                           cp_c_cancel();
    const sn_status_transfer_s&              sn_status_transfer() const;
    const ho_cancel_s&                       ho_cancel() const;
    const ran_paging_s&                      ran_paging() const;
    const xn_u_address_ind_s&                xn_u_address_ind() const;
    const ue_context_release_s&              ue_context_release() const;
    const sn_ode_recfg_complete_s&           sn_ode_recfg_complete() const;
    const sn_ode_counter_check_request_s&    sn_ode_counter_check_request() const;
    const rrc_transfer_s&                    rrc_transfer() const;
    const error_ind_s&                       error_ind() const;
    const private_msg_s&                     private_msg() const;
    const notif_ctrl_ind_s&                  notif_ctrl_ind() const;
    const activity_notif_s&                  activity_notif() const;
    const secondary_rat_data_usage_report_s& secondary_rat_data_usage_report() const;
    const deactiv_trace_s&                   deactiv_trace() const;
    const trace_start_s&                     trace_start() const;
    const ho_success_s&                      ho_success() const;
    const conditional_ho_cancel_s&           conditional_ho_cancel() const;
    const early_status_transfer_s&           early_status_transfer() const;
    const fail_ind_s&                        fail_ind() const;
    const ho_report_s&                       ho_report() const;
    const res_status_upd_s&                  res_status_upd() const;
    const access_and_mob_ind_s&              access_and_mob_ind() const;
    const cell_traffic_trace_s&              cell_traffic_trace() const;
    const ran_multicast_group_paging_s&      ran_multicast_group_paging() const;
    const scg_fail_info_report_s&            scg_fail_info_report() const;
    const scg_fail_transfer_s&               scg_fail_transfer() const;
    const f1_c_traffic_transfer_s&           f1_c_traffic_transfer() const;
    const retrieve_ue_context_confirm_s&     retrieve_ue_context_confirm() const;
    const cp_c_cancel_s&                     cp_c_cancel() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  // SuccessfulOutcome ::= OPEN TYPE
  struct successful_outcome_c {
    struct types_opts {
      enum options { nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::nulltype; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };
  // UnsuccessfulOutcome ::= OPEN TYPE
  struct unsuccessful_outcome_c {
    struct types_opts {
      enum options { nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::nulltype; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // members lookup methods
  static uint16_t               idx_to_proc_code(uint32_t idx);
  static bool                   is_proc_code_valid(const uint16_t& proc_code);
  static init_msg_c             get_init_msg(const uint16_t& proc_code);
  static successful_outcome_c   get_successful_outcome(const uint16_t& proc_code);
  static unsuccessful_outcome_c get_unsuccessful_outcome(const uint16_t& proc_code);
  static crit_e                 get_crit(const uint16_t& proc_code);
};

// XNAP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF XNAP-ELEMENTARY-PROCEDURE
struct xnap_elem_procs_o {
  // InitiatingMessage ::= OPEN TYPE
  struct init_msg_c {
    struct types_opts {
      enum options {
        ho_request,
        retrieve_ue_context_request,
        sn_ode_addition_request,
        sn_ode_mod_request,
        sn_ode_mod_required,
        sn_ode_release_request,
        sn_ode_release_required,
        sn_ode_change_required,
        xn_removal_request,
        xn_setup_request,
        ngran_node_cfg_upd,
        e_utra_nr_cell_res_coordination_request,
        cell_activation_request,
        reset_request,
        res_status_request,
        mob_change_request,
        iab_transport_migration_management_request,
        iab_transport_migration_mod_request,
        iab_res_coordination_request,
        partial_ue_context_transfer,
        sn_status_transfer,
        ho_cancel,
        ran_paging,
        xn_u_address_ind,
        ue_context_release,
        sn_ode_recfg_complete,
        sn_ode_counter_check_request,
        rrc_transfer,
        error_ind,
        private_msg,
        notif_ctrl_ind,
        activity_notif,
        secondary_rat_data_usage_report,
        deactiv_trace,
        trace_start,
        ho_success,
        conditional_ho_cancel,
        early_status_transfer,
        fail_ind,
        ho_report,
        res_status_upd,
        access_and_mob_ind,
        cell_traffic_trace,
        ran_multicast_group_paging,
        scg_fail_info_report,
        scg_fail_transfer,
        f1_c_traffic_transfer,
        retrieve_ue_context_confirm,
        cp_c_cancel,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    init_msg_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ho_request_s&                                       ho_request();
    retrieve_ue_context_request_s&                      retrieve_ue_context_request();
    sn_ode_addition_request_s&                          sn_ode_addition_request();
    sn_ode_mod_request_s&                               sn_ode_mod_request();
    sn_ode_mod_required_s&                              sn_ode_mod_required();
    sn_ode_release_request_s&                           sn_ode_release_request();
    sn_ode_release_required_s&                          sn_ode_release_required();
    sn_ode_change_required_s&                           sn_ode_change_required();
    xn_removal_request_s&                               xn_removal_request();
    xn_setup_request_s&                                 xn_setup_request();
    ngran_node_cfg_upd_s&                               ngran_node_cfg_upd();
    e_utra_nr_cell_res_coordination_request_s&          e_utra_nr_cell_res_coordination_request();
    cell_activation_request_s&                          cell_activation_request();
    reset_request_s&                                    reset_request();
    res_status_request_s&                               res_status_request();
    mob_change_request_s&                               mob_change_request();
    iab_transport_migration_management_request_s&       iab_transport_migration_management_request();
    iab_transport_migration_mod_request_s&              iab_transport_migration_mod_request();
    iab_res_coordination_request_s&                     iab_res_coordination_request();
    partial_ue_context_transfer_s&                      partial_ue_context_transfer();
    sn_status_transfer_s&                               sn_status_transfer();
    ho_cancel_s&                                        ho_cancel();
    ran_paging_s&                                       ran_paging();
    xn_u_address_ind_s&                                 xn_u_address_ind();
    ue_context_release_s&                               ue_context_release();
    sn_ode_recfg_complete_s&                            sn_ode_recfg_complete();
    sn_ode_counter_check_request_s&                     sn_ode_counter_check_request();
    rrc_transfer_s&                                     rrc_transfer();
    error_ind_s&                                        error_ind();
    private_msg_s&                                      private_msg();
    notif_ctrl_ind_s&                                   notif_ctrl_ind();
    activity_notif_s&                                   activity_notif();
    secondary_rat_data_usage_report_s&                  secondary_rat_data_usage_report();
    deactiv_trace_s&                                    deactiv_trace();
    trace_start_s&                                      trace_start();
    ho_success_s&                                       ho_success();
    conditional_ho_cancel_s&                            conditional_ho_cancel();
    early_status_transfer_s&                            early_status_transfer();
    fail_ind_s&                                         fail_ind();
    ho_report_s&                                        ho_report();
    res_status_upd_s&                                   res_status_upd();
    access_and_mob_ind_s&                               access_and_mob_ind();
    cell_traffic_trace_s&                               cell_traffic_trace();
    ran_multicast_group_paging_s&                       ran_multicast_group_paging();
    scg_fail_info_report_s&                             scg_fail_info_report();
    scg_fail_transfer_s&                                scg_fail_transfer();
    f1_c_traffic_transfer_s&                            f1_c_traffic_transfer();
    retrieve_ue_context_confirm_s&                      retrieve_ue_context_confirm();
    cp_c_cancel_s&                                      cp_c_cancel();
    const ho_request_s&                                 ho_request() const;
    const retrieve_ue_context_request_s&                retrieve_ue_context_request() const;
    const sn_ode_addition_request_s&                    sn_ode_addition_request() const;
    const sn_ode_mod_request_s&                         sn_ode_mod_request() const;
    const sn_ode_mod_required_s&                        sn_ode_mod_required() const;
    const sn_ode_release_request_s&                     sn_ode_release_request() const;
    const sn_ode_release_required_s&                    sn_ode_release_required() const;
    const sn_ode_change_required_s&                     sn_ode_change_required() const;
    const xn_removal_request_s&                         xn_removal_request() const;
    const xn_setup_request_s&                           xn_setup_request() const;
    const ngran_node_cfg_upd_s&                         ngran_node_cfg_upd() const;
    const e_utra_nr_cell_res_coordination_request_s&    e_utra_nr_cell_res_coordination_request() const;
    const cell_activation_request_s&                    cell_activation_request() const;
    const reset_request_s&                              reset_request() const;
    const res_status_request_s&                         res_status_request() const;
    const mob_change_request_s&                         mob_change_request() const;
    const iab_transport_migration_management_request_s& iab_transport_migration_management_request() const;
    const iab_transport_migration_mod_request_s&        iab_transport_migration_mod_request() const;
    const iab_res_coordination_request_s&               iab_res_coordination_request() const;
    const partial_ue_context_transfer_s&                partial_ue_context_transfer() const;
    const sn_status_transfer_s&                         sn_status_transfer() const;
    const ho_cancel_s&                                  ho_cancel() const;
    const ran_paging_s&                                 ran_paging() const;
    const xn_u_address_ind_s&                           xn_u_address_ind() const;
    const ue_context_release_s&                         ue_context_release() const;
    const sn_ode_recfg_complete_s&                      sn_ode_recfg_complete() const;
    const sn_ode_counter_check_request_s&               sn_ode_counter_check_request() const;
    const rrc_transfer_s&                               rrc_transfer() const;
    const error_ind_s&                                  error_ind() const;
    const private_msg_s&                                private_msg() const;
    const notif_ctrl_ind_s&                             notif_ctrl_ind() const;
    const activity_notif_s&                             activity_notif() const;
    const secondary_rat_data_usage_report_s&            secondary_rat_data_usage_report() const;
    const deactiv_trace_s&                              deactiv_trace() const;
    const trace_start_s&                                trace_start() const;
    const ho_success_s&                                 ho_success() const;
    const conditional_ho_cancel_s&                      conditional_ho_cancel() const;
    const early_status_transfer_s&                      early_status_transfer() const;
    const fail_ind_s&                                   fail_ind() const;
    const ho_report_s&                                  ho_report() const;
    const res_status_upd_s&                             res_status_upd() const;
    const access_and_mob_ind_s&                         access_and_mob_ind() const;
    const cell_traffic_trace_s&                         cell_traffic_trace() const;
    const ran_multicast_group_paging_s&                 ran_multicast_group_paging() const;
    const scg_fail_info_report_s&                       scg_fail_info_report() const;
    const scg_fail_transfer_s&                          scg_fail_transfer() const;
    const f1_c_traffic_transfer_s&                      f1_c_traffic_transfer() const;
    const retrieve_ue_context_confirm_s&                retrieve_ue_context_confirm() const;
    const cp_c_cancel_s&                                cp_c_cancel() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  // SuccessfulOutcome ::= OPEN TYPE
  struct successful_outcome_c {
    struct types_opts {
      enum options {
        ho_request_ack,
        retrieve_ue_context_resp,
        sn_ode_addition_request_ack,
        sn_ode_mod_request_ack,
        sn_ode_mod_confirm,
        sn_ode_release_request_ack,
        sn_ode_release_confirm,
        sn_ode_change_confirm,
        xn_removal_resp,
        xn_setup_resp,
        ngran_node_cfg_upd_ack,
        e_utra_nr_cell_res_coordination_resp,
        cell_activation_resp,
        reset_resp,
        res_status_resp,
        mob_change_ack,
        iab_transport_migration_management_resp,
        iab_transport_migration_mod_resp,
        iab_res_coordination_resp,
        partial_ue_context_transfer_ack,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    successful_outcome_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ho_request_ack_s&                                ho_request_ack();
    retrieve_ue_context_resp_s&                      retrieve_ue_context_resp();
    sn_ode_addition_request_ack_s&                   sn_ode_addition_request_ack();
    sn_ode_mod_request_ack_s&                        sn_ode_mod_request_ack();
    sn_ode_mod_confirm_s&                            sn_ode_mod_confirm();
    sn_ode_release_request_ack_s&                    sn_ode_release_request_ack();
    sn_ode_release_confirm_s&                        sn_ode_release_confirm();
    sn_ode_change_confirm_s&                         sn_ode_change_confirm();
    xn_removal_resp_s&                               xn_removal_resp();
    xn_setup_resp_s&                                 xn_setup_resp();
    ngran_node_cfg_upd_ack_s&                        ngran_node_cfg_upd_ack();
    e_utra_nr_cell_res_coordination_resp_s&          e_utra_nr_cell_res_coordination_resp();
    cell_activation_resp_s&                          cell_activation_resp();
    reset_resp_s&                                    reset_resp();
    res_status_resp_s&                               res_status_resp();
    mob_change_ack_s&                                mob_change_ack();
    iab_transport_migration_management_resp_s&       iab_transport_migration_management_resp();
    iab_transport_migration_mod_resp_s&              iab_transport_migration_mod_resp();
    iab_res_coordination_resp_s&                     iab_res_coordination_resp();
    partial_ue_context_transfer_ack_s&               partial_ue_context_transfer_ack();
    const ho_request_ack_s&                          ho_request_ack() const;
    const retrieve_ue_context_resp_s&                retrieve_ue_context_resp() const;
    const sn_ode_addition_request_ack_s&             sn_ode_addition_request_ack() const;
    const sn_ode_mod_request_ack_s&                  sn_ode_mod_request_ack() const;
    const sn_ode_mod_confirm_s&                      sn_ode_mod_confirm() const;
    const sn_ode_release_request_ack_s&              sn_ode_release_request_ack() const;
    const sn_ode_release_confirm_s&                  sn_ode_release_confirm() const;
    const sn_ode_change_confirm_s&                   sn_ode_change_confirm() const;
    const xn_removal_resp_s&                         xn_removal_resp() const;
    const xn_setup_resp_s&                           xn_setup_resp() const;
    const ngran_node_cfg_upd_ack_s&                  ngran_node_cfg_upd_ack() const;
    const e_utra_nr_cell_res_coordination_resp_s&    e_utra_nr_cell_res_coordination_resp() const;
    const cell_activation_resp_s&                    cell_activation_resp() const;
    const reset_resp_s&                              reset_resp() const;
    const res_status_resp_s&                         res_status_resp() const;
    const mob_change_ack_s&                          mob_change_ack() const;
    const iab_transport_migration_management_resp_s& iab_transport_migration_management_resp() const;
    const iab_transport_migration_mod_resp_s&        iab_transport_migration_mod_resp() const;
    const iab_res_coordination_resp_s&               iab_res_coordination_resp() const;
    const partial_ue_context_transfer_ack_s&         partial_ue_context_transfer_ack() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  // UnsuccessfulOutcome ::= OPEN TYPE
  struct unsuccessful_outcome_c {
    struct types_opts {
      enum options {
        ho_prep_fail,
        retrieve_ue_context_fail,
        sn_ode_addition_request_reject,
        sn_ode_mod_request_reject,
        sn_ode_mod_refuse,
        sn_ode_release_reject,
        sn_ode_change_refuse,
        xn_removal_fail,
        xn_setup_fail,
        ngran_node_cfg_upd_fail,
        cell_activation_fail,
        res_status_fail,
        mob_change_fail,
        iab_transport_migration_management_reject,
        partial_ue_context_transfer_fail,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    unsuccessful_outcome_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ho_prep_fail_s&                                    ho_prep_fail();
    retrieve_ue_context_fail_s&                        retrieve_ue_context_fail();
    sn_ode_addition_request_reject_s&                  sn_ode_addition_request_reject();
    sn_ode_mod_request_reject_s&                       sn_ode_mod_request_reject();
    sn_ode_mod_refuse_s&                               sn_ode_mod_refuse();
    sn_ode_release_reject_s&                           sn_ode_release_reject();
    sn_ode_change_refuse_s&                            sn_ode_change_refuse();
    xn_removal_fail_s&                                 xn_removal_fail();
    xn_setup_fail_s&                                   xn_setup_fail();
    ngran_node_cfg_upd_fail_s&                         ngran_node_cfg_upd_fail();
    cell_activation_fail_s&                            cell_activation_fail();
    res_status_fail_s&                                 res_status_fail();
    mob_change_fail_s&                                 mob_change_fail();
    iab_transport_migration_management_reject_s&       iab_transport_migration_management_reject();
    partial_ue_context_transfer_fail_s&                partial_ue_context_transfer_fail();
    const ho_prep_fail_s&                              ho_prep_fail() const;
    const retrieve_ue_context_fail_s&                  retrieve_ue_context_fail() const;
    const sn_ode_addition_request_reject_s&            sn_ode_addition_request_reject() const;
    const sn_ode_mod_request_reject_s&                 sn_ode_mod_request_reject() const;
    const sn_ode_mod_refuse_s&                         sn_ode_mod_refuse() const;
    const sn_ode_release_reject_s&                     sn_ode_release_reject() const;
    const sn_ode_change_refuse_s&                      sn_ode_change_refuse() const;
    const xn_removal_fail_s&                           xn_removal_fail() const;
    const xn_setup_fail_s&                             xn_setup_fail() const;
    const ngran_node_cfg_upd_fail_s&                   ngran_node_cfg_upd_fail() const;
    const cell_activation_fail_s&                      cell_activation_fail() const;
    const res_status_fail_s&                           res_status_fail() const;
    const mob_change_fail_s&                           mob_change_fail() const;
    const iab_transport_migration_management_reject_s& iab_transport_migration_management_reject() const;
    const partial_ue_context_transfer_fail_s&          partial_ue_context_transfer_fail() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint16_t               idx_to_proc_code(uint32_t idx);
  static bool                   is_proc_code_valid(const uint16_t& proc_code);
  static init_msg_c             get_init_msg(const uint16_t& proc_code);
  static successful_outcome_c   get_successful_outcome(const uint16_t& proc_code);
  static unsuccessful_outcome_c get_unsuccessful_outcome(const uint16_t& proc_code);
  static crit_e                 get_crit(const uint16_t& proc_code);
};

// InitiatingMessage ::= SEQUENCE{{XNAP-ELEMENTARY-PROCEDURE}}
struct init_msg_s {
  uint16_t                      proc_code = 0;
  crit_e                        crit;
  xnap_elem_procs_o::init_msg_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// SuccessfulOutcome ::= SEQUENCE{{XNAP-ELEMENTARY-PROCEDURE}}
struct successful_outcome_s {
  uint16_t                                proc_code = 0;
  crit_e                                  crit;
  xnap_elem_procs_o::successful_outcome_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// UnsuccessfulOutcome ::= SEQUENCE{{XNAP-ELEMENTARY-PROCEDURE}}
struct unsuccessful_outcome_s {
  uint16_t                                  proc_code = 0;
  crit_e                                    crit;
  xnap_elem_procs_o::unsuccessful_outcome_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// XnAP-PDU ::= CHOICE
struct xn_ap_pdu_c {
  struct types_opts {
    enum options { init_msg, successful_outcome, unsuccessful_outcome, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  xn_ap_pdu_c() = default;
  xn_ap_pdu_c(const xn_ap_pdu_c& other);
  xn_ap_pdu_c& operator=(const xn_ap_pdu_c& other);
  ~xn_ap_pdu_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  init_msg_s& init_msg()
  {
    assert_choice_type(types::init_msg, type_, "XnAP-PDU");
    return c.get<init_msg_s>();
  }
  successful_outcome_s& successful_outcome()
  {
    assert_choice_type(types::successful_outcome, type_, "XnAP-PDU");
    return c.get<successful_outcome_s>();
  }
  unsuccessful_outcome_s& unsuccessful_outcome()
  {
    assert_choice_type(types::unsuccessful_outcome, type_, "XnAP-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  const init_msg_s& init_msg() const
  {
    assert_choice_type(types::init_msg, type_, "XnAP-PDU");
    return c.get<init_msg_s>();
  }
  const successful_outcome_s& successful_outcome() const
  {
    assert_choice_type(types::successful_outcome, type_, "XnAP-PDU");
    return c.get<successful_outcome_s>();
  }
  const unsuccessful_outcome_s& unsuccessful_outcome() const
  {
    assert_choice_type(types::unsuccessful_outcome, type_, "XnAP-PDU");
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

} // namespace xnap
} // namespace asn1
