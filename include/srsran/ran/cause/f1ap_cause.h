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

enum class f1ap_cause_radio_network_t : uint8_t {
  unspecified = 0,
  rl_fail_rlc,
  unknown_or_already_allocated_gnb_cu_ue_f1ap_id,
  unknown_or_already_allocated_gnb_du_ue_f1ap_id,
  unknown_or_inconsistent_pair_of_ue_f1ap_id,
  interaction_with_other_proc,
  not_supported_qci_value,
  action_desirable_for_radio_reasons,
  no_radio_res_available,
  proc_cancelled,
  normal_release,
  cell_not_available,
  rl_fail_others,
  ue_rejection,
  res_not_available_for_the_slice,
  amf_initiated_abnormal_release,
  release_due_to_pre_emption,
  plmn_not_served_by_the_gnb_cu,
  multiple_drb_id_instances,
  unknown_drb_id,
  multiple_bh_rlc_ch_id_instances,
  unknown_bh_rlc_ch_id,
  cho_cpc_res_tobechanged,
  npn_not_supported,
  npn_access_denied,
  gnb_cu_cell_capacity_exceeded,
  report_characteristics_empty,
  existing_meas_id,
  meas_temporarily_not_available,
  meas_not_supported_for_the_obj,
  unknown_bh_address,
  unknown_bap_routing_id,
  insufficient_ue_cap,
  scg_activation_deactivation_fail,
  scg_deactivation_fail_due_to_data_tx,
  requested_item_not_supported_on_time,
  unknown_or_already_allocated_gnb_cu_mbs_f1ap_id,
  unknown_or_already_allocated_gnb_du_mbs_f1ap_id,
  unknown_or_inconsistent_pair_of_mbs_f1ap_id,
  unknown_or_inconsistent_mrb_id,
  tat_sdt_expiry
};

enum class f1ap_cause_transport_t : uint8_t {
  unspecified = 0,
  transport_res_unavailable,
  unknown_tnl_address_for_iab,
  unknown_up_tnl_info_for_iab
};

using f1ap_cause_t = variant<f1ap_cause_radio_network_t, f1ap_cause_transport_t, cause_protocol_t, cause_misc_t>;

} // namespace srsran

namespace fmt {

// f1ap_cause_t formatter
template <>
struct formatter<srsran::f1ap_cause_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::f1ap_cause_t o, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    if (srsran::variant_holds_alternative<srsran::f1ap_cause_radio_network_t>(o)) {
      return format_to(ctx.out(), "radio_network-id{}", srsran::variant_get<srsran::f1ap_cause_radio_network_t>(o));
    } else if (srsran::variant_holds_alternative<srsran::f1ap_cause_transport_t>(o)) {
      return format_to(ctx.out(), "transport-id{}", srsran::variant_get<srsran::f1ap_cause_transport_t>(o));
    } else if (srsran::variant_holds_alternative<srsran::cause_protocol_t>(o)) {
      return format_to(ctx.out(), "protocol-id{}", srsran::variant_get<srsran::cause_protocol_t>(o));
    } else {
      return format_to(ctx.out(), "misc-id{}", srsran::variant_get<srsran::cause_misc_t>(o));
    }
  }
};

} // namespace fmt
