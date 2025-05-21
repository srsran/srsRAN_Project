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

#include "srsran/ran/cause/common.h"
#include "fmt/format.h"
#include <variant>

namespace srsran {

/// The NGAP radio network cause, see TS 38.413 section 9.3.1.2.
enum class ngap_cause_radio_network_t : uint8_t {
  unspecified = 0,
  txnrelocoverall_expiry,
  successful_ho,
  release_due_to_ngran_generated_reason,
  release_due_to_5gc_generated_reason,
  ho_cancelled,
  partial_ho,
  ho_fail_in_target_5_gc_ngran_node_or_target_sys,
  ho_target_not_allowed,
  tngrelocoverall_expiry,
  tngrelocprep_expiry,
  cell_not_available,
  unknown_target_id,
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
  invalid_qos_combination,
  fail_in_radio_interface_proc,
  interaction_with_other_proc,
  unknown_pdu_session_id,
  unkown_qos_flow_id,
  multiple_pdu_session_id_instances,
  multiple_qos_flow_id_instances,
  encryption_and_or_integrity_protection_algorithms_not_supported,
  ng_intra_sys_ho_triggered,
  ng_inter_sys_ho_triggered,
  xn_ho_triggered,
  not_supported_5qi_value,
  ue_context_transfer,
  ims_voice_eps_fallback_or_rat_fallback_triggered,
  up_integrity_protection_not_possible,
  up_confidentiality_protection_not_possible,
  slice_not_supported,
  ue_in_rrc_inactive_state_not_reachable,
  redirection,
  res_not_available_for_the_slice,
  ue_max_integrity_protected_data_rate_reason,
  release_due_to_cn_detected_mob,
  n26_interface_not_available,
  release_due_to_pre_emption,
  multiple_location_report_ref_id_instances,
  rsn_not_available_for_the_up,
  npn_access_denied,
  cag_only_access_denied,
  insufficient_ue_cap,
  redcap_ue_not_supported,
  unknown_mbs_session_id,
  indicated_mbs_session_area_info_not_served_by_the_gnb,
  inconsistent_slice_info_for_the_session,
  misaligned_assoc_for_multicast_unicast
};

/// The NGAP transport cause, see TS 38.413 section 9.3.1.2.
enum class ngap_cause_transport_t : uint8_t {
  transport_res_unavailable = 0,
  unspecified,
};

/// The NGAP NAS cause, see TS 38.413 section 9.3.1.2.
enum class cause_nas_t : uint8_t { normal_release = 0, authentication_fail, deregister, unspecified }; // only NGAP

/// The NGAP misc cause, see TS 38.413 section 9.3.1.2.
enum class ngap_cause_misc_t : uint8_t {
  ctrl_processing_overload = 0,
  not_enough_user_plane_processing_res,
  hardware_fail,
  om_intervention,
  unknown_plmn_or_sn_pn,
  unspecified
};

/// The NGAP cause to indicate the reason for a particular event, see TS 38.413 section 9.3.1.2.
/// The NGAP cause is a union of the radio network cause, transport cause, nas cause, protocol cause and misc cause.
using ngap_cause_t =
    std::variant<ngap_cause_radio_network_t, ngap_cause_transport_t, cause_nas_t, cause_protocol_t, ngap_cause_misc_t>;

} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::ngap_cause_radio_network_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::ngap_cause_radio_network_t& cause, FormatContext& ctx) const
  {
    if (cause == srsran::ngap_cause_radio_network_t::unspecified) {
      return format_to(ctx.out(), "unspecified");
    }
    if (cause == srsran::ngap_cause_radio_network_t::txnrelocoverall_expiry) {
      return format_to(ctx.out(), "txnrelocoverall_expiry");
    }
    if (cause == srsran::ngap_cause_radio_network_t::successful_ho) {
      return format_to(ctx.out(), "successful_ho");
    }
    if (cause == srsran::ngap_cause_radio_network_t::release_due_to_ngran_generated_reason) {
      return format_to(ctx.out(), "release_due_to_ngran_generated_reason");
    }
    if (cause == srsran::ngap_cause_radio_network_t::release_due_to_5gc_generated_reason) {
      return format_to(ctx.out(), "release_due_to_5gc_generated_reason");
    }
    if (cause == srsran::ngap_cause_radio_network_t::ho_cancelled) {
      return format_to(ctx.out(), "ho_cancelled");
    }
    if (cause == srsran::ngap_cause_radio_network_t::partial_ho) {
      return format_to(ctx.out(), "partial_ho");
    }
    if (cause == srsran::ngap_cause_radio_network_t::ho_fail_in_target_5_gc_ngran_node_or_target_sys) {
      return format_to(ctx.out(), "ho_fail_in_target_5_gc_ngran_node_or_target_sys");
    }
    if (cause == srsran::ngap_cause_radio_network_t::ho_target_not_allowed) {
      return format_to(ctx.out(), "ho_target_not_allowed");
    }
    if (cause == srsran::ngap_cause_radio_network_t::tngrelocoverall_expiry) {
      return format_to(ctx.out(), "tngrelocoverall_expiry");
    }
    if (cause == srsran::ngap_cause_radio_network_t::tngrelocprep_expiry) {
      return format_to(ctx.out(), "tngrelocprep_expiry");
    }
    if (cause == srsran::ngap_cause_radio_network_t::cell_not_available) {
      return format_to(ctx.out(), "cell_not_available");
    }
    if (cause == srsran::ngap_cause_radio_network_t::unknown_target_id) {
      return format_to(ctx.out(), "unknown_target_id");
    }
    if (cause == srsran::ngap_cause_radio_network_t::no_radio_res_available_in_target_cell) {
      return format_to(ctx.out(), "no_radio_res_available_in_target_cell");
    }
    if (cause == srsran::ngap_cause_radio_network_t::unknown_local_ue_ngap_id) {
      return format_to(ctx.out(), "unknown_local_ue_ngap_id");
    }
    if (cause == srsran::ngap_cause_radio_network_t::inconsistent_remote_ue_ngap_id) {
      return format_to(ctx.out(), "inconsistent_remote_ue_ngap_id");
    }
    if (cause == srsran::ngap_cause_radio_network_t::ho_desirable_for_radio_reason) {
      return format_to(ctx.out(), "ho_desirable_for_radio_reason");
    }
    if (cause == srsran::ngap_cause_radio_network_t::time_crit_ho) {
      return format_to(ctx.out(), "time_crit_ho");
    }
    if (cause == srsran::ngap_cause_radio_network_t::res_optim_ho) {
      return format_to(ctx.out(), "res_optim_ho");
    }
    if (cause == srsran::ngap_cause_radio_network_t::reduce_load_in_serving_cell) {
      return format_to(ctx.out(), "reduce_load_in_serving_cell");
    }
    if (cause == srsran::ngap_cause_radio_network_t::user_inactivity) {
      return format_to(ctx.out(), "user_inactivity");
    }
    if (cause == srsran::ngap_cause_radio_network_t::radio_conn_with_ue_lost) {
      return format_to(ctx.out(), "radio_conn_with_ue_lost");
    }
    if (cause == srsran::ngap_cause_radio_network_t::radio_res_not_available) {
      return format_to(ctx.out(), "radio_res_not_available");
    }
    if (cause == srsran::ngap_cause_radio_network_t::invalid_qos_combination) {
      return format_to(ctx.out(), "invalid_qos_combination");
    }
    if (cause == srsran::ngap_cause_radio_network_t::fail_in_radio_interface_proc) {
      return format_to(ctx.out(), "fail_in_radio_interface_proc");
    }
    if (cause == srsran::ngap_cause_radio_network_t::interaction_with_other_proc) {
      return format_to(ctx.out(), "interaction_with_other_proc");
    }
    if (cause == srsran::ngap_cause_radio_network_t::unknown_pdu_session_id) {
      return format_to(ctx.out(), "unknown_pdu_session_id");
    }
    if (cause == srsran::ngap_cause_radio_network_t::unkown_qos_flow_id) {
      return format_to(ctx.out(), "unkown_qos_flow_id");
    }
    if (cause == srsran::ngap_cause_radio_network_t::multiple_pdu_session_id_instances) {
      return format_to(ctx.out(), "multiple_pdu_session_id_instances");
    }
    if (cause == srsran::ngap_cause_radio_network_t::multiple_qos_flow_id_instances) {
      return format_to(ctx.out(), "multiple_qos_flow_id_instances");
    }
    if (cause == srsran::ngap_cause_radio_network_t::encryption_and_or_integrity_protection_algorithms_not_supported) {
      return format_to(ctx.out(), "encryption_and_or_integrity_protection_algorithms_not_supported");
    }
    if (cause == srsran::ngap_cause_radio_network_t::ng_intra_sys_ho_triggered) {
      return format_to(ctx.out(), "ng_intra_sys_ho_triggered");
    }
    if (cause == srsran::ngap_cause_radio_network_t::ng_inter_sys_ho_triggered) {
      return format_to(ctx.out(), "ng_inter_sys_ho_triggered");
    }
    if (cause == srsran::ngap_cause_radio_network_t::xn_ho_triggered) {
      return format_to(ctx.out(), "xn_ho_triggered");
    }
    if (cause == srsran::ngap_cause_radio_network_t::not_supported_5qi_value) {
      return format_to(ctx.out(), "not_supported_5qi_value");
    }
    if (cause == srsran::ngap_cause_radio_network_t::ue_context_transfer) {
      return format_to(ctx.out(), "ue_context_transfer");
    }
    if (cause == srsran::ngap_cause_radio_network_t::ims_voice_eps_fallback_or_rat_fallback_triggered) {
      return format_to(ctx.out(), "ims_voice_eps_fallback_or_rat_fallback_triggered");
    }
    if (cause == srsran::ngap_cause_radio_network_t::up_integrity_protection_not_possible) {
      return format_to(ctx.out(), "up_integrity_protection_not_possible");
    }
    if (cause == srsran::ngap_cause_radio_network_t::up_confidentiality_protection_not_possible) {
      return format_to(ctx.out(), "up_confidentiality_protection_not_possible");
    }
    if (cause == srsran::ngap_cause_radio_network_t::slice_not_supported) {
      return format_to(ctx.out(), "slice_not_supported");
    }
    if (cause == srsran::ngap_cause_radio_network_t::ue_in_rrc_inactive_state_not_reachable) {
      return format_to(ctx.out(), "ue_in_rrc_inactive_state_not_reachable");
    }
    if (cause == srsran::ngap_cause_radio_network_t::redirection) {
      return format_to(ctx.out(), "redirection");
    }
    if (cause == srsran::ngap_cause_radio_network_t::res_not_available_for_the_slice) {
      return format_to(ctx.out(), "res_not_available_for_the_slice");
    }
    if (cause == srsran::ngap_cause_radio_network_t::ue_max_integrity_protected_data_rate_reason) {
      return format_to(ctx.out(), "ue_max_integrity_protected_data_rate_reason");
    }
    if (cause == srsran::ngap_cause_radio_network_t::release_due_to_cn_detected_mob) {
      return format_to(ctx.out(), "release_due_to_cn_detected_mob");
    }
    if (cause == srsran::ngap_cause_radio_network_t::n26_interface_not_available) {
      return format_to(ctx.out(), "n26_interface_not_available");
    }
    if (cause == srsran::ngap_cause_radio_network_t::release_due_to_pre_emption) {
      return format_to(ctx.out(), "release_due_to_pre_emption");
    }
    if (cause == srsran::ngap_cause_radio_network_t::multiple_location_report_ref_id_instances) {
      return format_to(ctx.out(), "multiple_location_report_ref_id_instances");
    }
    if (cause == srsran::ngap_cause_radio_network_t::rsn_not_available_for_the_up) {
      return format_to(ctx.out(), "rsn_not_available_for_the_up");
    }
    if (cause == srsran::ngap_cause_radio_network_t::npn_access_denied) {
      return format_to(ctx.out(), "npn_access_denied");
    }
    if (cause == srsran::ngap_cause_radio_network_t::cag_only_access_denied) {
      return format_to(ctx.out(), "cag_only_access_denied");
    }
    if (cause == srsran::ngap_cause_radio_network_t::insufficient_ue_cap) {
      return format_to(ctx.out(), "insufficient_ue_cap");
    }
    if (cause == srsran::ngap_cause_radio_network_t::redcap_ue_not_supported) {
      return format_to(ctx.out(), "redcap_ue_not_supported");
    }
    if (cause == srsran::ngap_cause_radio_network_t::unknown_mbs_session_id) {
      return format_to(ctx.out(), "unknown_mbs_session_id");
    }
    if (cause == srsran::ngap_cause_radio_network_t::indicated_mbs_session_area_info_not_served_by_the_gnb) {
      return format_to(ctx.out(), "indicated_mbs_session_area_info_not_served_by_the_gnb");
    }
    if (cause == srsran::ngap_cause_radio_network_t::inconsistent_slice_info_for_the_session) {
      return format_to(ctx.out(), "inconsistent_slice_info_for_the_session");
    }
    if (cause == srsran::ngap_cause_radio_network_t::misaligned_assoc_for_multicast_unicast) {
      return format_to(ctx.out(), "misaligned_assoc_for_multicast_unicast");
    }

    return format_to(ctx.out(), "unknown");
  }
};

template <>
struct formatter<srsran::ngap_cause_transport_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::ngap_cause_transport_t& cause, FormatContext& ctx) const
  {
    if (cause == srsran::ngap_cause_transport_t::transport_res_unavailable) {
      return format_to(ctx.out(), "transport_res_unavailable");
    }
    if (cause == srsran::ngap_cause_transport_t::unspecified) {
      return format_to(ctx.out(), "unspecified");
    }

    return format_to(ctx.out(), "unknown");
  }
};

template <>
struct formatter<srsran::cause_nas_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::cause_nas_t& cause, FormatContext& ctx) const
  {
    if (cause == srsran::cause_nas_t::normal_release) {
      return format_to(ctx.out(), "normal_release");
    }
    if (cause == srsran::cause_nas_t::authentication_fail) {
      return format_to(ctx.out(), "authentication_fail");
    }
    if (cause == srsran::cause_nas_t::deregister) {
      return format_to(ctx.out(), "deregister");
    }
    if (cause == srsran::cause_nas_t::unspecified) {
      return format_to(ctx.out(), "unspecified");
    }

    return format_to(ctx.out(), "unknown");
  }
};

template <>
struct formatter<srsran::ngap_cause_misc_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::ngap_cause_misc_t& cause, FormatContext& ctx) const
  {
    if (cause == srsran::ngap_cause_misc_t::ctrl_processing_overload) {
      return format_to(ctx.out(), "ctrl_processing_overload");
    }
    if (cause == srsran::ngap_cause_misc_t::not_enough_user_plane_processing_res) {
      return format_to(ctx.out(), "not_enough_user_plane_processing_res");
    }
    if (cause == srsran::ngap_cause_misc_t::hardware_fail) {
      return format_to(ctx.out(), "hardware_fail");
    }
    if (cause == srsran::ngap_cause_misc_t::om_intervention) {
      return format_to(ctx.out(), "om_intervention");
    }
    if (cause == srsran::ngap_cause_misc_t::unknown_plmn_or_sn_pn) {
      return format_to(ctx.out(), "unknown_plmn_or_sn_pn");
    }
    if (cause == srsran::ngap_cause_misc_t::unspecified) {
      return format_to(ctx.out(), "unspecified");
    }

    return format_to(ctx.out(), "unknown");
  }
};

template <>
struct formatter<srsran::ngap_cause_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::ngap_cause_t o, FormatContext& ctx) const
  {
    if (const auto* result = std::get_if<srsran::ngap_cause_radio_network_t>(&o)) {
      return format_to(ctx.out(), "radio_network-{}", *result);
    }
    if (const auto* result = std::get_if<srsran::ngap_cause_transport_t>(&o)) {
      return format_to(ctx.out(), "transport-{}", *result);
    }
    if (const auto* result = std::get_if<srsran::cause_nas_t>(&o)) {
      return format_to(ctx.out(), "nas-{}", *result);
    }
    if (const auto* result = std::get_if<srsran::cause_protocol_t>(&o)) {
      return format_to(ctx.out(), "protocol-{}", *result);
    }
    return format_to(ctx.out(), "misc-{}", std::get<srsran::ngap_cause_misc_t>(o));
  }
};

} // namespace fmt
