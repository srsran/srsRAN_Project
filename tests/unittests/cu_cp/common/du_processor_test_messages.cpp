/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_processor_test_messages.h"
#include "lib/e1ap/cu_cp/e1ap_cu_cp_asn1_helpers.h"
#include "lib/f1ap/common/asn1_helpers.h"
#include "lib/f1ap/cu_cp/f1ap_asn1_helpers.h"
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "tests/unittests/f1ap/common/f1ap_cu_test_messages.h"

using namespace srsgnb;
using namespace srs_cu_cp;

f1_setup_request_message srsgnb::srs_cu_cp::generate_valid_f1_setup_request()
{
  f1ap_message             f1setup_msg          = generate_f1_setup_request();
  f1_setup_request_message f1_setup_request_msg = {};
  f1_setup_request_msg.request                  = f1setup_msg.pdu.init_msg().value.f1_setup_request();
  return f1_setup_request_msg;
}

f1_setup_request_message srsgnb::srs_cu_cp::generate_f1_setup_request_base()
{
  f1ap_message f1setup_msg                                                              = generate_f1_setup_request();
  f1setup_msg.pdu.init_msg().value.f1_setup_request()->gnb_du_served_cells_list_present = false;
  f1setup_msg.pdu.init_msg().value.f1_setup_request()->gnb_du_served_cells_list->clear();
  f1_setup_request_message f1_setup_request_msg = {};
  f1_setup_request_msg.request                  = f1setup_msg.pdu.init_msg().value.f1_setup_request();
  return f1_setup_request_msg;
}

ue_creation_message srsgnb::srs_cu_cp::generate_ue_creation_message(rnti_t c_rnti, unsigned nrcell_id)
{
  ue_creation_message ue_creation_msg = {};
  ue_creation_msg.c_rnti              = c_rnti;
  asn1::f1ap::nr_cgi_s asn1_cgi;
  asn1_cgi.nr_cell_id.from_number(nrcell_id);
  asn1_cgi.plmn_id.from_string("02f899");
  ue_creation_msg.cgi = cgi_from_asn1(asn1_cgi);
  ue_creation_msg.du_to_cu_rrc_container.from_string(
      "5c00b001117aec701061e0007c20408d07810020a2090480ca8000f800000000008370842000088165000048200002069a06aa49880002"
      "00204000400d008013b64b1814400e468acf120000096070820f177e060870000000e25038000040bde802000400000000028201950300"
      "c400");

  return ue_creation_msg;
}

cu_cp_ue_context_release_command srsgnb::srs_cu_cp::generate_ue_context_release_command(ue_index_t ue_index)
{
  cu_cp_ue_context_release_command ue_context_release_command = {};
  ue_context_release_command.ue_index                         = ue_index;
  ue_context_release_command.cause                            = cause_t::radio_network;
  return ue_context_release_command;
}

cu_cp_pdu_session_resource_setup_request srsgnb::srs_cu_cp::generate_pdu_session_resource_setup()
{
  cu_cp_pdu_session_resource_setup_request req;
  pdu_session_id_t                         pdu_session_id = uint_to_pdu_session_id(1);

  req.ue_index = uint_to_ue_index(0);

  cu_cp_pdu_session_res_setup_item item;
  item.pdu_session_id = pdu_session_id;
  item.pdu_session_nas_pdu.resize(2);
  item.pdu_session_nas_pdu[0] = 0xaa;
  item.pdu_session_nas_pdu[1] = 0xbb;
  item.s_nssai.sst            = 1;

  item.pdu_session_aggregate_maximum_bit_rate_dl = 100;
  item.pdu_session_aggregate_maximum_bit_rate_ul = 100;
  item.ul_ngu_up_tnl_info                        = {transport_layer_address{"127.0.0.1"}, int_to_gtp_teid(0x1)};
  item.pdu_session_type                          = "ipv4";

  qos_flow_setup_request_item qos_item;
  qos_item.qos_flow_id                                   = uint_to_qos_flow_id(1);
  qos_item.qos_characteristics.is_dynamic_5qi            = false;
  qos_item.qos_characteristics.five_qi                   = 9;
  qos_item.qos_characteristics.prio_level_arp            = 8;
  qos_item.qos_characteristics.pre_emption_cap           = "not-pre-emptable";
  qos_item.qos_characteristics.pre_emption_vulnerability = "not-pre-emptable";

  item.qos_flow_setup_request_items.emplace(qos_item.qos_flow_id, qos_item);

  req.pdu_session_res_setup_items.emplace(pdu_session_id, std::move(item));

  return req;
};

e1ap_bearer_context_setup_response
srsgnb::srs_cu_cp::generate_e1ap_bearer_context_setup_response(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                               gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id)
{
  e1ap_bearer_context_setup_response resp;

  e1ap_message asn1_res = generate_bearer_context_setup_response(cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);

  fill_e1ap_bearer_context_setup_response(resp, asn1_res.pdu.successful_outcome().value.bearer_context_setup_resp());

  return resp;
};

e1ap_bearer_context_modification_response
srsgnb::srs_cu_cp::generate_e1ap_bearer_context_modification_response(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                                      gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id)
{
  e1ap_bearer_context_modification_response resp;

  e1ap_message asn1_res = generate_bearer_context_modification_response(cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);

  fill_e1ap_bearer_context_modification_response(resp,
                                                 asn1_res.pdu.successful_outcome().value.bearer_context_mod_resp());

  return resp;
};

cu_cp_ue_context_modification_response
srsgnb::srs_cu_cp::generate_cu_cp_ue_context_modification_response(gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id,
                                                                   gnb_du_ue_f1ap_id_t du_ue_f1ap_id)
{
  cu_cp_ue_context_modification_response resp;

  f1ap_message asn1_res = generate_ue_context_modification_response(cu_ue_f1ap_id, du_ue_f1ap_id);

  fill_f1ap_ue_context_modification_response_message(resp,
                                                     asn1_res.pdu.successful_outcome().value.ue_context_mod_resp());

  return resp;
};
