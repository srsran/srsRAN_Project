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

#include "ngap_validators.h"
#include <unordered_set>

using namespace srsran;
using namespace srs_cu_cp;

pdu_session_resource_setup_validation_outcome srsran::srs_cu_cp::verify_pdu_session_resource_setup_request(
    const cu_cp_pdu_session_resource_setup_request&    request,
    const asn1::ngap::pdu_session_res_setup_request_s& asn1_request,
    const ngap_ue_logger&                              ue_logger)
{
  pdu_session_resource_setup_validation_outcome verification_outcome;

  std::unordered_set<pdu_session_id_t> psis;
  std::unordered_set<pdu_session_id_t> failed_psis;
  for (const auto& pdu_session_item : asn1_request->pdu_session_res_setup_list_su_req) {
    pdu_session_id_t psi = uint_to_pdu_session_id(pdu_session_item.pdu_session_id);
    // Check for duplicate PDU Session IDs
    if (!psis.emplace(psi).second) {
      ue_logger.log_warning("Duplicate {} in PduSessionResourceSetupRequest", psi);
      // Make sure to only add each duplicate psi once
      if (failed_psis.emplace(psi).second) {
        // Add failed psi to response
        cu_cp_pdu_session_res_setup_failed_item failed_item;
        failed_item.pdu_session_id              = psi;
        failed_item.unsuccessful_transfer.cause = ngap_cause_radio_network_t::multiple_pdu_session_id_instances;
        verification_outcome.response.pdu_session_res_failed_to_setup_items.emplace(psi, failed_item);
      }
    }
  }

  // Remove failed psis from psis
  for (const auto& failed_psi : failed_psis) {
    psis.erase(failed_psi);
  }

  // If only duplicate PDU session IDs are present, return
  if (psis.empty()) {
    return verification_outcome;
  }

  // If Non-GBR QoS flow present then PDU Session Aggregate Maximum Bit Rate must be present
  for (auto& psi : psis) {
    for (const auto& qos_flow_item : request.pdu_session_res_setup_items[psi].qos_flow_setup_request_items) {
      if (qos_flow_item.qos_flow_level_qos_params.reflective_qos_attribute.has_value() ||
          qos_flow_item.qos_flow_level_qos_params.add_qos_flow_info.has_value()) {
        if (!asn1_request->ue_aggr_max_bit_rate_present) {
          ue_logger.log_warning("Non-GBR QoS flow for {} present but PduSessionAggregateMaximumBitRate not set", psi);
          failed_psis.emplace(psi);
          // Add failed psi to response
          cu_cp_pdu_session_res_setup_failed_item failed_item;
          failed_item.pdu_session_id              = psi;
          failed_item.unsuccessful_transfer.cause = ngap_cause_radio_network_t::invalid_qos_combination;
          verification_outcome.response.pdu_session_res_failed_to_setup_items.emplace(psi, failed_item);
          // If single QoS flow fails, then the whole PDU session fails
          break;
        }
      }
    }
  }

  // Remove failed psis from psis
  for (const auto& failed_psi : failed_psis) {
    psis.erase(failed_psi);
  }

  // Add remaining PDU sessions to verified request
  for (const auto& psi : psis) {
    verification_outcome.request.pdu_session_res_setup_items.emplace(psi, request.pdu_session_res_setup_items[psi]);
  }
  verification_outcome.request.ue_index                         = request.ue_index;
  verification_outcome.request.ue_aggregate_maximum_bit_rate_dl = request.ue_aggregate_maximum_bit_rate_dl;
  verification_outcome.request.serving_plmn                     = request.serving_plmn;

  return verification_outcome;
}

pdu_session_resource_modify_validation_outcome srsran::srs_cu_cp::verify_pdu_session_resource_modify_request(
    const cu_cp_pdu_session_resource_modify_request&    request,
    const asn1::ngap::pdu_session_res_modify_request_s& asn1_request,
    const ngap_ue_logger&                               ue_logger)
{
  pdu_session_resource_modify_validation_outcome verification_outcome;

  std::unordered_set<pdu_session_id_t> psis;
  std::unordered_set<pdu_session_id_t> failed_psis;
  for (const auto& pdu_session_item : asn1_request->pdu_session_res_modify_list_mod_req) {
    pdu_session_id_t psi = uint_to_pdu_session_id(pdu_session_item.pdu_session_id);
    // Check for duplicate PDU Session IDs
    if (!psis.emplace(psi).second) {
      ue_logger.log_warning("Duplicate {} in PduSessionResourceModifyRequest", psi);
      // Make sure to only add each duplicate psi once
      if (failed_psis.emplace(psi).second) {
        // Add failed psi to response
        cu_cp_pdu_session_res_setup_failed_item failed_item;
        failed_item.pdu_session_id              = psi;
        failed_item.unsuccessful_transfer.cause = ngap_cause_radio_network_t::multiple_pdu_session_id_instances;
        verification_outcome.response.pdu_session_res_failed_to_modify_list.emplace(psi, failed_item);
      }
    }
  }

  // Remove failed psis from psis
  for (const auto& failed_psi : failed_psis) {
    psis.erase(failed_psi);
  }

  // Add remaining PDU sessions to verified request
  for (const auto& psi : psis) {
    verification_outcome.request.pdu_session_res_modify_items.emplace(psi, request.pdu_session_res_modify_items[psi]);
  }
  verification_outcome.request.ue_index = request.ue_index;

  return verification_outcome;
}
