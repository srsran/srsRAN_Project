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

#include "srsran/ran/cause/e1ap_cause_converters.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

const uint8_t e1ap_to_ngap_cause_radio_network[] = {
    (uint8_t)ngap_cause_radio_network_t::unspecified, // unspecified
    (uint8_t)ngap_cause_radio_network_t::unspecified, // unknown_or_already_allocated_gnb_cu_cp_ue_e1ap_id
    (uint8_t)ngap_cause_radio_network_t::unspecified, // unknown_or_already_allocated_gnb_cu_up_ue_e1ap_id
    (uint8_t)ngap_cause_radio_network_t::unspecified, // unknown_or_inconsistent_pair_of_ue_e1ap_id
    (uint8_t)ngap_cause_radio_network_t::interaction_with_other_proc, // interaction_with_other_proc
    (uint8_t)ngap_cause_radio_network_t::unspecified,                 // ppdcp_count_wrap_around
    (uint8_t)ngap_cause_radio_network_t::unspecified,                 // not_supported_qci_value
    (uint8_t)ngap_cause_radio_network_t::not_supported_5qi_value,     // not_supported_5qi_value
    (uint8_t)ngap_cause_radio_network_t::
        encryption_and_or_integrity_protection_algorithms_not_supported, // encryption_algorithms_not_supported
    (uint8_t)ngap_cause_radio_network_t::
        encryption_and_or_integrity_protection_algorithms_not_supported, // integrity_protection_algorithms_not_supported
    (uint8_t)ngap_cause_radio_network_t::up_integrity_protection_not_possible, // up_integrity_protection_not_possible
    (uint8_t)ngap_cause_radio_network_t::
        up_confidentiality_protection_not_possible, // up_confidentiality_protection_not_possible
    (uint8_t)ngap_cause_radio_network_t::multiple_pdu_session_id_instances, // multiple_pdu_session_id_instances
    (uint8_t)ngap_cause_radio_network_t::unknown_pdu_session_id,            // unknown_pdu_session_id
    (uint8_t)ngap_cause_radio_network_t::multiple_qos_flow_id_instances,    // multiple_qos_flow_id_instances
    (uint8_t)ngap_cause_radio_network_t::unspecified,                       // unknown_qos_flow_id
    (uint8_t)ngap_cause_radio_network_t::unspecified,                       // multiple_drb_id_instances
    (uint8_t)ngap_cause_radio_network_t::unspecified,                       // unknown_drb_id
    (uint8_t)ngap_cause_radio_network_t::invalid_qos_combination,           // invalid_qos_combination
    (uint8_t)ngap_cause_radio_network_t::unspecified,                       // proc_cancelled
    (uint8_t)ngap_cause_radio_network_t::unspecified,                       // normal_release
    (uint8_t)ngap_cause_radio_network_t::unspecified,                       // no_radio_res_available
    (uint8_t)ngap_cause_radio_network_t::unspecified,                       // action_desirable_for_radio_reasons
    (uint8_t)ngap_cause_radio_network_t::res_not_available_for_the_slice,   // res_not_available_for_the_slice
    (uint8_t)ngap_cause_radio_network_t::unspecified,                       // pdcp_cfg_not_supported
    (uint8_t)ngap_cause_radio_network_t::unspecified,                       // ue_dl_max_ip_data_rate_reason
    (uint8_t)ngap_cause_radio_network_t::unspecified,                       // up_integrity_protection_fail
    (uint8_t)ngap_cause_radio_network_t::unspecified,                       // release_due_to_pre_emption
    (uint8_t)ngap_cause_radio_network_t::unspecified,                       // rsn_not_available_for_the_up
    (uint8_t)ngap_cause_radio_network_t::unspecified,                       // npn_not_supported
    (uint8_t)ngap_cause_radio_network_t::unspecified,                       // report_characteristic_empty
    (uint8_t)ngap_cause_radio_network_t::unspecified,                       // existing_meas_id
    (uint8_t)ngap_cause_radio_network_t::unspecified,                       // meas_temporarily_not_available
    (uint8_t)ngap_cause_radio_network_t::unspecified                        // meas_not_supported_for_the_obj
};

const uint8_t e1ap_to_ngap_cause_transport[] = {
    (uint8_t)ngap_cause_transport_t::transport_res_unavailable, // transport_res_unavailable
    (uint8_t)ngap_cause_transport_t::unspecified,               // unspecified
    (uint8_t)ngap_cause_transport_t::unspecified                // unknown_tnl_address_for_iab
};

const uint8_t e1ap_to_ngap_cause_misc[] = {
    (uint8_t)ngap_cause_misc_t::ctrl_processing_overload,             // ctrl_processing_overload
    (uint8_t)ngap_cause_misc_t::not_enough_user_plane_processing_res, // not_enough_user_plane_processing_res
    (uint8_t)ngap_cause_misc_t::hardware_fail,                        // hardware_fail
    (uint8_t)ngap_cause_misc_t::om_intervention,                      // om_intervention
    (uint8_t)ngap_cause_misc_t::unspecified,                          // unspecified
};

ngap_cause_t srsran::e1ap_to_ngap_cause(e1ap_cause_t e1ap_cause)
{
  ngap_cause_t ngap_cause;

  if (variant_holds_alternative<e1ap_cause_radio_network_t>(e1ap_cause)) {
    ngap_cause = ngap_cause_radio_network_t(
        e1ap_to_ngap_cause_radio_network[uint8_t(variant_get<e1ap_cause_radio_network_t>(e1ap_cause))]);
  } else if (variant_holds_alternative<e1ap_cause_transport_t>(e1ap_cause)) {
    ngap_cause =
        ngap_cause_transport_t(e1ap_to_ngap_cause_transport[uint8_t(variant_get<e1ap_cause_transport_t>(e1ap_cause))]);
  } else if (variant_holds_alternative<cause_protocol_t>(e1ap_cause)) {
    ngap_cause = variant_get<cause_protocol_t>(e1ap_cause);
  } else if (variant_holds_alternative<cause_misc_t>(e1ap_cause)) {
    ngap_cause = ngap_cause_misc_t(e1ap_to_ngap_cause_misc[uint8_t(variant_get<cause_misc_t>(e1ap_cause))]);
  } else {
    report_fatal_error("Cannot convert cause to NGAP type: {}", e1ap_cause);
  }

  return ngap_cause;
};
