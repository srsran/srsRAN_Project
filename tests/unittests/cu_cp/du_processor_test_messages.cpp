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

#include "du_processor_test_messages.h"
#include "lib/e1ap/cu_cp/e1ap_cu_cp_asn1_helpers.h"
#include "lib/f1ap/common/asn1_helpers.h"
#include "lib/f1ap/cu_cp/f1ap_asn1_helpers.h"
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "tests/unittests/f1ap/common/f1ap_cu_test_messages.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/f1ap/common/f1ap_message.h"

using namespace srsran;
using namespace srs_cu_cp;

void srsran::srs_cu_cp::generate_valid_f1_setup_request(f1ap_f1_setup_request& f1_setup_request,
                                                        unsigned               gnb_du_id,
                                                        unsigned               nrcell_id,
                                                        pci_t                  pci)
{
  f1ap_message f1setup_msg = generate_f1_setup_request(gnb_du_id, nrcell_id, pci);
  fill_f1_setup_request(f1_setup_request, f1setup_msg.pdu.init_msg().value.f1_setup_request());
}

void srsran::srs_cu_cp::generate_f1_setup_request_base(f1ap_f1_setup_request& f1_setup_request)
{
  f1ap_message f1setup_msg = generate_f1_setup_request(0x11, 6576, 0);
  f1setup_msg.pdu.init_msg().value.f1_setup_request()->gnb_du_served_cells_list_present = false;
  f1setup_msg.pdu.init_msg().value.f1_setup_request()->gnb_du_served_cells_list.clear();
  fill_f1_setup_request(f1_setup_request, f1setup_msg.pdu.init_msg().value.f1_setup_request());
}

void srsran::srs_cu_cp::generate_f1_setup_request_with_too_many_cells(f1ap_f1_setup_request& f1_setup_request)
{
  f1ap_message f1setup_msg  = generate_f1_setup_request(0x11, 6576, 0);
  auto&        f1_setup_req = f1setup_msg.pdu.init_msg().value.f1_setup_request();
  f1_setup_req->gnb_du_served_cells_list.clear();

  f1_setup_req->gnb_du_served_cells_list_present = true;

  for (int du_cell_idx_int = du_cell_index_to_uint(du_cell_index_t::min); du_cell_idx_int < MAX_NOF_DU_CELLS + 1;
       du_cell_idx_int++) {
    f1_setup_req->gnb_du_served_cells_list.push_back({});
    f1_setup_req->gnb_du_served_cells_list.back().load_info_obj(ASN1_F1AP_ID_GNB_DU_SERVED_CELLS_ITEM);
    f1_setup_req->gnb_du_served_cells_list.back()->gnb_du_served_cells_item() =
        generate_served_cells_item(du_cell_idx_int, du_cell_idx_int);
  }
  fill_f1_setup_request(f1_setup_request, f1setup_msg.pdu.init_msg().value.f1_setup_request());
}

cu_cp_ue_creation_message
srsran::srs_cu_cp::generate_ue_creation_message(ue_index_t ue_index, rnti_t c_rnti, unsigned nrcell_id)
{
  cu_cp_ue_creation_message ue_creation_msg = {};
  ue_creation_msg.ue_index                  = ue_index;
  ue_creation_msg.c_rnti                    = c_rnti;
  asn1::f1ap::nr_cgi_s asn1_cgi;
  asn1_cgi.nr_cell_id.from_number(nrcell_id);
  asn1_cgi.plmn_id.from_string("02f899");
  ue_creation_msg.cgi = cgi_from_asn1(asn1_cgi);
  asn1::unbounded_octstring<true> tmp;
  tmp.from_string(
      "5c00b001117aec701061e0007c20408d07810020a2090480ca8000f800000000008370842000088165000048200002069a06aa49880002"
      "00204000400d008013b64b1814400e468acf120000096070820f177e060870000000e25038000040bde802000400000000028201950300"
      "c400");
  ue_creation_msg.du_to_cu_rrc_container = {tmp.begin(), tmp.end()};

  return ue_creation_msg;
}

cu_cp_ue_context_release_command srsran::srs_cu_cp::generate_ue_context_release_command(ue_index_t ue_index)
{
  cu_cp_ue_context_release_command ue_context_release_command = {};
  ue_context_release_command.ue_index                         = ue_index;
  ue_context_release_command.cause                            = cause_radio_network_t::unspecified;
  return ue_context_release_command;
}

cu_cp_pdu_session_resource_setup_request
srsran::srs_cu_cp::generate_pdu_session_resource_setup(unsigned num_pdu_sessions, unsigned num_qos_flows)
{
  cu_cp_pdu_session_resource_setup_request req;
  req.ue_index = uint_to_ue_index(0);

  req.ue_aggregate_maximum_bit_rate_dl = 1000;

  for (unsigned i = 0; i < num_pdu_sessions; ++i) {
    pdu_session_id_t pdu_session_id = uint_to_pdu_session_id(i + 1);

    cu_cp_pdu_session_res_setup_item item;
    item.pdu_session_id = pdu_session_id;
    item.pdu_session_nas_pdu.resize(2);
    item.pdu_session_nas_pdu[0] = 0xaa;
    item.pdu_session_nas_pdu[1] = 0xbb;
    item.s_nssai.sst            = 1;

    item.pdu_session_aggregate_maximum_bit_rate_dl = 100;
    item.pdu_session_aggregate_maximum_bit_rate_ul = 100;
    item.ul_ngu_up_tnl_info                        = {transport_layer_address{"127.0.0.1"}, int_to_gtpu_teid(0x1)};
    item.pdu_session_type                          = "ipv4";
    item.security_ind                              = {};

    for (unsigned k = 0; k < num_qos_flows; ++k) {
      qos_flow_setup_request_item qos_item;
      qos_item.qos_flow_id = uint_to_qos_flow_id(i + k + 1);

      non_dyn_5qi_descriptor_t non_dyn_5qi;
      non_dyn_5qi.five_qi                                                = uint_to_five_qi(9); // all with same FiveQI
      qos_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi = non_dyn_5qi;

      qos_item.qos_flow_level_qos_params.alloc_and_retention_prio.prio_level_arp            = 8;
      qos_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_cap           = "not-pre-emptable";
      qos_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_vulnerability = "not-pre-emptable";

      item.qos_flow_setup_request_items.emplace(qos_item.qos_flow_id, qos_item);
    }

    req.pdu_session_res_setup_items.emplace(pdu_session_id, std::move(item));
  }

  return req;
};

cu_cp_pdu_session_resource_release_command srsran::srs_cu_cp::generate_pdu_session_resource_release()
{
  cu_cp_pdu_session_resource_release_command cmd;
  pdu_session_id_t                           pdu_session_id = uint_to_pdu_session_id(1);

  cmd.ue_index = uint_to_ue_index(0);

  cu_cp_pdu_session_res_to_release_item_rel_cmd pdu_session_res_to_release_item_rel_cmd;
  pdu_session_res_to_release_item_rel_cmd.pdu_session_id                             = pdu_session_id;
  pdu_session_res_to_release_item_rel_cmd.pdu_session_res_release_cmd_transfer.cause = cause_nas_t::unspecified;

  cmd.pdu_session_res_to_release_list_rel_cmd.emplace(pdu_session_id, pdu_session_res_to_release_item_rel_cmd);

  return cmd;
};

cu_cp_pdu_session_resource_modify_request srsran::srs_cu_cp::generate_pdu_session_resource_modification(unsigned psi,
                                                                                                        unsigned qfi)
{
  cu_cp_pdu_session_resource_modify_request request;
  request.ue_index = uint_to_ue_index(0);

  cu_cp_pdu_session_res_modify_item_mod_req modify_item;
  modify_item.pdu_session_id = uint_to_pdu_session_id(psi);

  cu_cp_qos_flow_add_or_mod_item qos_item;
  qos_item.qos_flow_id = uint_to_qos_flow_id(qfi);
  {
    non_dyn_5qi_descriptor_t non_dyn_5qi;
    non_dyn_5qi.five_qi                                                                   = uint_to_five_qi(7);
    qos_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi                    = non_dyn_5qi;
    qos_item.qos_flow_level_qos_params.alloc_and_retention_prio.prio_level_arp            = 8;
    qos_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_cap           = "not-pre-emptable";
    qos_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_vulnerability = "not-pre-emptable";
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
  release_item.cause       = cause_radio_network_t::unspecified;
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
};

e1ap_bearer_context_modification_response
srsran::srs_cu_cp::generate_e1ap_bearer_context_modification_response(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                                      gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id)
{
  e1ap_bearer_context_modification_response resp;

  e1ap_message asn1_res = generate_bearer_context_modification_response(cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);

  fill_e1ap_bearer_context_modification_response(resp,
                                                 asn1_res.pdu.successful_outcome().value.bearer_context_mod_resp());

  return resp;
};

f1ap_ue_context_modification_response
srsran::srs_cu_cp::generate_f1ap_ue_context_modification_response(gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id,
                                                                  gnb_du_ue_f1ap_id_t du_ue_f1ap_id)
{
  f1ap_ue_context_modification_response resp;

  f1ap_message asn1_res = generate_ue_context_modification_response(cu_ue_f1ap_id, du_ue_f1ap_id);

  fill_f1ap_ue_context_modification_response(resp, asn1_res.pdu.successful_outcome().value.ue_context_mod_resp());

  return resp;
};
