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

#include "cu_cp_test_helpers.h"
#include "srsran/e1ap/common/e1ap_common.h"
#include "srsran/ran/cu_types.h"
#include <utility>

using namespace srsran;
using namespace srs_cu_cp;

cu_cp_test::cu_cp_test()
{
  test_logger.set_level(srslog::basic_levels::debug);
  cu_cp_logger.set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("NGAP").set_hex_dump_max_size(32);
  srslog::fetch_basic_logger("RRC").set_hex_dump_max_size(32);
  srslog::fetch_basic_logger("SEC").set_hex_dump_max_size(32);
  srslog::fetch_basic_logger("PDCP").set_hex_dump_max_size(32);
  srslog::init();

  // create CU-CP config
  cu_cp_configuration cfg;
  cfg.cu_cp_executor = &ctrl_worker;
  cfg.e1ap_notifier  = &e1ap_pdu_notifier;
  cfg.ngap_notifier  = &ngap_amf_notifier;

  cfg.ngap_config.gnb_id        = 411;
  cfg.ngap_config.ran_node_name = "srsgnb01";
  cfg.ngap_config.plmn          = "00101";
  cfg.ngap_config.tac           = 7;
  s_nssai_t slice_cfg;
  slice_cfg.sst = 1;
  cfg.ngap_config.slice_configurations.push_back(slice_cfg);

  // create and start DUT
  cu_cp_obj = std::make_unique<cu_cp_impl>(std::move(cfg));
  cu_cp_obj->start();

  // Attach F1-C gateway to CU-CP
  f1c_gw.attach_cu_cp_du_repo(cu_cp_obj->get_connected_dus());
}

cu_cp_test::~cu_cp_test()
{
  // flush logger after each test
  srslog::flush();
  cu_cp_obj->stop();
}

void cu_cp_test::attach_ue(gnb_du_ue_f1ap_id_t du_ue_id,
                           gnb_cu_ue_f1ap_id_t cu_ue_id,
                           rnti_t              crnti,
                           du_index_t          du_index)
{
  // Inject Initial UL RRC message
  f1ap_message init_ul_rrc_msg = generate_init_ul_rrc_message_transfer(du_ue_id, crnti);
  test_logger.info("Injecting Initial UL RRC message");
  cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(init_ul_rrc_msg);

  // Inject UL RRC message containing RRC Setup Complete
  f1ap_message ul_rrc_msg =
      generate_ul_rrc_message_transfer(cu_ue_id, du_ue_id, srb_id_t::srb1, generate_rrc_setup_complete());
  test_logger.info("Injecting UL RRC message (RRC Setup Complete)");
  cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(ul_rrc_msg);
}

void cu_cp_test::authenticate_ue(amf_ue_id_t         amf_ue_id,
                                 ran_ue_id_t         ran_ue_id,
                                 du_index_t          du_index,
                                 gnb_du_ue_f1ap_id_t du_ue_id,
                                 gnb_cu_ue_f1ap_id_t cu_ue_id)
{
  // Inject NGAP DL message (authentication request)
  ngap_message dl_nas_transport = generate_downlink_nas_transport_message(amf_ue_id, ran_ue_id);
  cu_cp_obj->get_ngap_message_handler().handle_message(dl_nas_transport);

  // Inject UL RRC msg transfer (authentication response)
  f1ap_message ul_rrc_msg_transfer = generate_ul_rrc_message_transfer(
      cu_ue_id,
      du_ue_id,
      srb_id_t::srb1,
      make_byte_buffer("00013a0abf002b96883cf61fddc6e2b603bc28210b27c8605b8000000000"));
  cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(ul_rrc_msg_transfer);

  // Inject DL NAS Transport message (ue security mode command)
  dl_nas_transport = generate_downlink_nas_transport_message(amf_ue_id, ran_ue_id);
  cu_cp_obj->get_ngap_message_handler().handle_message(dl_nas_transport);

  // Inject UL RRC msg transfer (ue security mode complete)
  ul_rrc_msg_transfer = generate_ul_rrc_message_transfer(
      cu_ue_id,
      du_ue_id,
      srb_id_t::srb1,
      make_byte_buffer("00023a1cbf026753e33a003f002f3b80048290a1b283800000f8b880103f0020bc800680807888787f800008192a3b4"
                       "c080080170170700c0080a980808000000000"));
  cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(ul_rrc_msg_transfer);
}

void cu_cp_test::setup_security(amf_ue_id_t         amf_ue_id,
                                ran_ue_id_t         ran_ue_id,
                                du_index_t          du_index,
                                gnb_du_ue_f1ap_id_t du_ue_id,
                                gnb_cu_ue_f1ap_id_t cu_ue_id)
{
  // Inject Initial Context Setup Request
  ngap_message init_ctxt_setup_req = generate_valid_initial_context_setup_request_message(amf_ue_id, ran_ue_id);
  cu_cp_obj->get_ngap_message_handler().handle_message(init_ctxt_setup_req);

  // Inject Security Mode Complete
  f1ap_message ul_rrc_msg_transfer =
      generate_ul_rrc_message_transfer(cu_ue_id, du_ue_id, srb_id_t::srb1, make_byte_buffer("00032a00fd5ec7ff"));
  cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(ul_rrc_msg_transfer);
}

void cu_cp_test::test_preamble_ue_creation(du_index_t          du_index,
                                           gnb_du_ue_f1ap_id_t du_ue_id,
                                           gnb_cu_ue_f1ap_id_t cu_ue_id,
                                           pci_t               pci,
                                           rnti_t              crnti,
                                           amf_ue_id_t         amf_ue_id,
                                           ran_ue_id_t         ran_ue_id)
{
  // Connect AMF by injecting a ng_setup_response
  ngap_message ngap_msg = generate_ng_setup_response();
  cu_cp_obj->get_ngap_message_handler().handle_message(ngap_msg);

  ASSERT_TRUE(cu_cp_obj->amf_is_connected());

  // Create a new DU connection to the CU-CP, creating a new DU processor in the CU-CP in the process.
  f1c_gw.request_new_du_connection();
  ASSERT_EQ(f1c_gw.nof_connections(), 1U);
  ASSERT_EQ(cu_cp_obj->get_connected_dus().get_nof_dus(), 1U);

  // Connect CU-UP
  cu_cp_obj->handle_new_cu_up_connection();
  ASSERT_EQ(cu_cp_obj->get_nof_cu_ups(), 1U);

  // Generate F1SetupRequest
  f1ap_message f1setup_msg = generate_f1_setup_request(0x11, 6576, pci);

  // Pass message to CU-CP
  cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(f1setup_msg);

  // Attach UE
  attach_ue(du_ue_id, cu_ue_id, crnti, du_index);
  ASSERT_EQ(cu_cp_obj->get_connected_dus().get_nof_ues(), 1U);

  authenticate_ue(amf_ue_id, ran_ue_id, du_index, du_ue_id, cu_ue_id);

  setup_security(amf_ue_id, ran_ue_id, du_index, du_ue_id, cu_ue_id);
}

bool cu_cp_test::check_minimal_paging_result()
{
  span<const f1ap_message> last_f1ap_msgs = f1c_gw.last_tx_pdus(0);
  if (last_f1ap_msgs.empty() or last_f1ap_msgs.back().pdu.type() != asn1::f1ap::f1ap_pdu_c::types::init_msg ||
      last_f1ap_msgs.back().pdu.init_msg().value.type() != asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types::paging) {
    return false;
  }

  auto& paging_msg = last_f1ap_msgs.back().pdu.init_msg().value.paging();

  // check ue id idx value
  if (paging_msg->ue_id_idx_value.idx_len10().to_number() != (279089024671 % 1024)) {
    test_logger.error("UE ID idx value mismatch {} != {}",
                      paging_msg->ue_id_idx_value.idx_len10().to_number(),
                      (279089024671 % 1024));
    return false;
  }

  // check paging id
  if (paging_msg->paging_id.cn_ue_paging_id().five_g_s_tmsi().to_number() != 279089024671) {
    test_logger.error("Paging ID mismatch {} != {}",
                      paging_msg->paging_id.cn_ue_paging_id().five_g_s_tmsi().to_number(),
                      279089024671);
    return false;
  }

  // check paging cell list
  if (paging_msg->paging_cell_list.size() != 1) {
    test_logger.error("Paging cell list size mismatch {} != {}", paging_msg->paging_cell_list.size(), 1);
    return false;
  }
  auto& paging_cell_item = paging_msg->paging_cell_list[0].value().paging_cell_item();
  if (paging_cell_item.nr_cgi.nr_cell_id.to_number() != 6576) {
    test_logger.error("NR CGI NCI mismatch {} != {}", paging_cell_item.nr_cgi.nr_cell_id.to_number(), 6576);
    return false;
  }
  if (paging_cell_item.nr_cgi.plmn_id.to_string() != "00f110") {
    test_logger.error("NR CGI PLMN mismatch {} != 00f110", paging_cell_item.nr_cgi.plmn_id.to_string());
    return false;
  }

  return true;
}

bool cu_cp_test::check_paging_result()
{
  if (!check_minimal_paging_result()) {
    return false;
  }

  span<const f1ap_message> last_f1ap_msgs = f1c_gw.last_tx_pdus(0);
  if (last_f1ap_msgs.empty()) {
    test_logger.error("No F1AP messages sent");
    return false;
  }
  auto& paging_msg = last_f1ap_msgs.back().pdu.init_msg().value.paging();

  // check paging drx
  if (!paging_msg->paging_drx_present) {
    return false;
  }
  if (paging_msg->paging_drx.to_number() != 64) {
    test_logger.error("Paging DRX mismatch {} != {}", paging_msg->paging_drx.to_number(), 64);
    return false;
  }

  // check paging prio
  if (!paging_msg->paging_prio_present) {
    return false;
  }
  if (paging_msg->paging_prio.to_number() != 5) {
    test_logger.error("Paging prio mismatch {} != {}", paging_msg->paging_prio.to_number(), 5);
    return false;
  }

  // check paging origin
  if (!paging_msg->paging_origin_present) {
    return false;
  }
  if ((std::string)paging_msg->paging_origin.to_string() != "non-3gpp") {
    test_logger.error("Paging origin mismatch {} != non-3gpp", paging_msg->paging_origin.to_string());
    return false;
  }

  return true;
}
