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

#include "srsgnb/adt/expected.h"
#include "srsgnb/asn1/ngap/ngap.h"
#include "srsgnb/ngap/ngc_types.h"

namespace srsgnb {
namespace srs_cu_cp {

/// \brief Convert NGAP ASN1 PDU Session Resource Setup List SU REQ ASN1 struct to common type.
/// \param[out] cu_cp_pdu_session_res_setup_msg The cu_cp_pdu_session_res_setup_msg struct to fill.
/// \param[in] pdu_session_res_setup_su_req The pdu_session_res_setup_list_su_req ASN1 struct.
inline void pdu_session_resource_setup_list_su_req_to_cu_cp_type(
    cu_cp_pdu_session_resource_setup_message& cu_cp_pdu_session_resource_setup_msg,
    const asn1::dyn_seq_of<asn1::ngap::pdu_session_res_setup_item_su_req_s, 1U, 256U, true>&
        pdu_session_res_setup_su_req)
{
  for (auto it = pdu_session_res_setup_su_req.begin(); it != pdu_session_res_setup_su_req.end(); ++it) {
    cu_cp_pdu_session_res_setup_item setup_item;

    // pDUSessionID
    setup_item.pdu_session_id = it->pdu_session_id;

    // pDUSessionNAS-PDU
    if (!it->pdu_session_nas_pdu.empty()) {
      setup_item.pdu_session_nas_pdu.resize(it->pdu_session_nas_pdu.size());
      std::copy(it->pdu_session_nas_pdu.begin(), it->pdu_session_nas_pdu.end(), setup_item.pdu_session_nas_pdu.begin());
    }

    // s-NSSAI
    if (it->s_nssai.sd_present) {
      setup_item.s_nssai.sd = it->s_nssai.sd.to_number();
    }
    setup_item.s_nssai.sst = it->s_nssai.sst.to_number();

    // pDUSessionResourceSetupRequestTransfer
    asn1::cbit_ref                                                   bref(it->pdu_session_res_setup_request_transfer);
    asn1::ngap::pdu_session_res_setup_request_transfer_ies_container setup_req_transfer = {};
    if (setup_req_transfer.unpack(bref) != asn1::SRSASN_SUCCESS) {
      //   logger.error("Couldn't unpack PDU Session Resource Setup Request Transfer PDU");
      return;
    }

    // id-PDUSessionAggregateMaximumBitRate
    setup_item.pdu_session_aggregate_maximum_bit_rate_dl =
        setup_req_transfer.pdu_session_aggregate_maximum_bit_rate.value.pdu_session_aggregate_maximum_bit_rate_dl;
    setup_item.pdu_session_aggregate_maximum_bit_rate_ul =
        setup_req_transfer.pdu_session_aggregate_maximum_bit_rate.value.pdu_session_aggregate_maximum_bit_rate_ul;

    // id-UL-NGU-UP-TNLInformation
    setup_item.ul_ngu_up_tnl_info.transport_layer_address =
        setup_req_transfer.ul_ngu_up_tnl_info.value.gtp_tunnel().transport_layer_address.to_number();
    setup_item.ul_ngu_up_tnl_info.gtp_teid =
        setup_req_transfer.ul_ngu_up_tnl_info.value.gtp_tunnel().gtp_teid.to_number();

    // id-PDUSessionType
    setup_item.pdu_session_type = setup_req_transfer.pdu_session_type.value.to_string();

    // id-QosFlowSetupRequestList
    for (auto it2 = setup_req_transfer.qos_flow_setup_request_list.value.begin();
         it2 != setup_req_transfer.qos_flow_setup_request_list.value.end();
         ++it2) {
      qos_flow_setup_request_item qos_flow_setup_req_item;

      // qosFlowIdentifier
      qos_flow_setup_req_item.qos_flow_id = it2->qos_flow_id;

      // qosFlowLevelQosParameters
      if (it2->qos_flow_level_qos_params.qos_characteristics.type() ==
          asn1::ngap::qos_characteristics_c::types::dynamic5_qi) {
        qos_flow_setup_req_item.qos_charact.is_dynamic_5qi = true;
        qos_flow_setup_req_item.qos_charact.five_qi =
            it2->qos_flow_level_qos_params.qos_characteristics.dynamic5_qi().five_qi;

        // TODO: ADD optional valuess

      } else if (it2->qos_flow_level_qos_params.qos_characteristics.type() ==
                 asn1::ngap::qos_characteristics_c::types::non_dynamic5_qi) {
        qos_flow_setup_req_item.qos_charact.is_dynamic_5qi = false;
        qos_flow_setup_req_item.qos_charact.five_qi =
            it2->qos_flow_level_qos_params.qos_characteristics.non_dynamic5_qi().five_qi;

        // TODO: ADD optional values
      }

      // allocationAndRetentionPriority
      qos_flow_setup_req_item.qos_charact.prio_level_arp =
          it2->qos_flow_level_qos_params.alloc_and_retention_prio.prio_level_arp;
      qos_flow_setup_req_item.qos_charact.pre_emption_cap =
          it2->qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_cap.to_string();
      qos_flow_setup_req_item.qos_charact.pre_emption_vulnerability =
          it2->qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_vulnerability.to_string();

      // Optional Parameters
      if (it2->qos_flow_level_qos_params.add_qos_flow_info_present) {
        qos_flow_setup_req_item.add_qos_flow_info.value() =
            it2->qos_flow_level_qos_params.add_qos_flow_info.to_string();
      }

      if (it2->qos_flow_level_qos_params.gbr_qos_info_present) {
        // TODO: Add to common type
      }

      if (it2->qos_flow_level_qos_params.reflective_qos_attribute_present) {
        qos_flow_setup_req_item.reflective_qos_attribute.value() =
            it2->qos_flow_level_qos_params.reflective_qos_attribute.to_string();
      }

      if (it2->erab_id_present) {
        qos_flow_setup_req_item.erab_id.value() = it2->erab_id;
      }

      setup_item.qos_flow_setup_request_items.push_back(qos_flow_setup_req_item);
    }

    cu_cp_pdu_session_resource_setup_msg.pdu_session_res_setup_items.push_back(setup_item);
  }
}

inline asn1::ngap::cause_c cu_cp_cause_to_ngap_cause(cu_cp_cause_t cu_cp_cause)
{
  asn1::ngap::cause_c ngap_cause;

  switch (cu_cp_cause) {
    case cu_cp_cause_t::radio_network:
      ngap_cause.set(asn1::ngap::cause_c::types_opts::radio_network);
      return ngap_cause;
      break;
    case cu_cp_cause_t::transport:
      ngap_cause.set(asn1::ngap::cause_c::types_opts::transport);
      return ngap_cause;
      break;
    case cu_cp_cause_t::protocol:
      ngap_cause.set(asn1::ngap::cause_c::types_opts::protocol);
      return ngap_cause;
      break;
    default:
      ngap_cause.set(asn1::ngap::cause_c::types_opts::misc);
      return ngap_cause;
      break;
      break;
  }
}

inline void cu_cp_type_to_pdu_session_res_setup_resp_s(asn1::ngap::pdu_session_res_setup_resp_s&         resp,
                                                       cu_cp_pdu_session_resource_setup_response_message cu_cp_resp)
{
  // Fill PDU Session Resource Setup Response List
  if (!cu_cp_resp.pdu_session_res_setup_response_items.empty()) {
    resp->pdu_session_res_setup_list_su_res_present = true;
    resp->pdu_session_res_setup_list_su_res->resize(cu_cp_resp.pdu_session_res_setup_response_items.size());

    for (auto& cu_cp_resp_item : cu_cp_resp.pdu_session_res_setup_response_items) {
      asn1::ngap::pdu_session_res_setup_item_su_res_s resp_item;

      resp_item.pdu_session_id = cu_cp_resp_item.pdu_session_id;

      asn1::ngap::pdu_session_res_setup_resp_transfer_s response_transfer;

      // TODO: Add helper functions to convert structs

      if (cu_cp_resp_item.pdu_session_resource_setup_response_transfer.security_result.has_value()) {
        // TODO: Convert values
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
