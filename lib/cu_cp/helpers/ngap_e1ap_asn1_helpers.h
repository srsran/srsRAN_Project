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

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/asn1/e1ap/e1ap.h"
#include "srsgnb/asn1/ngap/ngap.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/e1/cu_cp/e1_cu_cp.h"
#include <string>
#include <vector>

namespace srsgnb {
namespace srs_cu_cp {

inline asn1::e1ap::pdu_session_type_e
ngap_pdu_session_type_to_e1ap_pdu_session_type(asn1::ngap::pdu_session_type_e ngap_session_type)
{
  asn1::e1ap::pdu_session_type_e e1ap_session_type;

  switch (ngap_session_type) {
    case asn1::ngap::pdu_session_type_opts::options::ethernet:
      e1ap_session_type = asn1::e1ap::pdu_session_type_opts::options::ethernet;
      break;
    case asn1::ngap::pdu_session_type_opts::options::ipv4:
      e1ap_session_type = asn1::e1ap::pdu_session_type_opts::options::ipv4;
      break;
    case asn1::ngap::pdu_session_type_opts::options::ipv4v6:
      e1ap_session_type = asn1::e1ap::pdu_session_type_opts::options::ipv4v6;
      break;
    case asn1::ngap::pdu_session_type_opts::options::ipv6:
      e1ap_session_type = asn1::e1ap::pdu_session_type_opts::options::ipv6;
      break;

    default:
      e1ap_session_type = asn1::e1ap::pdu_session_type_opts::options::nulltype;
      break;
  }

  return e1ap_session_type;
}

inline void fill_e1ap_bearer_context_setup_request(e1ap_bearer_context_setup_request_message& e1_request,
                                                   const pdu_session_resource_setup_message&  msg)
{
  e1_request.sys_bearer_context_setup_request.set_ng_ran_bearer_context_setup_request();
  auto& ng_ran_bearer_context_setup_request =
      e1_request.sys_bearer_context_setup_request.ng_ran_bearer_context_setup_request();

  for (auto ngap_pdu_session_res_item : msg.pdu_session_res_setup_items) {
    asn1::protocol_ie_field_s<asn1::e1ap::ng_ran_bearer_context_setup_request_o> bearer_request_item;
    auto& e1ap_pdu_session_res_items = bearer_request_item.value().pdu_session_res_to_setup_list();

    asn1::e1ap::pdu_session_res_to_setup_item_s e1ap_pdu_session_res_item;

    // pdu session id
    e1ap_pdu_session_res_item.pdu_session_id = ngap_pdu_session_res_item.pdu_session_id;

    // s-NSSAI
    e1ap_pdu_session_res_item.snssai.sst.from_number(ngap_pdu_session_res_item.s_nssai.sst.to_number());
    if (ngap_pdu_session_res_item.s_nssai.sd_present) {
      e1ap_pdu_session_res_item.snssai.sd_present = true;
      e1ap_pdu_session_res_item.snssai.sd.from_number(ngap_pdu_session_res_item.s_nssai.sd.to_number());
    }

    asn1::cbit_ref bref(ngap_pdu_session_res_item.pdu_session_res_setup_request_transfer);
    asn1::ngap::pdu_session_res_setup_request_transfer_s pdu_session_res_setup_request_transfer;
    // TODO: Check return value
    pdu_session_res_setup_request_transfer.unpack(bref);

    // pdu session type
    e1ap_pdu_session_res_item.pdu_session_type =
        ngap_pdu_session_type_to_e1ap_pdu_session_type(pdu_session_res_setup_request_transfer->pdu_session_type.value);

    // ng ul up transport layer information
    if (pdu_session_res_setup_request_transfer->ul_ngu_up_tnl_info->type() ==
        asn1::ngap::up_transport_layer_info_c::types::gtp_tunnel) {
      e1ap_pdu_session_res_item.ng_ul_up_tnl_info.set_gtp_tunnel();
      auto& e1ap_gtp_tunnel    = e1ap_pdu_session_res_item.ng_ul_up_tnl_info.gtp_tunnel();
      e1ap_gtp_tunnel.gtp_teid = pdu_session_res_setup_request_transfer->ul_ngu_up_tnl_info->gtp_tunnel().gtp_teid;
      e1ap_gtp_tunnel.transport_layer_address.from_number(
          pdu_session_res_setup_request_transfer->ul_ngu_up_tnl_info->gtp_tunnel().transport_layer_address.to_number());
    } else {
      // TODO: error?
    }

    // TODO: add optional values

    // pdu session resource dl aggregate maximum bit rate
    if (pdu_session_res_setup_request_transfer->pdu_session_aggregate_maximum_bit_rate_present) {
      e1ap_pdu_session_res_item.pdu_session_res_dl_ambr_present = true;
      e1ap_pdu_session_res_item.pdu_session_res_dl_ambr =
          pdu_session_res_setup_request_transfer->pdu_session_aggregate_maximum_bit_rate
              ->pdu_session_aggregate_maximum_bit_rate_dl;
    }

    e1ap_pdu_session_res_items.push_back(e1ap_pdu_session_res_item);

    ng_ran_bearer_context_setup_request.push_back(bearer_request_item);
  }

  e1_request.uedl_aggregate_maximum_bit_rate = msg.ue_aggregate_maximum_bit_rate_dl;
}

inline asn1::ngap::cause_c e1ap_cause_to_ngap_cause(asn1::e1ap::cause_c e1ap_cause)
{
  asn1::ngap::cause_c ngap_cause;

  switch (e1ap_cause.type()) {
    case asn1::e1ap::cause_c::types_opts::radio_network:
      ngap_cause.set(asn1::ngap::cause_c::types_opts::radio_network);
      return ngap_cause;
      break;
    case asn1::e1ap::cause_c::types_opts::transport:
      ngap_cause.set(asn1::ngap::cause_c::types_opts::transport);
      return ngap_cause;
      break;
    case asn1::e1ap::cause_c::types_opts::protocol:
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

inline void fill_failed_ngap_pdu_session_res_setup_response(
    pdu_session_resource_setup_response_message&      res,
    const pdu_session_resource_setup_message&         msg,
    const e1ap_bearer_context_setup_response_message& e1_bearer_context_setup_resp_msg,
    asn1::ngap::cause_c                               cause)
{
  asn1::ngap::pdu_session_res_setup_unsuccessful_transfer_s setup_unsuccessful_transfer;
  setup_unsuccessful_transfer.cause = cause;
  if (e1_bearer_context_setup_resp_msg.failure->crit_diagnostics_present) {
    // TODO: Add crit diagnostics
  }

  // Pack pdu_session_res_setup_unsuccessful_transfer_s
  byte_buffer   pdu;
  asn1::bit_ref bref(pdu);
  // TODO: Check return value
  setup_unsuccessful_transfer.pack(bref);

  for (auto e1ap_failed_item : msg.pdu_session_res_setup_items) {
    asn1::ngap::pdu_session_res_failed_to_setup_item_su_res_s ngap_failed_item;
    ngap_failed_item.pdu_session_id                              = e1ap_failed_item.pdu_session_id;
    ngap_failed_item.pdu_session_res_setup_unsuccessful_transfer = std::move(pdu);
    res.pdu_session_res_failed_to_setup_items.push_back(ngap_failed_item);
  }
}

inline void fill_ngap_pdu_session_res_setup_response(
    pdu_session_resource_setup_response_message&      res,
    const pdu_session_resource_setup_message&         msg,
    const e1ap_bearer_context_setup_response_message& e1_bearer_context_setup_resp_msg)
{
  if (e1_bearer_context_setup_resp_msg.success) {
    auto& bearer_context_setup_response =
        e1_bearer_context_setup_resp_msg.response->sys_bearer_context_setup_resp->ng_ran_bearer_context_setup_resp();

    for (auto e1ap_response_item : bearer_context_setup_response.pdu_session_res_setup_list.value) {
      asn1::ngap::pdu_session_res_setup_item_su_res_s ngap_response_item;
      ngap_response_item.pdu_session_id = e1ap_response_item.pdu_session_id;

      // PDUSessionResourceSetupResponseTransfer
      asn1::ngap::pdu_session_res_setup_resp_transfer_s setup_response_transfer;

      // Add dlqos_flow_per_tnl_info
      setup_response_transfer.dlqos_flow_per_tnl_info.uptransport_layer_info.set_gtp_tunnel();
      auto& gtp_tunnel    = setup_response_transfer.dlqos_flow_per_tnl_info.uptransport_layer_info.gtp_tunnel();
      gtp_tunnel.gtp_teid = e1ap_response_item.ng_dl_up_tnl_info.gtp_tunnel().gtp_teid;
      gtp_tunnel.transport_layer_address = e1ap_response_item.ng_dl_up_tnl_info.gtp_tunnel().transport_layer_address;

      for (auto e1ap_drb_setup_item : e1ap_response_item.drb_setup_list_ng_ran) {
        for (auto e1ap_qos_flow_item : e1ap_drb_setup_item.flow_setup_list) {
          asn1::ngap::associated_qos_flow_item_s ngap_qos_flow_item;
          ngap_qos_flow_item.qos_flow_id = e1ap_qos_flow_item.qo_s_flow_id;

          setup_response_transfer.dlqos_flow_per_tnl_info.associated_qos_flow_list.push_back(ngap_qos_flow_item);
        }
        for (auto e1ap_failed_qos_flow_item : e1ap_drb_setup_item.flow_failed_list) {
          asn1::ngap::qos_flow_with_cause_item_s ngap_failed_qos_flow_item;
          ngap_failed_qos_flow_item.qos_flow_id = e1ap_failed_qos_flow_item.qo_s_flow_id;
          ngap_failed_qos_flow_item.cause       = e1ap_cause_to_ngap_cause(e1ap_failed_qos_flow_item.cause);

          setup_response_transfer.qos_flow_failed_to_setup_list.push_back(ngap_failed_qos_flow_item);
        }
      }

      // TODO: Add optional values

      // Pack pdu_session_res_setup_resp_transfer_s
      byte_buffer   pdu;
      asn1::bit_ref bref(pdu);
      setup_response_transfer.pack(bref);

      ngap_response_item.pdu_session_res_setup_resp_transfer = std::move(pdu);

      res.pdu_session_res_setup_response_items.push_back(ngap_response_item);
    }

    if (bearer_context_setup_response.pdu_session_res_failed_list_present) {
      for (auto e1ap_failed_item : bearer_context_setup_response.pdu_session_res_failed_list.value) {
        asn1::ngap::pdu_session_res_failed_to_setup_item_su_res_s ngap_failed_item;
        ngap_failed_item.pdu_session_id = e1ap_failed_item.pdu_session_id;

        asn1::ngap::pdu_session_res_setup_unsuccessful_transfer_s setup_unsuccessful_transfer;
        setup_unsuccessful_transfer.cause = e1ap_cause_to_ngap_cause(e1ap_failed_item.cause);
        // Pack pdu_session_res_setup_unsuccessful_transfer_s
        byte_buffer   pdu;
        asn1::bit_ref bref(pdu);
        setup_unsuccessful_transfer.pack(bref);

        ngap_failed_item.pdu_session_res_setup_unsuccessful_transfer = std::move(pdu);
        res.pdu_session_res_failed_to_setup_items.push_back(ngap_failed_item);
      }
    }
  } else {
    fill_failed_ngap_pdu_session_res_setup_response(
        res,
        msg,
        e1_bearer_context_setup_resp_msg,
        e1ap_cause_to_ngap_cause(e1_bearer_context_setup_resp_msg.failure->cause.value));
  }
}

} // namespace srs_cu_cp
} // namespace srsgnb
