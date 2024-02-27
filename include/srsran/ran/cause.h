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

#include "srsran/adt/variant.h"

namespace srsran {

constexpr uint16_t NGAP_RADIO_NETWORK_CAUSE_OFFSET = 100;
constexpr uint16_t F1AP_RADIO_NETWORK_CAUSE_OFFSET = 200;
constexpr uint16_t E1AP_RADIO_NETWORK_CAUSE_OFFSET = 300;

enum class cause_radio_network_t : uint16_t {
  // Common
  unspecified = 0,
  interaction_with_other_proc,
  res_not_available_for_the_slice,

  // NGAP and F1AP
  cell_not_available,

  // NGAP and E1AP
  not_supported_5qi_value,
  up_integrity_protection_not_possible,
  up_confidentiality_protection_not_possible,
  multiple_pdu_session_id_instances,
  unknown_pdu_session_id,
  multiple_qos_flow_id_instances,
  invalid_qos_combination,

  // F1AP and E1AP
  not_supported_qci_value,
  multiple_drb_id_instances,
  unknown_drb_id,
  proc_cancelled,
  normal_release,
  no_radio_res_available,
  action_desirable_for_radio_reasons,
  release_due_to_pre_emption,
  npn_not_supported,
  existing_meas_id,
  meas_temporarily_not_available,
  meas_not_supported_for_the_obj,

  // NGAP
  txnrelocoverall_expiry = NGAP_RADIO_NETWORK_CAUSE_OFFSET + 1,
  successful_ho,
  release_due_to_ngran_generated_reason,
  release_due_to_5gc_generated_reason,
  ho_cancelled,
  partial_ho,
  ho_fail_in_target_5_gc_ngran_node_or_target_sys,
  ho_target_not_allowed,
  tngrelocoverall_expiry,
  tngrelocprep_expiry,
  unknown_target_id = NGAP_RADIO_NETWORK_CAUSE_OFFSET + 12,
  no_radio_res_available_in_target_cell,
  unknown_local_ue_ngap_id,
  inconsistent_remote_ue_ngap_id,
  ho_desirable_for_radio_reason,
  time_crit_ho,
  res_optim_ho,
  reduce_load_in_serving_cell,
  user_inactivity,
  radio_conn_with_ue_lost,
  radio_res_not_available,
  fail_in_radio_interface_proc                                    = NGAP_RADIO_NETWORK_CAUSE_OFFSET + 24,
  unkown_qos_flow_id                                              = NGAP_RADIO_NETWORK_CAUSE_OFFSET + 27,
  encryption_and_or_integrity_protection_algorithms_not_supported = NGAP_RADIO_NETWORK_CAUSE_OFFSET + 30,
  ng_intra_sys_ho_triggered,
  ng_inter_sys_ho_triggered,
  xn_ho_triggered,
  ue_context_transfer = NGAP_RADIO_NETWORK_CAUSE_OFFSET + 35,
  ims_voice_eps_fallback_or_rat_fallback_triggered,
  slice_not_supported = NGAP_RADIO_NETWORK_CAUSE_OFFSET + 39,
  ue_in_rrc_inactive_state_not_reachable,
  redirection,
  ue_max_integrity_protected_data_rate_reason = NGAP_RADIO_NETWORK_CAUSE_OFFSET + 43,
  release_due_to_cn_detected_mob,

  // F1AP
  rl_fail_rlc = F1AP_RADIO_NETWORK_CAUSE_OFFSET + 1,
  unknown_or_already_allocated_gnb_cu_ue_f1ap_id,
  unknown_or_already_allocated_gnb_du_ue_f1ap_id,
  unknown_or_inconsistent_pair_of_ue_f1ap_id,
  rl_fail_others = F1AP_RADIO_NETWORK_CAUSE_OFFSET + 12,
  ue_rejection,
  amf_initiated_abnormal_release  = F1AP_RADIO_NETWORK_CAUSE_OFFSET + 15,
  plmn_not_served_by_the_gnb_cu   = F1AP_RADIO_NETWORK_CAUSE_OFFSET + 17,
  multiple_bh_rlc_ch_id_instances = F1AP_RADIO_NETWORK_CAUSE_OFFSET + 20,
  unknown_bh_rlc_ch_id,
  cho_cpc_res_tobechanged,
  npn_access_denied = F1AP_RADIO_NETWORK_CAUSE_OFFSET + 24,
  gnb_cu_cell_capacity_exceeded,
  report_characteristics_empty,
  unknown_bh_address = F1AP_RADIO_NETWORK_CAUSE_OFFSET + 30,
  unknown_bap_routing_id,
  insufficient_ue_cap,
  scg_activation_deactivation_fail,
  scg_deactivation_fail_due_to_data_tx,
  requested_item_not_supported_on_time,
  unknown_or_already_allocated_gnb_cu_mbs_f1ap_id,
  unknown_or_already_allocated_gnb_du_mbs_f1ap_id,
  unknown_or_inconsistent_pair_of_mbs_f1ap_id,
  unknown_or_inconsistent_mrb_id,
  tat_sdt_expiry,

  // E1AP
  unknown_or_already_allocated_gnb_cu_cp_ue_e1ap_id = E1AP_RADIO_NETWORK_CAUSE_OFFSET + 1,
  unknown_or_already_allocated_gnb_cu_up_ue_e1ap_id,
  unknown_or_inconsistent_pair_of_ue_e1ap_id,
  ppdcp_count_wrap_around             = E1AP_RADIO_NETWORK_CAUSE_OFFSET + 5,
  encryption_algorithms_not_supported = E1AP_RADIO_NETWORK_CAUSE_OFFSET + 8,
  integrity_protection_algorithms_not_supported,
  unknown_qos_flow_id    = E1AP_RADIO_NETWORK_CAUSE_OFFSET + 15,
  pdcp_cfg_not_supported = E1AP_RADIO_NETWORK_CAUSE_OFFSET + 24,
  ue_dl_max_ip_data_rate_reason,
  up_integrity_protection_fail,
  rsn_not_available_for_the_up = E1AP_RADIO_NETWORK_CAUSE_OFFSET + 28,
  report_characteristic_empty  = E1AP_RADIO_NETWORK_CAUSE_OFFSET + 30
};

enum class cause_transport_t : uint8_t {
  transport_res_unavailable = 0,
  unspecified,
  unknown_tnl_address_for_iab, // only F1AP and E1AP
  unknown_up_tnl_info_for_iab  // only F1AP
};

enum class cause_nas_t : uint8_t { normal_release = 0, authentication_fail, deregister, unspecified }; // only NGAP

enum class cause_protocol_t : uint8_t {
  transfer_syntax_error = 0,
  abstract_syntax_error_reject,
  abstract_syntax_error_ignore_and_notify,
  msg_not_compatible_with_receiver_state,
  semantic_error,
  abstract_syntax_error_falsely_constructed_msg,
  unspecified
};

enum class cause_misc_t : uint8_t {
  ctrl_processing_overload = 0,
  not_enough_user_plane_processing_res,
  hardware_fail,
  om_intervention,
  unknown_plmn_or_sn_pn, // only NGAP
  unspecified
};

using cause_t = variant<cause_radio_network_t, cause_transport_t, cause_nas_t, cause_protocol_t, cause_misc_t>;

// Establishment cause

enum class establishment_cause_t : uint8_t {
  emergency = 0,
  high_prio_access,
  mt_access,
  mo_sig,
  mo_data,
  mo_voice_call,
  mo_video_call,
  mo_sms,
  mps_prio_access,
  mcs_prio_access
};

} // namespace srsran

namespace fmt {

// cause_t formatter
template <>
struct formatter<srsran::cause_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::cause_t o, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    if (srsran::variant_holds_alternative<srsran::cause_radio_network_t>(o)) {
      return format_to(ctx.out(), "radio_network-id{}", srsran::variant_get<srsran::cause_radio_network_t>(o));
    } else if (srsran::variant_holds_alternative<srsran::cause_transport_t>(o)) {
      return format_to(ctx.out(), "transport-id{}", srsran::variant_get<srsran::cause_transport_t>(o));
    } else if (srsran::variant_holds_alternative<srsran::cause_nas_t>(o)) {
      return format_to(ctx.out(), "nas-id{}", srsran::variant_get<srsran::cause_nas_t>(o));
    } else if (srsran::variant_holds_alternative<srsran::cause_protocol_t>(o)) {
      return format_to(ctx.out(), "protocol-id{}", srsran::variant_get<srsran::cause_protocol_t>(o));
    } else {
      return format_to(ctx.out(), "misc-id{}", srsran::variant_get<srsran::cause_misc_t>(o));
    }
  }
};

} // namespace fmt
