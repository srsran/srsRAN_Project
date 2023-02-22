/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "ngap_asn1_converters.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/optional.h"
#include "srsran/ran/bcd_helpers.h"
#include "srsran/security/security.h"
#include <string>
#include <vector>

namespace srsran {

namespace srs_cu_cp {

// Helper to create PDU from NGAP message
template <class T>
byte_buffer pack_into_pdu(const T& msg, const char* context_name = nullptr)
{
  context_name = context_name == nullptr ? __FUNCTION__ : context_name;
  byte_buffer   pdu{};
  asn1::bit_ref bref{pdu};
  if (msg.pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
    srslog::fetch_basic_logger("NGAP").error("Failed to pack message in {} - discarding it", context_name);
  }
  return pdu;
}

// TODO: pass params using struct
/// \brief Fills ASN.1 NGSetupRequest struct.
/// \param[out] request The NGSetupRequest ASN.1 struct to fill.
/// \param[in] global_gnb_id The global RAN node id
/// \param[in] ran_node_name The RAN node name
/// \param[in] plmn_id The BCD-encoded PLMN
/// \param[in] tac The TAC
inline void fill_asn1_ng_setup_request(asn1::ngap::ng_setup_request_s& request,
                                       int                             global_gnb_id,
                                       std::string                     ran_node_name,
                                       std::string                     plmn_id,
                                       int                             tac)
{
  // convert PLMN to BCD
  uint32_t plmn_bcd = plmn_string_to_bcd(plmn_id);

  // fill global ran node id
  request->global_ran_node_id.value.set_global_gnb_id();
  request->global_ran_node_id.value.global_gnb_id().gnb_id.set_gnb_id();
  request->global_ran_node_id.value.global_gnb_id().gnb_id.gnb_id().from_number(global_gnb_id);
  request->global_ran_node_id.value.global_gnb_id().plmn_id.from_number(plmn_bcd);

  // fill ran node name
  request->ran_node_name_present = true;
  request->ran_node_name.value.from_string(ran_node_name);

  // fill supported ta list
  // TODO: add support for more items
  request->supported_ta_list.id   = ASN1_NGAP_ID_SUPPORTED_TA_LIST;
  request->supported_ta_list.crit = asn1::crit_opts::reject;

  asn1::ngap::supported_ta_item_s supported_ta_item = {};

  asn1::ngap::broadcast_plmn_item_s broadcast_plmn_item = {};
  broadcast_plmn_item.plmn_id.from_number(plmn_bcd);

  asn1::ngap::slice_support_item_s slice_support_item = {};
  slice_support_item.s_nssai.sst.from_number(1);
  broadcast_plmn_item.tai_slice_support_list.push_back(slice_support_item);

  supported_ta_item.broadcast_plmn_list.push_back(broadcast_plmn_item);
  supported_ta_item.tac.from_number(tac);

  request->supported_ta_list.value.push_back(supported_ta_item);

  // fill paging drx
  request->default_paging_drx.value.value = asn1::ngap::paging_drx_opts::v256;
}

/// \brief Convert NGAP ASN1 PDU Session Resource Setup List SU REQ ASN1 struct to common type.
/// \param[out] cu_cp_pdu_session_res_setup_msg The cu_cp_pdu_session_res_setup_msg struct to fill.
/// \param[in] asn1_pdu_session_res_setup_msg The pdu_session_res_setup_list_su_req ASN1 struct.
inline void fill_cu_cp_pdu_session_resource_setup_request(
    cu_cp_pdu_session_resource_setup_request& cu_cp_pdu_session_resource_setup_msg,
    const asn1::dyn_seq_of<asn1::ngap::pdu_session_res_setup_item_su_req_s, 1U, 256U, true>&
        asn1_pdu_session_res_setup_msg)
{
  for (const auto& asn1_session_item : asn1_pdu_session_res_setup_msg) {
    cu_cp_pdu_session_res_setup_item setup_item;

    // pDUSessionID
    setup_item.pdu_session_id = uint_to_pdu_session_id(asn1_session_item.pdu_session_id);

    // pDUSessionNAS-PDU
    if (!asn1_session_item.pdu_session_nas_pdu.empty()) {
      setup_item.pdu_session_nas_pdu.resize(asn1_session_item.pdu_session_nas_pdu.size());
      std::copy(asn1_session_item.pdu_session_nas_pdu.begin(),
                asn1_session_item.pdu_session_nas_pdu.end(),
                setup_item.pdu_session_nas_pdu.begin());
    }

    // s-NSSAI
    if (asn1_session_item.s_nssai.sd_present) {
      setup_item.s_nssai.sd = asn1_session_item.s_nssai.sd.to_number();
    }
    setup_item.s_nssai.sst = asn1_session_item.s_nssai.sst.to_number();

    // pDUSessionResourceSetupRequestTransfer
    asn1::ngap::pdu_session_res_setup_request_transfer_s asn1_setup_req_transfer;
    asn1::cbit_ref bref({asn1_session_item.pdu_session_res_setup_request_transfer.begin(),
                         asn1_session_item.pdu_session_res_setup_request_transfer.end()});

    if (asn1_setup_req_transfer.unpack(bref) != asn1::SRSASN_SUCCESS) {
      //   logger.error("Couldn't unpack PDU Session Resource Setup Request Transfer PDU");
      return;
    }

    // id-PDUSessionAggregateMaximumBitRate
    setup_item.pdu_session_aggregate_maximum_bit_rate_dl =
        asn1_setup_req_transfer->pdu_session_aggr_max_bit_rate.value.pdu_session_aggr_max_bit_rate_dl;
    setup_item.pdu_session_aggregate_maximum_bit_rate_ul =
        asn1_setup_req_transfer->pdu_session_aggr_max_bit_rate.value.pdu_session_aggr_max_bit_rate_ul;

    // id-UL-NGU-UP-TNLInformation
    setup_item.ul_ngu_up_tnl_info = asn1_to_up_transport_layer_info(asn1_setup_req_transfer->ul_ngu_up_tnl_info.value);

    // id-PDUSessionType
    setup_item.pdu_session_type = asn1_setup_req_transfer->pdu_session_type.value.to_string();

    // id-QosFlowSetupRequestList
    for (const auto& asn1_flow_item : asn1_setup_req_transfer->qos_flow_setup_request_list.value) {
      qos_flow_setup_request_item qos_flow_setup_req_item;

      // qosFlowIdentifier
      qos_flow_setup_req_item.qos_flow_id = uint_to_qos_flow_id(asn1_flow_item.qos_flow_id);

      // qosFlowLevelQosParameters
      if (asn1_flow_item.qos_flow_level_qos_params.qos_characteristics.type() ==
          asn1::ngap::qos_characteristics_c::types::dyn5qi) {
        qos_flow_setup_req_item.qos_characteristics.is_dynamic_5qi = true;
        qos_flow_setup_req_item.qos_characteristics.five_qi =
            asn1_flow_item.qos_flow_level_qos_params.qos_characteristics.dyn5qi().five_qi;

        // TODO: ADD optional values

      } else if (asn1_flow_item.qos_flow_level_qos_params.qos_characteristics.type() ==
                 asn1::ngap::qos_characteristics_c::types::non_dyn5qi) {
        qos_flow_setup_req_item.qos_characteristics.is_dynamic_5qi = false;
        qos_flow_setup_req_item.qos_characteristics.five_qi =
            asn1_flow_item.qos_flow_level_qos_params.qos_characteristics.non_dyn5qi().five_qi;

        // TODO: ADD optional values
      }

      // allocationAndRetentionPriority
      qos_flow_setup_req_item.qos_characteristics.prio_level_arp =
          asn1_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.prio_level_arp;
      qos_flow_setup_req_item.qos_characteristics.pre_emption_cap =
          asn1_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_cap.to_string();
      qos_flow_setup_req_item.qos_characteristics.pre_emption_vulnerability =
          asn1_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_vulnerability.to_string();

      // Optional Parameters
      if (asn1_flow_item.qos_flow_level_qos_params.add_qos_flow_info_present) {
        qos_flow_setup_req_item.add_qos_flow_info =
            asn1_flow_item.qos_flow_level_qos_params.add_qos_flow_info.to_string();
      }

      if (asn1_flow_item.qos_flow_level_qos_params.gbr_qos_info_present) {
        // TODO: Add to common type
      }

      if (asn1_flow_item.qos_flow_level_qos_params.reflective_qos_attribute_present) {
        qos_flow_setup_req_item.reflective_qos_attribute =
            asn1_flow_item.qos_flow_level_qos_params.reflective_qos_attribute.to_string();
      }

      if (asn1_flow_item.erab_id_present) {
        qos_flow_setup_req_item.erab_id = asn1_flow_item.erab_id;
      }

      setup_item.qos_flow_setup_request_items.emplace(qos_flow_setup_req_item.qos_flow_id, qos_flow_setup_req_item);
    }

    cu_cp_pdu_session_resource_setup_msg.pdu_session_res_setup_items.emplace(setup_item.pdu_session_id,
                                                                             std::move(setup_item));
  }
}

/// \brief Convert common type PDU Session Resource Setup Response message to NGAP PDU Session Resource Setup Response
/// message.
/// \param[out] resp The ASN1 NGAP PDU Session Resource Setup Response message.
/// \param[in] cu_cp_resp The CU-CP PDU Session Resource Setup Response message.
inline void fill_asn1_pdu_session_res_setup_response(asn1::ngap::pdu_session_res_setup_resp_s&       resp,
                                                     const cu_cp_pdu_session_resource_setup_response cu_cp_resp)
{
  // Fill PDU Session Resource Setup Response List
  if (!cu_cp_resp.pdu_session_res_setup_response_items.empty()) {
    resp->pdu_session_res_setup_list_su_res_present = true;

    for (const auto& cu_cp_resp_item : cu_cp_resp.pdu_session_res_setup_response_items) {
      asn1::ngap::pdu_session_res_setup_item_su_res_s resp_item;

      resp_item.pdu_session_id = pdu_session_id_to_uint(cu_cp_resp_item.pdu_session_id);

      asn1::ngap::pdu_session_res_setup_resp_transfer_s response_transfer;

      // Add dLQosFlowPerTNLInformation
      response_transfer.dl_qos_flow_per_tnl_info = cu_cp_qos_flow_per_tnl_info_to_ngap_qos_flow_per_tnl_info(
          cu_cp_resp_item.pdu_session_resource_setup_response_transfer.dlqos_flow_per_tnl_info);

      // Add AdditionalDLQosFlowPerTNLInformation
      for (const auto& cu_cp_qos_flow_info :
           cu_cp_resp_item.pdu_session_resource_setup_response_transfer.add_dl_qos_flow_per_tnl_info) {
        asn1::ngap::qos_flow_per_tnl_info_item_s ngap_qos_flow_info;
        ngap_qos_flow_info.qos_flow_per_tnl_info =
            cu_cp_qos_flow_per_tnl_info_to_ngap_qos_flow_per_tnl_info(cu_cp_qos_flow_info);
        response_transfer.add_dl_qos_flow_per_tnl_info.push_back(ngap_qos_flow_info);
      }

      // Add QosFlowFailedToSetupList
      for (const auto& cu_cp_failed_item :
           cu_cp_resp_item.pdu_session_resource_setup_response_transfer.qos_flow_failed_to_setup_list) {
        asn1::ngap::qos_flow_with_cause_item_s ngap_failed_item =
            cu_cp_qos_flow_failed_to_setup_item_to_ngap_qos_flow_with_cause_item(cu_cp_failed_item);
        response_transfer.qos_flow_failed_to_setup_list.push_back(ngap_failed_item);
      }

      // Add SecurityResult
      if (cu_cp_resp_item.pdu_session_resource_setup_response_transfer.security_result.has_value()) {
        response_transfer.security_result = cu_cp_security_result_to_ngap_security_result(
            cu_cp_resp_item.pdu_session_resource_setup_response_transfer.security_result.value());
      }

      // Pack pdu_session_res_setup_resp_transfer_s
      byte_buffer pdu = pack_into_pdu(response_transfer);

      resp_item.pdu_session_res_setup_resp_transfer.resize(pdu.length());
      std::copy(pdu.begin(), pdu.end(), resp_item.pdu_session_res_setup_resp_transfer.begin());

      resp->pdu_session_res_setup_list_su_res->push_back(resp_item);
    }
  }

  // Fill PDU Session Resource Failed to Setup List
  if (!cu_cp_resp.pdu_session_res_failed_to_setup_items.empty()) {
    resp->pdu_session_res_failed_to_setup_list_su_res_present = true;
    for (const auto& cu_cp_setup_failed_item : cu_cp_resp.pdu_session_res_failed_to_setup_items) {
      asn1::ngap::pdu_session_res_failed_to_setup_item_su_res_s setup_failed_item;

      setup_failed_item.pdu_session_id = pdu_session_id_to_uint(cu_cp_setup_failed_item.pdu_session_id);

      asn1::ngap::pdu_session_res_setup_unsuccessful_transfer_s setup_unsuccessful_transfer;
      setup_unsuccessful_transfer.cause =
          cause_to_ngap_cause(cu_cp_setup_failed_item.pdu_session_resource_setup_unsuccessful_transfer.cause);
      // TODO: Add crit diagnostics

      // Pack pdu_session_res_setup_unsuccessful_transfer_s
      byte_buffer pdu = pack_into_pdu(setup_unsuccessful_transfer);

      setup_failed_item.pdu_session_res_setup_unsuccessful_transfer.resize(pdu.length());
      std::copy(pdu.begin(), pdu.end(), setup_failed_item.pdu_session_res_setup_unsuccessful_transfer.begin());

      resp->pdu_session_res_failed_to_setup_list_su_res->push_back(setup_failed_item);
    }
  }
}

/// \brief Convert NGAP ASN1 UE Context Release Command ASN1 struct to common type.
/// \param[out] cu_cp_ue_context_release_cmd The cu_cp_ue_context_release_cmd struct to fill.
/// \param[in] asn1_ue_context_release_cmd The UE Context Release Command ASN1 struct.
inline void
fill_cu_cp_ue_context_release_command(cu_cp_ue_context_release_command&           cu_cp_ue_context_release_cmd,
                                      const asn1::ngap::ue_context_release_cmd_s& asn1_ue_context_release_cmd)
{
  cu_cp_ue_context_release_cmd.cause = ngap_cause_to_cause(asn1_ue_context_release_cmd->cause.value);
}

/// \brief Convert common type UE Context Release Complete message to NGAP ASN1 UE Context Release Complete
/// message.
/// \param[out] asn1_resp The ASN1 NGAP UE Context Release Complete message.
/// \param[in] cu_cp_resp The CU-CP UE Context Release Complete message.
inline void fill_asn1_ue_context_release_complete(asn1::ngap::ue_context_release_complete_s& asn1_resp,
                                                  const cu_cp_ue_context_release_complete    cu_cp_resp)
{
  // add user location info
  if (cu_cp_resp.user_location_info.has_value()) {
    asn1_resp->user_location_info_present = true;
    auto& asn1_user_location_info         = asn1_resp->user_location_info.value.set_user_location_info_nr();
    // add nr cgi
    asn1_user_location_info.nr_cgi.nr_cell_id.from_number(cu_cp_resp.user_location_info.value().nr_cgi.nci.packed);
    asn1_user_location_info.nr_cgi.plmn_id.from_string(cu_cp_resp.user_location_info.value().nr_cgi.plmn_hex);
    // add tai
    asn1_user_location_info.tai.plmn_id.from_string(cu_cp_resp.user_location_info.value().tai.plmn_id);
    asn1_user_location_info.tai.tac.from_number(cu_cp_resp.user_location_info.value().tai.tac);
    // add timestamp
    if (cu_cp_resp.user_location_info.value().time_stamp.has_value()) {
      asn1_user_location_info.time_stamp_present = true;
      asn1_user_location_info.time_stamp.from_number(cu_cp_resp.user_location_info.value().time_stamp.value());
    }
  }

  // add info on recommended cells and ran nodes for paging
  if (cu_cp_resp.info_on_recommended_cells_and_ran_nodes_for_paging.has_value()) {
    asn1_resp->info_on_recommended_cells_and_ran_nodes_for_paging_present = true;

    for (auto cu_cp_recommended_cell_item : cu_cp_resp.info_on_recommended_cells_and_ran_nodes_for_paging.value()
                                                .recommended_cells_for_paging.recommended_cell_list) {
      asn1::ngap::recommended_cell_item_s asn1_recommended_cell_item;

      // add ngran cgi
      asn1_recommended_cell_item.ngran_cgi.set_nr_cgi().nr_cell_id.from_number(
          cu_cp_recommended_cell_item.ngran_cgi.nci.packed);
      asn1_recommended_cell_item.ngran_cgi.set_nr_cgi().plmn_id.from_string(
          cu_cp_recommended_cell_item.ngran_cgi.plmn_hex);

      // add time stayed in cell
      if (cu_cp_recommended_cell_item.time_stayed_in_cell.has_value()) {
        asn1_recommended_cell_item.time_stayed_in_cell_present = true;
        asn1_recommended_cell_item.time_stayed_in_cell = cu_cp_recommended_cell_item.time_stayed_in_cell.value();
      }

      asn1_resp->info_on_recommended_cells_and_ran_nodes_for_paging.value.recommended_cells_for_paging
          .recommended_cell_list.push_back(asn1_recommended_cell_item);
    }

    for (auto cu_cp_recommended_ran_node_item : cu_cp_resp.info_on_recommended_cells_and_ran_nodes_for_paging.value()
                                                    .recommend_ran_nodes_for_paging.recommended_ran_node_list) {
      asn1::ngap::recommended_ran_node_item_s asn1_recommended_ran_node_item;

      // add amf paging target
      if (cu_cp_recommended_ran_node_item.amf_paging_target.is_global_ran_node_id) {
        // add global gnb id
        auto& asn1_global_ran_node_id = asn1_recommended_ran_node_item.amf_paging_target.set_global_ran_node_id();
        asn1_global_ran_node_id.set_global_gnb_id().plmn_id.from_string(
            cu_cp_recommended_ran_node_item.amf_paging_target.global_ran_node_id.value().plmn_id);
        asn1_global_ran_node_id.global_gnb_id().gnb_id.set_gnb_id().from_string(
            cu_cp_recommended_ran_node_item.amf_paging_target.global_ran_node_id.value().gnb_id);
      } else if (cu_cp_recommended_ran_node_item.amf_paging_target.is_tai) {
        // add tai
        auto& asn1_tai = asn1_recommended_ran_node_item.amf_paging_target.set_tai();
        asn1_tai.plmn_id.from_string(cu_cp_recommended_ran_node_item.amf_paging_target.tai.value().plmn_id);
        asn1_tai.tac.from_number(cu_cp_recommended_ran_node_item.amf_paging_target.tai.value().tac);
      } else {
        asn1_recommended_ran_node_item.amf_paging_target.set(asn1::ngap::amf_paging_target_c::types_opts::nulltype);
      }

      asn1_resp->info_on_recommended_cells_and_ran_nodes_for_paging.value.recommend_ran_nodes_for_paging
          .recommended_ran_node_list.push_back(asn1_recommended_ran_node_item);
    }
  }

  // add pdu session res list context release complete
  if (!cu_cp_resp.pdu_session_res_list_cxt_rel_cpl.empty()) {
    asn1_resp->pdu_session_res_list_cxt_rel_cpl_present = true;

    for (auto pdu_session_id : cu_cp_resp.pdu_session_res_list_cxt_rel_cpl) {
      asn1::ngap::pdu_session_res_item_cxt_rel_cpl_s asn1_rel_item;
      asn1_rel_item.pdu_session_id = pdu_session_id_to_uint(pdu_session_id);
      asn1_resp->pdu_session_res_list_cxt_rel_cpl.value.push_back(asn1_rel_item);
    }
  }

  // add crit diagnostics
  if (cu_cp_resp.crit_diagnostics.has_value()) {
    // TODO: Add crit diagnostics
  }
}

} // namespace srs_cu_cp
} // namespace srsran
