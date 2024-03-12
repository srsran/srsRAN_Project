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

enum class e1ap_cause_radio_network_t : uint8_t {
  unspecified = 0,
  unknown_or_already_allocated_gnb_cu_cp_ue_e1ap_id,
  unknown_or_already_allocated_gnb_cu_up_ue_e1ap_id,
  unknown_or_inconsistent_pair_of_ue_e1ap_id,
  interaction_with_other_proc,
  ppdcp_count_wrap_around,
  not_supported_qci_value,
  not_supported_5qi_value,
  encryption_algorithms_not_supported,
  integrity_protection_algorithms_not_supported,
  up_integrity_protection_not_possible,
  up_confidentiality_protection_not_possible,
  multiple_pdu_session_id_instances,
  unknown_pdu_session_id,
  multiple_qos_flow_id_instances,
  unknown_qos_flow_id,
  multiple_drb_id_instances,
  unknown_drb_id,
  invalid_qos_combination,
  proc_cancelled,
  normal_release,
  no_radio_res_available,
  action_desirable_for_radio_reasons,
  res_not_available_for_the_slice,
  pdcp_cfg_not_supported,
  ue_dl_max_ip_data_rate_reason,
  up_integrity_protection_fail,
  release_due_to_pre_emption,
  rsn_not_available_for_the_up,
  npn_not_supported,
  report_characteristic_empty,
  existing_meas_id,
  meas_temporarily_not_available,
  meas_not_supported_for_the_obj
};

enum class e1ap_cause_transport_t : uint8_t { unspecified = 0, transport_res_unavailable, unknown_tnl_address_for_iab };

using e1ap_cause_t = variant<e1ap_cause_radio_network_t, e1ap_cause_transport_t, cause_protocol_t, cause_misc_t>;

} // namespace srsran

namespace fmt {

// e1ap_cause_t formatter
template <>
struct formatter<srsran::e1ap_cause_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::e1ap_cause_t o, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    if (srsran::variant_holds_alternative<srsran::e1ap_cause_radio_network_t>(o)) {
      return format_to(ctx.out(), "radio_network-id{}", srsran::variant_get<srsran::e1ap_cause_radio_network_t>(o));
    } else if (srsran::variant_holds_alternative<srsran::e1ap_cause_transport_t>(o)) {
      return format_to(ctx.out(), "transport-id{}", srsran::variant_get<srsran::e1ap_cause_transport_t>(o));
    } else if (srsran::variant_holds_alternative<srsran::cause_protocol_t>(o)) {
      return format_to(ctx.out(), "protocol-id{}", srsran::variant_get<srsran::cause_protocol_t>(o));
    } else {
      return format_to(ctx.out(), "misc-id{}", srsran::variant_get<srsran::cause_misc_t>(o));
    }
  }
};

} // namespace fmt
