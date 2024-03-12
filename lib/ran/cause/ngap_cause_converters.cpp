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

#include "srsran/ran/cause/ngap_cause_converters.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

const uint8_t ngap_to_f1ap_cause_radio_network[] = {
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // unspecified
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // txnrelocoverall_expiry
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // successful_ho
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // release_due_to_ngran_generated_reason
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // release_due_to_5gc_generated_reason
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // ho_cancelled
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // partial_ho
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // ho_fail_in_target_5_gc_ngran_node_or_target_sys
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // ho_target_not_allowed
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // tngrelocoverall_expiry
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // tngrelocprep_expiry
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // cell_not_available
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // unknown_target_id
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // no_radio_res_available_in_target_cell
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // unknown_local_ue_ngap_id
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // inconsistent_remote_ue_ngap_id
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // ho_desirable_for_radio_reason
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // time_crit_ho
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // res_optim_ho
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // reduce_load_in_serving_cell
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // user_inactivity
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // radio_conn_with_ue_lost
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // radio_res_not_available
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // invalid_qos_combination
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // fail_in_radio_interface_proc
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // interaction_with_other_proc
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // unknown_pdu_session_id
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // unkown_qos_flow_id
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // multiple_pdu_session_id_instances
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // multiple_qos_flow_id_instances
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // encryption_and_or_integrity_protection_algorithms_not_supported
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // ng_intra_sys_ho_triggered
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // ng_inter_sys_ho_triggered
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // xn_ho_triggered
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // not_supported_5qi_value
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // ue_context_transfer
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // ims_voice_eps_fallback_or_rat_fallback_triggered
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // up_integrity_protection_not_possible
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // up_confidentiality_protection_not_possible
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // slice_not_supported
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // ue_in_rrc_inactive_state_not_reachable
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // redirection
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // res_not_available_for_the_slice
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // ue_max_integrity_protected_data_rate_reason
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // release_due_to_cn_detected_mob
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // n26_interface_not_available
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // release_due_to_pre_emption
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // multiple_location_report_ref_id_instances
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // rsn_not_available_for_the_up
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // npn_access_denied
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // cag_only_access_denied
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // insufficient_ue_cap
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // redcap_ue_not_supported
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // unknown_mbs_session_id
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // indicated_mbs_session_area_info_not_served_by_the_gnb
    (uint8_t)f1ap_cause_radio_network_t::unspecified, // inconsistent_slice_info_for_the_session
    (uint8_t)f1ap_cause_radio_network_t::unspecified  // misaligned_assoc_for_multicast_unicast
};

const uint8_t ngap_to_f1ap_cause_transport[] = {
    (uint8_t)f1ap_cause_transport_t::transport_res_unavailable, // transport_res_unavailable
    (uint8_t)f1ap_cause_transport_t::unspecified                // unspecified
};

const uint8_t ngap_to_f1ap_cause_misc[] = {
    (uint8_t)cause_misc_t::unspecified, // ctrl_processing_overload
    (uint8_t)cause_misc_t::unspecified, // not_enough_user_plane_processing_res
    (uint8_t)cause_misc_t::unspecified, // hardware_fail
    (uint8_t)cause_misc_t::unspecified, // om_intervention
    (uint8_t)cause_misc_t::unspecified, // unknown_plmn_or_sn_pn
    (uint8_t)cause_misc_t::unspecified, // unspecified
};

f1ap_cause_t srsran::ngap_to_f1ap_cause(ngap_cause_t ngap_cause)
{
  f1ap_cause_t f1ap_cause;

  if (variant_holds_alternative<ngap_cause_radio_network_t>(ngap_cause)) {
    f1ap_cause = f1ap_cause_radio_network_t(
        ngap_to_f1ap_cause_radio_network[uint8_t(variant_get<ngap_cause_radio_network_t>(ngap_cause))]);
  } else if (variant_holds_alternative<ngap_cause_transport_t>(ngap_cause)) {
    f1ap_cause =
        f1ap_cause_transport_t(ngap_to_f1ap_cause_transport[uint8_t(variant_get<ngap_cause_transport_t>(ngap_cause))]);
  } else if (variant_holds_alternative<cause_nas_t>(ngap_cause)) {
    switch (variant_get<cause_nas_t>(ngap_cause)) {
      case cause_nas_t::normal_release:
        f1ap_cause = f1ap_cause_radio_network_t::normal_release;
        break;
      case cause_nas_t::authentication_fail:
        f1ap_cause = f1ap_cause_radio_network_t::unspecified;
        break;
      case cause_nas_t::deregister:
        f1ap_cause = f1ap_cause_radio_network_t::normal_release;
        break;
      case cause_nas_t::unspecified:
        f1ap_cause = f1ap_cause_radio_network_t::unspecified;
        break;
      default:
        report_fatal_error("Cannot convert cause to F1AP type: {}", ngap_cause);
    }
  } else if (variant_holds_alternative<cause_protocol_t>(ngap_cause)) {
    f1ap_cause = variant_get<cause_protocol_t>(ngap_cause);
  } else if (variant_holds_alternative<ngap_cause_misc_t>(ngap_cause)) {
    f1ap_cause = cause_misc_t(ngap_to_f1ap_cause_misc[uint8_t(variant_get<ngap_cause_misc_t>(ngap_cause))]);
  } else {
    report_fatal_error("Cannot convert cause to F1AP type: {}", ngap_cause);
  }

  return f1ap_cause;
};

const uint8_t ngap_to_e1ap_cause_radio_network[] = {
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // unspecified
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // txnrelocoverall_expiry
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // successful_ho
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // release_due_to_ngran_generated_reason
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // release_due_to_5gc_generated_reason
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // ho_cancelled
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // partial_ho
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // ho_fail_in_target_5_gc_ngran_node_or_target_sys
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // ho_target_not_allowed
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // tngrelocoverall_expiry
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // tngrelocprep_expiry
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // cell_not_available
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // unknown_target_id
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // no_radio_res_available_in_target_cell
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // unknown_local_ue_ngap_id
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // inconsistent_remote_ue_ngap_id
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // ho_desirable_for_radio_reason
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // time_crit_ho
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // res_optim_ho
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // reduce_load_in_serving_cell
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // user_inactivity
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // radio_conn_with_ue_lost
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // radio_res_not_available
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // invalid_qos_combination
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // fail_in_radio_interface_proc
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // interaction_with_other_proc
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // unknown_pdu_session_id
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // unkown_qos_flow_id
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // multiple_pdu_session_id_instances
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // multiple_qos_flow_id_instances
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // encryption_and_or_integrity_protection_algorithms_not_supported
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // ng_intra_sys_ho_triggered
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // ng_inter_sys_ho_triggered
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // xn_ho_triggered
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // not_supported_5qi_value
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // ue_context_transfer
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // ims_voice_eps_fallback_or_rat_fallback_triggered
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // up_integrity_protection_not_possible
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // up_confidentiality_protection_not_possible
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // slice_not_supported
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // ue_in_rrc_inactive_state_not_reachable
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // redirection
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // res_not_available_for_the_slice
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // ue_max_integrity_protected_data_rate_reason
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // release_due_to_cn_detected_mob
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // n26_interface_not_available
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // release_due_to_pre_emption
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // multiple_location_report_ref_id_instances
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // rsn_not_available_for_the_up
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // npn_access_denied
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // cag_only_access_denied
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // insufficient_ue_cap
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // redcap_ue_not_supported
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // unknown_mbs_session_id
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // indicated_mbs_session_area_info_not_served_by_the_gnb
    (uint8_t)e1ap_cause_radio_network_t::unspecified, // inconsistent_slice_info_for_the_session
    (uint8_t)e1ap_cause_radio_network_t::unspecified  // misaligned_assoc_for_multicast_unicast
};

const uint8_t ngap_to_e1ap_cause_transport[] = {
    (uint8_t)e1ap_cause_transport_t::transport_res_unavailable, // transport_res_unavailable
    (uint8_t)e1ap_cause_transport_t::unspecified                // unspecified
};

const uint8_t ngap_to_e1ap_cause_misc[] = {
    (uint8_t)cause_misc_t::unspecified, // ctrl_processing_overload
    (uint8_t)cause_misc_t::unspecified, // not_enough_user_plane_processing_res
    (uint8_t)cause_misc_t::unspecified, // hardware_fail
    (uint8_t)cause_misc_t::unspecified, // om_intervention
    (uint8_t)cause_misc_t::unspecified, // unknown_plmn_or_sn_pn
    (uint8_t)cause_misc_t::unspecified  // unspecified
};

e1ap_cause_t srsran::ngap_to_e1ap_cause(ngap_cause_t ngap_cause)
{
  e1ap_cause_t e1ap_cause;

  if (variant_holds_alternative<ngap_cause_radio_network_t>(ngap_cause)) {
    e1ap_cause = e1ap_cause_radio_network_t(
        ngap_to_e1ap_cause_radio_network[uint8_t(variant_get<ngap_cause_radio_network_t>(ngap_cause))]);
  } else if (variant_holds_alternative<ngap_cause_transport_t>(ngap_cause)) {
    e1ap_cause =
        e1ap_cause_transport_t(ngap_to_e1ap_cause_transport[uint8_t(variant_get<ngap_cause_transport_t>(ngap_cause))]);
  } else if (variant_holds_alternative<cause_nas_t>(ngap_cause)) {
    switch (variant_get<cause_nas_t>(ngap_cause)) {
      case cause_nas_t::normal_release:
        e1ap_cause = e1ap_cause_radio_network_t::normal_release;
        break;
      case cause_nas_t::authentication_fail:
        e1ap_cause = e1ap_cause_radio_network_t::unspecified;
        break;
      case cause_nas_t::deregister:
        e1ap_cause = e1ap_cause_radio_network_t::normal_release;
        break;
      case cause_nas_t::unspecified:
        e1ap_cause = e1ap_cause_radio_network_t::unspecified;
        break;
      default:
        report_fatal_error("Cannot convert cause to E1AP type: {}", ngap_cause);
    }
  } else if (variant_holds_alternative<cause_protocol_t>(ngap_cause)) {
    e1ap_cause = variant_get<cause_protocol_t>(ngap_cause);
  } else if (variant_holds_alternative<ngap_cause_misc_t>(ngap_cause)) {
    e1ap_cause = cause_misc_t(ngap_to_e1ap_cause_misc[uint8_t(variant_get<ngap_cause_misc_t>(ngap_cause))]);
  } else {
    report_fatal_error("Cannot convert cause to E1AP type: {}", ngap_cause);
  }

  return e1ap_cause;
};
