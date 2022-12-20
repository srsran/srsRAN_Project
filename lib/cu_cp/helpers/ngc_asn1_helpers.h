/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../converters/ngc_asn1_converters.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/bcd_helpers.h"
#include "srsgnb/security/security.h"
#include <string>
#include <vector>

namespace srsgnb {

namespace srs_cu_cp {

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
/// \param[in] pdu_session_res_setup_su_req The pdu_session_res_setup_list_su_req ASN1 struct.
inline void fill_cu_cp_pdu_session_resource_setup_message(
    cu_cp_pdu_session_resource_setup_message& cu_cp_pdu_session_resource_setup_msg,
    const asn1::dyn_seq_of<asn1::ngap::pdu_session_res_setup_item_su_req_s, 1U, 256U, true>&
        pdu_session_res_setup_su_req)
{
  for (auto ngap_session_item : pdu_session_res_setup_su_req) {
    cu_cp_pdu_session_res_setup_item setup_item;

    // pDUSessionID
    setup_item.pdu_session_id = ngap_session_item.pdu_session_id;

    // pDUSessionNAS-PDU
    if (!ngap_session_item.pdu_session_nas_pdu.empty()) {
      setup_item.pdu_session_nas_pdu.resize(ngap_session_item.pdu_session_nas_pdu.size());
      std::copy(ngap_session_item.pdu_session_nas_pdu.begin(),
                ngap_session_item.pdu_session_nas_pdu.end(),
                setup_item.pdu_session_nas_pdu.begin());
    }

    // s-NSSAI
    if (ngap_session_item.s_nssai.sd_present) {
      setup_item.s_nssai.sd = ngap_session_item.s_nssai.sd.to_number();
    }
    setup_item.s_nssai.sst = ngap_session_item.s_nssai.sst.to_number();

    // pDUSessionResourceSetupRequestTransfer
    asn1::ngap::pdu_session_res_setup_request_transfer_s setup_req_transfer;
    asn1::cbit_ref bref({ngap_session_item.pdu_session_res_setup_request_transfer.begin(),
                         ngap_session_item.pdu_session_res_setup_request_transfer.end()});

    if (setup_req_transfer.unpack(bref) != asn1::SRSASN_SUCCESS) {
      //   logger.error("Couldn't unpack PDU Session Resource Setup Request Transfer PDU");
      return;
    }

    // id-PDUSessionAggregateMaximumBitRate
    setup_item.pdu_session_aggregate_maximum_bit_rate_dl =
        setup_req_transfer->pdu_session_aggregate_maximum_bit_rate.value.pdu_session_aggregate_maximum_bit_rate_dl;
    setup_item.pdu_session_aggregate_maximum_bit_rate_ul =
        setup_req_transfer->pdu_session_aggregate_maximum_bit_rate.value.pdu_session_aggregate_maximum_bit_rate_ul;

    // id-UL-NGU-UP-TNLInformation
    setup_item.ul_ngu_up_tnl_info.transport_layer_address =
        setup_req_transfer->ul_ngu_up_tnl_info.value.gtp_tunnel().transport_layer_address.to_number();
    setup_item.ul_ngu_up_tnl_info.gtp_teid =
        setup_req_transfer->ul_ngu_up_tnl_info.value.gtp_tunnel().gtp_teid.to_number();

    // id-PDUSessionType
    setup_item.pdu_session_type = setup_req_transfer->pdu_session_type.value.to_string();

    // id-QosFlowSetupRequestList
    for (auto ngap_flow_item : setup_req_transfer->qos_flow_setup_request_list.value) {
      qos_flow_setup_request_item qos_flow_setup_req_item;

      // qosFlowIdentifier
      qos_flow_setup_req_item.qos_flow_id = ngap_flow_item.qos_flow_id;

      // qosFlowLevelQosParameters
      if (ngap_flow_item.qos_flow_level_qos_params.qos_characteristics.type() ==
          asn1::ngap::qos_characteristics_c::types::dynamic5_qi) {
        qos_flow_setup_req_item.qos_charact.is_dynamic_5qi = true;
        qos_flow_setup_req_item.qos_charact.five_qi =
            ngap_flow_item.qos_flow_level_qos_params.qos_characteristics.dynamic5_qi().five_qi;

        // TODO: ADD optional values

      } else if (ngap_flow_item.qos_flow_level_qos_params.qos_characteristics.type() ==
                 asn1::ngap::qos_characteristics_c::types::non_dynamic5_qi) {
        qos_flow_setup_req_item.qos_charact.is_dynamic_5qi = false;
        qos_flow_setup_req_item.qos_charact.five_qi =
            ngap_flow_item.qos_flow_level_qos_params.qos_characteristics.non_dynamic5_qi().five_qi;

        // TODO: ADD optional values
      }

      // allocationAndRetentionPriority
      qos_flow_setup_req_item.qos_charact.prio_level_arp =
          ngap_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.prio_level_arp;
      qos_flow_setup_req_item.qos_charact.pre_emption_cap =
          ngap_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_cap.to_string();
      qos_flow_setup_req_item.qos_charact.pre_emption_vulnerability =
          ngap_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_vulnerability.to_string();

      // Optional Parameters
      if (ngap_flow_item.qos_flow_level_qos_params.add_qos_flow_info_present) {
        qos_flow_setup_req_item.add_qos_flow_info.value() =
            ngap_flow_item.qos_flow_level_qos_params.add_qos_flow_info.to_string();
      }

      if (ngap_flow_item.qos_flow_level_qos_params.gbr_qos_info_present) {
        // TODO: Add to common type
      }

      if (ngap_flow_item.qos_flow_level_qos_params.reflective_qos_attribute_present) {
        qos_flow_setup_req_item.reflective_qos_attribute.value() =
            ngap_flow_item.qos_flow_level_qos_params.reflective_qos_attribute.to_string();
      }

      if (ngap_flow_item.erab_id_present) {
        qos_flow_setup_req_item.erab_id.value() = ngap_flow_item.erab_id;
      }

      setup_item.qos_flow_setup_request_items.push_back(qos_flow_setup_req_item);
    }

    cu_cp_pdu_session_resource_setup_msg.pdu_session_res_setup_items.push_back(setup_item);
  }
}

/// \brief Convert common type PDU Session Resource Setup Response message to NGAP PDU Session Resource Setup Response
/// message.
/// \param[out] resp The NGAP PDU Session Resource Setup Response message.
/// \param[in] cu_cp_resp The CU-CP PDU Session Resource Setup Response message.
inline void fill_pdu_session_res_setup_resp_s(asn1::ngap::pdu_session_res_setup_resp_s&               resp,
                                              const cu_cp_pdu_session_resource_setup_response_message cu_cp_resp)
{
  // Fill PDU Session Resource Setup Response List
  if (!cu_cp_resp.pdu_session_res_setup_response_items.empty()) {
    resp->pdu_session_res_setup_list_su_res_present = true;
    resp->pdu_session_res_setup_list_su_res->resize(cu_cp_resp.pdu_session_res_setup_response_items.size());

    for (auto& cu_cp_resp_item : cu_cp_resp.pdu_session_res_setup_response_items) {
      asn1::ngap::pdu_session_res_setup_item_su_res_s resp_item;

      resp_item.pdu_session_id = cu_cp_resp_item.pdu_session_id;

      asn1::ngap::pdu_session_res_setup_resp_transfer_s response_transfer;

      // Add dLQosFlowPerTNLInformation
      response_transfer.dlqos_flow_per_tnl_info = cu_cp_qos_flow_per_tnl_info_to_ngap_qos_flow_per_tnl_info(
          cu_cp_resp_item.pdu_session_resource_setup_response_transfer.dlqos_flow_per_tnl_info);

      // Add AdditionalDLQosFlowPerTNLInformation
      for (auto cu_cp_qos_flow_info :
           cu_cp_resp_item.pdu_session_resource_setup_response_transfer.add_dl_qos_flow_per_tnl_info) {
        asn1::ngap::qos_flow_per_tnl_info_item_s ngap_qos_flow_info;
        ngap_qos_flow_info.qos_flow_per_tnl_info =
            cu_cp_qos_flow_per_tnl_info_to_ngap_qos_flow_per_tnl_info(cu_cp_qos_flow_info);
        response_transfer.add_dl_qos_flow_per_tnl_info.push_back(ngap_qos_flow_info);
      }

      // Add QosFlowFailedToSetupList
      for (auto cu_cp_failed_item :
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
      byte_buffer   pdu;
      asn1::bit_ref bref(pdu);
      // TODO: Check return value
      response_transfer.pack(bref);

      resp_item.pdu_session_res_setup_resp_transfer.resize(pdu.length());
      std::copy(pdu.begin(), pdu.end(), resp_item.pdu_session_res_setup_resp_transfer.begin());

      resp->pdu_session_res_setup_list_su_res->push_back(resp_item);
    }
  }

  // Fill PDU Session Resource Failed to Setup List
  if (!cu_cp_resp.pdu_session_res_failed_to_setup_items.empty()) {
    resp->pdu_session_res_failed_to_setup_list_su_res_present = true;
    resp->pdu_session_res_failed_to_setup_list_su_res->resize(cu_cp_resp.pdu_session_res_failed_to_setup_items.size());
    for (auto& cu_cp_setup_failed_item : cu_cp_resp.pdu_session_res_failed_to_setup_items) {
      asn1::ngap::pdu_session_res_failed_to_setup_item_su_res_s setup_failed_item;

      asn1::ngap::pdu_session_res_setup_unsuccessful_transfer_s setup_unsuccessful_transfer;
      setup_unsuccessful_transfer.cause =
          cu_cp_cause_to_ngap_cause(cu_cp_setup_failed_item.pdu_session_resource_setup_unsuccessful_transfer.cause);
      // TODO: Add crit diagnostics

      // Pack pdu_session_res_setup_unsuccessful_transfer_s
      byte_buffer   pdu;
      asn1::bit_ref bref(pdu);
      // TODO: Check return value
      setup_unsuccessful_transfer.pack(bref);

      setup_failed_item.pdu_session_res_setup_unsuccessful_transfer.resize(pdu.length());
      std::copy(pdu.begin(), pdu.end(), setup_failed_item.pdu_session_res_setup_unsuccessful_transfer.begin());

      resp->pdu_session_res_failed_to_setup_list_su_res->push_back(setup_failed_item);
    }
  }
}

} // namespace srs_cu_cp
} // namespace srsgnb
