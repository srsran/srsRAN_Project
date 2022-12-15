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

#include "../converters/rrc_ue_asn1_converters.h"
#include "srsgnb/asn1/f1ap/f1ap.h"
#include "srsgnb/f1c/cu_cp/f1c_cu.h"

namespace srsgnb {
namespace srs_cu_cp {

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
    e1ap_pdu_session_res_item.snssai = common_snssai_to_e1ap_snssai(pdu_session_res_item.s_nssai);

    // pdu session type
    e1ap_pdu_session_res_item.pdu_session_type =
        cu_cp_pdu_session_type_to_e1ap_pdu_session_type(pdu_session_res_item.pdu_session_type);

    // ng ul up transport layer information
    e1ap_pdu_session_res_item.ng_ul_up_tnl_info =
        cu_cp_ul_ngu_up_tnl_info_to_e1ap_up_tnl_info(pdu_session_res_item.ul_ngu_up_tnl_info);

    // TODO: add optional values

    // pdu session resource dl aggregate maximum bit rate
    e1ap_pdu_session_res_item.pdu_session_res_dl_ambr_present = true;
    e1ap_pdu_session_res_item.pdu_session_res_dl_ambr = pdu_session_res_item.pdu_session_aggregate_maximum_bit_rate_dl;

    e1ap_pdu_session_res_items.push_back(e1ap_pdu_session_res_item);

    ng_ran_bearer_context_setup_request.push_back(bearer_request_item);
  }

  e1_request.uedl_aggregate_maximum_bit_rate = msg.ue_aggregate_maximum_bit_rate_dl;
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
      auto& dlqos_flow_per_tnl_info = response_transfer.dlqos_flow_per_tnl_info;

      // Add uptransport_layer_info
      dlqos_flow_per_tnl_info.uptransport_layer_info =
          cu_cp_uptransport_layer_info_from_e1ap_up_tnl_info(e1ap_response_item.ng_dl_up_tnl_info);

      for (auto e1ap_drb_setup_item : e1ap_response_item.drb_setup_list_ng_ran) {
        res.rrc_ue_drb_setup_msg.drb_id = e1ap_drb_setup_item.drb_id;

        // Add gtp tunnels
        for (auto ul_up_transport_param : e1ap_drb_setup_item.ul_up_transport_params) {
          rrc_ue_gtp_tunnel gtp_tunnel = e1ap_up_param_item_to_rrc_ue_gtp_tunnel(ul_up_transport_param);
          res.rrc_ue_drb_setup_msg.gtp_tunnels.push_back(gtp_tunnel);
        }

        // Add associated qos flows
        for (auto e1ap_qos_flow_item : e1ap_drb_setup_item.flow_setup_list) {
          cu_cp_associated_qos_flow qos_flow_item;
          qos_flow_item.qos_flow_id = e1ap_qos_flow_item.qo_s_flow_id;

          response_transfer.dlqos_flow_per_tnl_info.associated_qos_flow_list.push_back(qos_flow_item);
        }

        // Add qos flow failed to setups
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

    // Add pdu session res failed list
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

inline void fill_f1ap_ue_context_modification_request(f1ap_ue_context_modification_request_message&  f1c_request,
                                                      const f1ap_pdu_session_resource_setup_message& msg)
{
  // drb to be setup mod list
  f1c_request.msg->drbs_to_be_setup_mod_list_present = true;

  asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_setup_mod_item_ies_o> f1ap_setup_item;
  auto& f1ap_drb_to_setup_item = f1ap_setup_item->drbs_to_be_setup_mod_item();

  f1ap_drb_to_setup_item.drbid = msg.rrc_ue_drb_setup_msg.drb_id;

  for (auto rrc_ue_gtp_tunnel_item : msg.rrc_ue_drb_setup_msg.gtp_tunnels) {
    asn1::f1ap::uluptnl_info_to_be_setup_item_s uluptnl_item =
        rrc_ue_gtp_tunnel_to_f1ap_uluptnl_info_to_be_setup_item(rrc_ue_gtp_tunnel_item);

    f1ap_drb_to_setup_item.uluptnl_info_to_be_setup_list.push_back(uluptnl_item);
  }

  // TODO: Add qos information and optional values

  f1c_request.msg->drbs_to_be_setup_mod_list.value.push_back(f1ap_setup_item);

  if (msg.ue_aggregate_maximum_bit_rate_ul.has_value()) {
    f1c_request.msg->gnb_du_ue_ambr_ul_present = true;
    f1c_request.msg->gnb_du_ue_ambr_ul.value   = msg.ue_aggregate_maximum_bit_rate_ul.value();
  }
}

} // namespace srs_cu_cp
} // namespace srsgnb
