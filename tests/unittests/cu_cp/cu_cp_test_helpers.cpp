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

#include "cu_cp_test_helpers.h"
#include "srsran/cu_cp/cu_cp_configuration_helpers.h"
#include "srsran/e1ap/common/e1ap_common.h"
#include "srsran/ran/cu_types.h"
#include <chrono>
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
  cu_cp_configuration cfg = config_helpers::make_default_cu_cp_config();
  cfg.cu_cp_executor      = &ctrl_worker;
  cfg.ngap_notifier       = &ngap_amf_notifier;
  cfg.timers              = timers.get();

  // NGAP config
  cfg.ngap_config.gnb_id        = 411;
  cfg.ngap_config.ran_node_name = "srsgnb01";
  cfg.ngap_config.plmn          = "00101";
  cfg.ngap_config.tac           = 7;
  s_nssai_t slice_cfg;
  slice_cfg.sst = 1;
  cfg.ngap_config.slice_configurations.push_back(slice_cfg);

  // RRC config
  cfg.rrc_config.drb_config         = config_helpers::make_default_cu_cp_qos_config_list();
  cfg.rrc_config.int_algo_pref_list = {security::integrity_algorithm::nia2,
                                       security::integrity_algorithm::nia1,
                                       security::integrity_algorithm::nia3,
                                       security::integrity_algorithm::nia0};
  cfg.rrc_config.enc_algo_pref_list = {security::ciphering_algorithm::nea0,
                                       security::ciphering_algorithm::nea2,
                                       security::ciphering_algorithm::nea1,
                                       security::ciphering_algorithm::nea3};

  // UE config
  cfg.ue_config.inactivity_timer = std::chrono::seconds{7200};

  // periodic statistic logging
  cfg.statistics_report_period = std::chrono::seconds(1);

  // mobility config
  cfg.mobility_config.mobility_manager_config.trigger_handover_from_measurements = true;
  rrc_report_cfg_nr periodic_rep_cfg                                             = {};
  periodic_rep_cfg.periodical.emplace();
  periodic_rep_cfg.periodical.value().report_interv = 1024;
  cfg.mobility_config.meas_manager_config.report_config_ids.emplace(uint_to_report_cfg_id(1), periodic_rep_cfg);
  rrc_report_cfg_nr ev_triggered_rep_cfg = {};
  ev_triggered_rep_cfg.event_triggered.emplace();
  ev_triggered_rep_cfg.event_triggered.value().event_id.event_a3.emplace();
  ev_triggered_rep_cfg.event_triggered.value().event_id.event_a3.value().a3_offset.rsrp.emplace(6);
  ev_triggered_rep_cfg.event_triggered.value().event_id.event_a3.value().hysteresis      = 0;
  ev_triggered_rep_cfg.event_triggered.value().event_id.event_a3.value().time_to_trigger = 100;
  ev_triggered_rep_cfg.event_triggered.value().report_interv                             = 1024;
  cfg.mobility_config.meas_manager_config.report_config_ids.emplace(uint_to_report_cfg_id(2), ev_triggered_rep_cfg);

  cell_meas_config cell_cfg_1;
  cell_cfg_1.periodic_report_cfg_id = uint_to_report_cfg_id(1);
  neighbor_cell_meas_config ncell_1;
  ncell_1.nci = 0x19c0;
  ncell_1.report_cfg_ids.push_back(uint_to_report_cfg_id(2));
  cell_cfg_1.ncells.push_back(ncell_1);
  cell_cfg_1.serving_cell_cfg.nci = 0x19b0;
  cfg.mobility_config.meas_manager_config.cells.emplace(0x19b0, cell_cfg_1);

  cell_meas_config cell_cfg_2;
  cell_cfg_2.periodic_report_cfg_id = uint_to_report_cfg_id(1);
  neighbor_cell_meas_config ncell_2;
  ncell_2.nci = 0x19b0;
  ncell_2.report_cfg_ids.push_back(uint_to_report_cfg_id(2));
  cell_cfg_2.ncells.push_back(ncell_2);
  cell_cfg_2.serving_cell_cfg.nci       = 0x19c0;
  cell_cfg_2.serving_cell_cfg.gnb_id    = 412;
  cell_cfg_2.serving_cell_cfg.ssb_arfcn = 632628;
  cell_cfg_2.serving_cell_cfg.band      = nr_band::n78;
  cell_cfg_2.serving_cell_cfg.ssb_scs   = subcarrier_spacing::kHz30;
  cell_cfg_2.serving_cell_cfg.ssb_mtc.emplace();
  cell_cfg_2.serving_cell_cfg.ssb_mtc.value().periodicity_and_offset.sf20.emplace(0);
  cell_cfg_2.serving_cell_cfg.ssb_mtc.value().dur = 5;
  cfg.mobility_config.meas_manager_config.cells.emplace(0x19c0, cell_cfg_2);

  // create and start CU-CP.
  cu_cp_obj = std::make_unique<cu_cp_impl>(std::move(cfg));
  cu_cp_obj->handle_amf_connection();
  cu_cp_obj->start();

  // Attach F1-C gateway to CU-CP
  f1c_gw.attach_cu_cp_du_repo(cu_cp_obj->get_connected_dus());

  // Attach E1AP gateway to CU-CP
  e1ap_gw.attach_cu_cp_cu_up_repo(cu_cp_obj->get_connected_cu_ups());
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
      make_byte_buffer("00013a0abf002b96882dac46355c4f34464ddaf7b43fde37ae8000000000"));
  cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(ul_rrc_msg_transfer);

  // Inject DL NAS Transport message (ue security mode command)
  dl_nas_transport = generate_downlink_nas_transport_message(amf_ue_id, ran_ue_id);
  cu_cp_obj->get_ngap_message_handler().handle_message(dl_nas_transport);

  // Inject UL RRC msg transfer (ue security mode complete)
  ul_rrc_msg_transfer = generate_ul_rrc_message_transfer(
      cu_ue_id,
      du_ue_id,
      srb_id_t::srb1,
      make_byte_buffer("00023a1cbf0243241cb5003f002f3b80048290a1b283800000f8b880103f0020bc800680807888787f800008192a3b4"
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

void cu_cp_test::test_amf_connection()
{
  // Connect AMF by injecting a ng_setup_response
  ngap_message ngap_msg = generate_ng_setup_response();
  cu_cp_obj->get_ngap_message_handler().handle_message(ngap_msg);

  ASSERT_TRUE(cu_cp_obj->amf_is_connected());
}

void cu_cp_test::test_e1ap_attach()
{
  // Create a new CU-UP connection to the CU-CP, creating a new CU-UP processor in the CU-CP in the process.
  e1ap_gw.request_new_cu_up_connection();
  ASSERT_EQ(e1ap_gw.nof_connections(), 1U);
  ASSERT_EQ(cu_cp_obj->get_connected_cu_ups().get_nof_cu_ups(), 1U);

  // Pass E1SetupRequest to the CU-CP
  e1ap_message e1setup_msg = generate_valid_cu_up_e1_setup_request();
  cu_cp_obj->get_connected_cu_ups()
      .get_cu_up(uint_to_cu_up_index(0))
      .get_e1ap_message_handler()
      .handle_message(e1setup_msg);
}

void cu_cp_test::test_du_attach(du_index_t du_index, unsigned gnb_du_id, unsigned nrcell_id, pci_t pci)
{
  // Store current number of DUs.
  size_t nof_dus = cu_cp_obj->get_connected_dus().get_nof_dus();

  // Create a new DU connection to the CU-CP, creating a new DU processor in the CU-CP in the process.
  f1c_gw.request_new_du_connection();

  size_t expected_nof_dus = nof_dus + 1;
  ASSERT_EQ(f1c_gw.nof_connections(), expected_nof_dus);
  ASSERT_EQ(cu_cp_obj->get_connected_dus().get_nof_dus(), expected_nof_dus);

  // Pass F1SetupRequest to the CU-CP
  f1ap_message f1setup_msg = generate_f1_setup_request(gnb_du_id, nrcell_id, pci);
  cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(f1setup_msg);
}

void cu_cp_test::test_preamble_all_connected(du_index_t du_index, pci_t pci)
{
  test_amf_connection();

  test_e1ap_attach();

  test_du_attach(du_index, 0x11, 6576, pci);
}

void cu_cp_test::test_preamble_ue_creation(du_index_t          du_index,
                                           gnb_du_ue_f1ap_id_t du_ue_id,
                                           gnb_cu_ue_f1ap_id_t cu_ue_id,
                                           pci_t               pci,
                                           rnti_t              crnti,
                                           amf_ue_id_t         amf_ue_id,
                                           ran_ue_id_t         ran_ue_id)
{
  // Connect AMF, DU, CU-UP
  test_preamble_all_connected(du_index, pci);

  // Attach UE
  attach_ue(du_ue_id, cu_ue_id, crnti, du_index);
  ASSERT_EQ(cu_cp_obj->get_connected_dus().get_nof_ues(), 1U);

  authenticate_ue(amf_ue_id, ran_ue_id, du_index, du_ue_id, cu_ue_id);

  setup_security(amf_ue_id, ran_ue_id, du_index, du_ue_id, cu_ue_id);
}

void cu_cp_test::test_preamble_ue_full_attach(du_index_t             du_index,
                                              gnb_du_ue_f1ap_id_t    du_ue_id,
                                              gnb_cu_ue_f1ap_id_t    cu_ue_id,
                                              pci_t                  pci,
                                              rnti_t                 crnti,
                                              amf_ue_id_t            amf_ue_id,
                                              ran_ue_id_t            ran_ue_id,
                                              gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                              gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id)
{
  // Create UE
  test_preamble_ue_creation(du_index, du_ue_id, cu_ue_id, pci, crnti, amf_ue_id, ran_ue_id);

  // Inject Registration Complete
  f1ap_message ul_rrc_msg_transfer = generate_ul_rrc_message_transfer(
      cu_ue_id, du_ue_id, srb_id_t::srb1, make_byte_buffer("00043a053f015362c51680bf00218003fe6db7"));
  cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(ul_rrc_msg_transfer);

  // Inject PDU Session Establishment Request
  ul_rrc_msg_transfer = generate_ul_rrc_message_transfer(
      cu_ue_id,
      du_ue_id,
      srb_id_t::srb1,
      make_byte_buffer("00053a253f011ffa9203013f0033808018970080e0ffffc9d8bd8013404010880080000840830000000041830000000"
                       "00000800001800005000006000006800008800900c092838339b939b0b837002c98dcab"));
  cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(ul_rrc_msg_transfer);

  // Inject Configuration Update Command
  ngap_message dl_nas_transport_msg = generate_downlink_nas_transport_message(
      amf_ue_id,
      ran_ue_id,
      make_byte_buffer("7e0205545bfc027e0054430f90004f00700065006e00350047005346004732800131235200490100"));
  cu_cp_obj->get_ngap_message_handler().handle_message(dl_nas_transport_msg);

  // Inject PDU Session Resource Setup Request
  ngap_message pdu_session_resource_setup_request =
      generate_valid_pdu_session_resource_setup_request_message(amf_ue_id, ran_ue_id, uint_to_pdu_session_id(1));
  cu_cp_obj->get_ngap_message_handler().handle_message(pdu_session_resource_setup_request);

  // check that the UE capability enquiry was sent to the DU
  ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.type(), asn1::f1ap::f1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::dl_rrc_msg_transfer);

  // Inject UE capability info
  ul_rrc_msg_transfer = generate_ul_rrc_message_transfer(
      cu_ue_id,
      du_ue_id,
      srb_id_t::srb1,
      make_byte_buffer("00064c821930680ce811d1968097e340e1480005824c5c00060fc2c00637fe002e00131401a0000000880058d006007"
                       "a071e439f0000240400e0300000000100186c0000700809df0000000000000103a0002000012cb2800281c50f000700"
                       "0f00000004008010240a00126cc3c6"));
  cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(ul_rrc_msg_transfer);

  // check that the Bearer Context Setup was sent to the CU-UP
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_setup_request);

  // Inject Bearer Context Setup Response
  e1ap_message bearer_context_setup_resp = generate_bearer_context_setup_response(cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);
  cu_cp_obj->get_cu_cp_cu_up_connection_interface()
      .get_e1ap_message_handler(uint_to_cu_up_index(0))
      .handle_message(bearer_context_setup_resp);

  // check that the UE Context Modification Request was sent to the DU
  ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.type(), asn1::f1ap::f1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_mod_request);

  // Inject UE Context Modification Response
  f1ap_message ue_context_mod_resp = generate_ue_context_modification_response(cu_ue_id, du_ue_id);
  cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(ue_context_mod_resp);

  // check that the Bearer Context Modification was sent to the CU-UP
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_mod_request);

  // Inject Bearer Context Modification Response
  e1ap_message bearer_context_mod_resp =
      generate_bearer_context_modification_response(cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);
  cu_cp_obj->get_cu_cp_cu_up_connection_interface()
      .get_e1ap_message_handler(uint_to_cu_up_index(0))
      .handle_message(bearer_context_mod_resp);

  // check that the RRC Reconfiguration was sent to the DU
  ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.type(), asn1::f1ap::f1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::dl_rrc_msg_transfer);

  // Inject RRC Reconfiguration Complete
  ul_rrc_msg_transfer =
      generate_ul_rrc_message_transfer(cu_ue_id, du_ue_id, srb_id_t::srb1, make_byte_buffer("00070e00cc6fcda5"));
  cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(ul_rrc_msg_transfer);

  // check that the PDU Session Resource Setup Response was sent to the AMF
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.type(),
            asn1::ngap::ngap_pdu_c::types_opts::options::successful_outcome);
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msgs.back().pdu.successful_outcome().value.type().value,
            asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::pdu_session_res_setup_resp);
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
