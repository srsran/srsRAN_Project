/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "cu_cp_test_messages.h"
#include "lib/e1ap/cu_cp/e1ap_cu_cp_asn1_helpers.h"
#include "lib/f1ap/cu_cp/f1ap_asn1_helpers.h"
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/f1ap/f1ap_message.h"

using namespace srsran;
using namespace srs_cu_cp;

cu_cp_ue_context_release_command srsran::srs_cu_cp::generate_ue_context_release_command(ue_index_t   ue_index,
                                                                                        ngap_cause_t cause)
{
  cu_cp_ue_context_release_command ue_context_release_command = {};
  ue_context_release_command.ue_index                         = ue_index;
  ue_context_release_command.cause                            = cause;
  return ue_context_release_command;
}

cu_cp_pdu_session_resource_setup_request
srsran::srs_cu_cp::generate_pdu_session_resource_setup(ue_index_t ue_index,
                                                       unsigned   num_pdu_sessions,
                                                       unsigned   num_qos_flows)
{
  cu_cp_pdu_session_resource_setup_request req;
  req.ue_index = ue_index;

  req.ue_aggregate_maximum_bit_rate_dl = 1000;
  req.ue_aggregate_maximum_bit_rate_ul = 1000;

  for (unsigned i = 0; i < num_pdu_sessions; ++i) {
    pdu_session_id_t pdu_session_id = uint_to_pdu_session_id(i + 1);

    cu_cp_pdu_session_res_setup_item item;
    item.pdu_session_id = pdu_session_id;
    (void)item.pdu_session_nas_pdu.resize(2);
    item.pdu_session_nas_pdu[0] = 0xaa;
    item.pdu_session_nas_pdu[1] = 0xbb;
    item.s_nssai.sst            = slice_service_type{1};

    item.pdu_session_aggregate_maximum_bit_rate_dl = 100;
    item.pdu_session_aggregate_maximum_bit_rate_ul = 100;
    item.ul_ngu_up_tnl_info = {transport_layer_address::create_from_string("127.0.0.1"), int_to_gtpu_teid(0x1)};
    item.pdu_session_type   = pdu_session_type_t::ipv4;
    item.security_ind       = {};

    for (unsigned k = 0; k < num_qos_flows; ++k) {
      qos_flow_setup_request_item qos_item;
      qos_item.qos_flow_id = uint_to_qos_flow_id(i + k + 1);

      non_dyn_5qi_descriptor non_dyn_5qi;
      non_dyn_5qi.five_qi                         = uint_to_five_qi(9); // all with same FiveQI
      qos_item.qos_flow_level_qos_params.qos_desc = non_dyn_5qi;

      qos_item.qos_flow_level_qos_params.alloc_retention_prio.prio_level_arp = 8;

      item.qos_flow_setup_request_items.emplace(qos_item.qos_flow_id, qos_item);
    }

    req.pdu_session_res_setup_items.emplace(pdu_session_id, std::move(item));
  }

  return req;
}

cu_cp_pdu_session_resource_release_command srsran::srs_cu_cp::generate_pdu_session_resource_release(ue_index_t ue_index)
{
  cu_cp_pdu_session_resource_release_command cmd;
  pdu_session_id_t                           pdu_session_id = uint_to_pdu_session_id(1);

  cmd.ue_index = ue_index;

  cu_cp_pdu_session_res_to_release_item_rel_cmd pdu_session_res_to_release_item_rel_cmd;
  pdu_session_res_to_release_item_rel_cmd.pdu_session_id = pdu_session_id;
  pdu_session_res_to_release_item_rel_cmd.pdu_session_res_release_cmd_transfer.cause =
      ngap_cause_radio_network_t::unspecified;

  cmd.pdu_session_res_to_release_list_rel_cmd.emplace(pdu_session_id, pdu_session_res_to_release_item_rel_cmd);

  return cmd;
}

cu_cp_pdu_session_resource_modify_request
srsran::srs_cu_cp::generate_pdu_session_resource_modification(ue_index_t ue_index, unsigned psi, unsigned qfi)
{
  cu_cp_pdu_session_resource_modify_request request;
  request.ue_index = ue_index;

  cu_cp_pdu_session_res_modify_item_mod_req modify_item;
  modify_item.pdu_session_id = uint_to_pdu_session_id(psi);

  cu_cp_qos_flow_add_or_mod_item qos_item;
  qos_item.qos_flow_id = uint_to_qos_flow_id(qfi);
  {
    non_dyn_5qi_descriptor non_dyn_5qi;
    non_dyn_5qi.five_qi                                                    = uint_to_five_qi(7);
    qos_item.qos_flow_level_qos_params.qos_desc                            = non_dyn_5qi;
    qos_item.qos_flow_level_qos_params.alloc_retention_prio.prio_level_arp = 8;
  }

  cu_cp_pdu_session_res_modify_request_transfer transfer;
  transfer.qos_flow_add_or_modify_request_list.emplace(qos_item.qos_flow_id, qos_item);

  modify_item.transfer = transfer;
  request.pdu_session_res_modify_items.emplace(modify_item.pdu_session_id, modify_item);

  return request;
}

cu_cp_pdu_session_resource_modify_request
srsran::srs_cu_cp::generate_pdu_session_resource_modification_with_qos_flow_removal(qos_flow_id_t flow_id)
{
  cu_cp_pdu_session_resource_modify_request request;
  request.ue_index = uint_to_ue_index(0);

  cu_cp_pdu_session_res_modify_item_mod_req modify_item;
  modify_item.pdu_session_id = uint_to_pdu_session_id(1);

  cu_cp_pdu_session_res_modify_request_transfer transfer;

  // Add item to remove inexisting QoS flow.
  cu_cp_qos_flow_with_cause_item release_item;
  release_item.qos_flow_id = flow_id;
  release_item.cause       = ngap_cause_radio_network_t::unspecified;
  transfer.qos_flow_to_release_list.emplace(release_item.qos_flow_id, release_item);

  modify_item.transfer = transfer;
  request.pdu_session_res_modify_items.emplace(modify_item.pdu_session_id, modify_item);

  return request;
}

e1ap_bearer_context_setup_response
srsran::srs_cu_cp::generate_e1ap_bearer_context_setup_response(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                               gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id)
{
  e1ap_bearer_context_setup_response resp;

  e1ap_message asn1_res = generate_bearer_context_setup_response(cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);

  fill_e1ap_bearer_context_setup_response(resp, asn1_res.pdu.successful_outcome().value.bearer_context_setup_resp());

  return resp;
}

e1ap_bearer_context_modification_response
srsran::srs_cu_cp::generate_e1ap_bearer_context_modification_response(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                                      gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id)
{
  e1ap_bearer_context_modification_response resp;

  e1ap_message asn1_res = generate_bearer_context_modification_response(cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);

  fill_e1ap_bearer_context_modification_response(resp,
                                                 asn1_res.pdu.successful_outcome().value.bearer_context_mod_resp());

  return resp;
}
