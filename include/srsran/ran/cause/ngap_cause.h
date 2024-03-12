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

#include "common.h"
#include "srsran/adt/variant.h"
#include "fmt/format.h"

namespace srsran {

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

enum class ngap_cause_transport_t : uint8_t {
  transport_res_unavailable = 0,
  unspecified,
};

enum class cause_nas_t : uint8_t { normal_release = 0, authentication_fail, deregister, unspecified }; // only NGAP

enum class ngap_cause_misc_t : uint8_t {
  ctrl_processing_overload = 0,
  not_enough_user_plane_processing_res,
  hardware_fail,
  om_intervention,
  unknown_plmn_or_sn_pn,
  unspecified
};

using ngap_cause_t =
    variant<ngap_cause_radio_network_t, ngap_cause_transport_t, cause_nas_t, cause_protocol_t, ngap_cause_misc_t>;

} // namespace srsran

namespace fmt {

// ngap_cause_t formatter
template <>
struct formatter<srsran::ngap_cause_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::ngap_cause_t o, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    if (srsran::variant_holds_alternative<srsran::ngap_cause_radio_network_t>(o)) {
      return format_to(ctx.out(), "radio_network-id{}", srsran::variant_get<srsran::ngap_cause_radio_network_t>(o));
    } else if (srsran::variant_holds_alternative<srsran::ngap_cause_transport_t>(o)) {
      return format_to(ctx.out(), "transport-id{}", srsran::variant_get<srsran::ngap_cause_transport_t>(o));
    } else if (srsran::variant_holds_alternative<srsran::cause_nas_t>(o)) {
      return format_to(ctx.out(), "nas-id{}", srsran::variant_get<srsran::cause_nas_t>(o));
    } else if (srsran::variant_holds_alternative<srsran::cause_protocol_t>(o)) {
      return format_to(ctx.out(), "protocol-id{}", srsran::variant_get<srsran::cause_protocol_t>(o));
    } else {
      return format_to(ctx.out(), "misc-id{}", srsran::variant_get<srsran::ngap_cause_misc_t>(o));
    }
  }
};

} // namespace fmt
