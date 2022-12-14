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
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/e1/cu_cp/e1_cu_cp.h"
#include <string>
#include <vector>

namespace srsgnb {
namespace srs_cu_cp {

inline asn1::e1ap::pdu_session_type_e cu_cp_pdu_session_type_to_e1ap_pdu_session_type(std::string cu_cp_session_type)
{
  asn1::e1ap::pdu_session_type_e e1ap_session_type;

  if (cu_cp_session_type == "ethernet") {
    e1ap_session_type = asn1::e1ap::pdu_session_type_opts::options::ethernet;
    return e1ap_session_type;
  } else if (cu_cp_session_type == "ipv4") {
    e1ap_session_type = asn1::e1ap::pdu_session_type_opts::options::ipv4;
    return e1ap_session_type;
  } else if (cu_cp_session_type == "ipv4v6") {
    e1ap_session_type = asn1::e1ap::pdu_session_type_opts::options::ipv4v6;
    return e1ap_session_type;
  } else if (cu_cp_session_type == "ipv6") {
    e1ap_session_type = asn1::e1ap::pdu_session_type_opts::options::ipv6;
    return e1ap_session_type;
  } else {
    e1ap_session_type = asn1::e1ap::pdu_session_type_opts::options::nulltype;
    return e1ap_session_type;
  }
}

inline void fill_e1ap_bearer_context_setup_request(e1ap_bearer_context_setup_request_message&     e1_request,
                                                   const e1ap_pdu_session_resource_setup_message& msg)
{
  e1_request.sys_bearer_context_setup_request.set_ng_ran_bearer_context_setup_request();
  auto& ng_ran_bearer_context_setup_request =
      e1_request.sys_bearer_context_setup_request.ng_ran_bearer_context_setup_request();

  for (auto pdu_session_res_item : msg.pdu_session_res_setup_items) {
    asn1::protocol_ie_field_s<asn1::e1ap::ng_ran_bearer_context_setup_request_o> bearer_request_item;
    auto& e1ap_pdu_session_res_items = bearer_request_item.value().pdu_session_res_to_setup_list();

    asn1::e1ap::pdu_session_res_to_setup_item_s e1ap_pdu_session_res_item;

    // pdu session id
    e1ap_pdu_session_res_item.pdu_session_id = pdu_session_res_item.pdu_session_id;

    // s-NSSAI
    e1ap_pdu_session_res_item.snssai.sst.from_number(pdu_session_res_item.s_nssai.sst);
    if (pdu_session_res_item.s_nssai.sd.has_value()) {
      e1ap_pdu_session_res_item.snssai.sd_present = true;
      e1ap_pdu_session_res_item.snssai.sd.from_number(pdu_session_res_item.s_nssai.sd.value());
    }

    // pdu session type
    e1ap_pdu_session_res_item.pdu_session_type =
        cu_cp_pdu_session_type_to_e1ap_pdu_session_type(pdu_session_res_item.pdu_session_type);

    // ng ul up transport layer information
    e1ap_pdu_session_res_item.ng_ul_up_tnl_info.set_gtp_tunnel();
    auto& e1ap_gtp_tunnel = e1ap_pdu_session_res_item.ng_ul_up_tnl_info.gtp_tunnel();
    e1ap_gtp_tunnel.gtp_teid.from_number(pdu_session_res_item.ul_ngu_up_tnl_info.gtp_teid);
    e1ap_gtp_tunnel.transport_layer_address.from_number(
        pdu_session_res_item.ul_ngu_up_tnl_info.transport_layer_address);

    // TODO: add optional values

    // pdu session resource dl aggregate maximum bit rate
    e1ap_pdu_session_res_item.pdu_session_res_dl_ambr_present = true;
    e1ap_pdu_session_res_item.pdu_session_res_dl_ambr = pdu_session_res_item.pdu_session_aggregate_maximum_bit_rate_dl;

    e1ap_pdu_session_res_items.push_back(e1ap_pdu_session_res_item);

    ng_ran_bearer_context_setup_request.push_back(bearer_request_item);
  }

  e1_request.uedl_aggregate_maximum_bit_rate = msg.ue_aggregate_maximum_bit_rate_dl;
}

inline cu_cp_cause_t e1ap_cause_to_cu_cp_cause(asn1::e1ap::cause_c e1ap_cause)
{
  cu_cp_cause_t cu_cp_cause;

  switch (e1ap_cause.type()) {
    case asn1::e1ap::cause_c::types_opts::radio_network:
      cu_cp_cause = cu_cp_cause_t::radio_network;
      return cu_cp_cause;
      break;
    case asn1::e1ap::cause_c::types_opts::transport:
      cu_cp_cause = cu_cp_cause_t::transport;
      return cu_cp_cause;
      break;
    case asn1::e1ap::cause_c::types_opts::protocol:
      cu_cp_cause = cu_cp_cause_t::protocol;
      return cu_cp_cause;
      break;
    default:
      cu_cp_cause = cu_cp_cause_t::misc;
      return cu_cp_cause;
      break;
      break;
  }
}

inline void fill_failed_rrc_ue_pdu_session_res_setup_response(
    e1ap_pdu_session_resource_setup_response_message& res,
    const e1ap_pdu_session_resource_setup_message&    msg,
    const e1ap_bearer_context_setup_response_message& e1_bearer_context_setup_resp_msg,
    cu_cp_cause_t                                     cause)
{
  for (auto e1ap_failed_item : msg.pdu_session_res_setup_items) {
    cu_cp_pdu_session_res_setup_failed_item failed_item;
    failed_item.pdu_session_id                                         = e1ap_failed_item.pdu_session_id;
    failed_item.pdu_session_resource_setup_unsuccessful_transfer.cause = cause;
    if (e1_bearer_context_setup_resp_msg.failure->crit_diagnostics_present) {
      // TODO: Add crit diagnostics
    }
    res.pdu_session_res_failed_to_setup_items.push_back(failed_item);
  }
}

inline void fill_rrc_ue_pdu_session_res_setup_response(
    e1ap_pdu_session_resource_setup_response_message& res,
    const e1ap_pdu_session_resource_setup_message&    msg,
    const e1ap_bearer_context_setup_response_message& e1_bearer_context_setup_resp_msg)
{
  if (e1_bearer_context_setup_resp_msg.success) {
    auto& bearer_context_setup_response =
        e1_bearer_context_setup_resp_msg.response->sys_bearer_context_setup_resp->ng_ran_bearer_context_setup_resp();

    for (auto e1ap_response_item : bearer_context_setup_response.pdu_session_res_setup_list.value) {
      cu_cp_pdu_session_res_setup_response_item response_item;
      response_item.pdu_session_id = e1ap_response_item.pdu_session_id;

      // PDUSessionResourceSetupResponseTransfer
      auto& response_transfer = response_item.pdu_session_resource_setup_response_transfer;

      // Add dlqos_flow_per_tnl_info
      auto& uptransport_layer_info    = response_transfer.dlqos_flow_per_tnl_info.uptransport_layer_info;
      uptransport_layer_info.gtp_teid = e1ap_response_item.ng_dl_up_tnl_info.gtp_tunnel().gtp_teid.to_number();
      uptransport_layer_info.transport_layer_address =
          e1ap_response_item.ng_dl_up_tnl_info.gtp_tunnel().transport_layer_address.to_number();

      for (auto e1ap_drb_setup_item : e1ap_response_item.drb_setup_list_ng_ran) {
        res.rrc_ue_drb_setup_msg.drb_id = e1ap_drb_setup_item.drb_id;
        for (auto ul_up_transport_param : e1ap_drb_setup_item.ul_up_transport_params) {
          rrc_ue_gtp_tunnel gtp_tunnel;
          gtp_tunnel.gtp_teid = ul_up_transport_param.up_tnl_info.gtp_tunnel().gtp_teid.to_number();
          gtp_tunnel.transport_layer_address =
              ul_up_transport_param.up_tnl_info.gtp_tunnel().transport_layer_address.to_number();
          gtp_tunnel.cell_group_id = ul_up_transport_param.cell_group_id;

          res.rrc_ue_drb_setup_msg.gtp_tunnels.push_back(gtp_tunnel);
        }

        for (auto e1ap_qos_flow_item : e1ap_drb_setup_item.flow_setup_list) {
          cu_cp_associated_qos_flow qos_flow_item;
          qos_flow_item.qos_flow_id = e1ap_qos_flow_item.qo_s_flow_id;

          response_transfer.dlqos_flow_per_tnl_info.associated_qos_flow_list.push_back(qos_flow_item);
        }
        for (auto e1ap_failed_qos_flow_item : e1ap_drb_setup_item.flow_failed_list) {
          cu_cp_qos_flow_failed_to_setup_item failed_qos_flow_item;

          failed_qos_flow_item.qos_flow_id = e1ap_failed_qos_flow_item.qo_s_flow_id;
          failed_qos_flow_item.cause       = e1ap_cause_to_cu_cp_cause(e1ap_failed_qos_flow_item.cause);

          response_transfer.qos_flow_failed_to_setup_list.push_back(failed_qos_flow_item);
        }
      }

      // TODO: Add optional values

      res.pdu_session_res_setup_response_items.push_back(response_item);
    }

    if (bearer_context_setup_response.pdu_session_res_failed_list_present) {
      for (auto e1ap_failed_item : bearer_context_setup_response.pdu_session_res_failed_list.value) {
        cu_cp_pdu_session_res_setup_failed_item failed_item;

        failed_item.pdu_session_id = e1ap_failed_item.pdu_session_id;

        failed_item.pdu_session_resource_setup_unsuccessful_transfer.cause =
            e1ap_cause_to_cu_cp_cause(e1ap_failed_item.cause);
        res.pdu_session_res_failed_to_setup_items.push_back(failed_item);
      }
    }
  } else {
    fill_failed_rrc_ue_pdu_session_res_setup_response(
        res,
        msg,
        e1_bearer_context_setup_resp_msg,
        e1ap_cause_to_cu_cp_cause(e1_bearer_context_setup_resp_msg.failure->cause.value));
  }
}

} // namespace srs_cu_cp
} // namespace srsgnb
