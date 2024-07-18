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
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/cu_cp/cu_cp_configuration_helpers.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/cu_types.h"
#include "srsran/support/executors/task_worker.h"
#include <chrono>
#include <utility>

using namespace srsran;
using namespace srs_cu_cp;

namespace {

struct amf_test_stub final : public ngap_message_handler {
  amf_test_stub(ngap_message_handler& cu_cp_) : cu_cp(cu_cp_) {}

  void handle_message(const ngap_message& msg) override
  {
    if (is_pdu_type(msg, asn1::ngap::ngap_elem_procs_o::init_msg_c::types::ng_setup_request)) {
      // Generate fake NG Setup Response.
      cu_cp.handle_message(generate_ng_setup_response());
    }
  }

private:
  ngap_message_handler& cu_cp;
};

} // namespace

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
  cu_cp_configuration cfg     = config_helpers::make_default_cu_cp_config();
  cfg.services.cu_cp_executor = &ctrl_worker;
  cfg.services.n2_gw          = &n2_gw;
  cfg.services.timers         = &timers;
  // mobility config
  cfg.mobility.mobility_manager_config.trigger_handover_from_measurements = true;
  // Generate NCIs.
  gnb_id_t         gnb_id1 = cfg.node.gnb_id;
  nr_cell_identity nci1    = nr_cell_identity::create(gnb_id1, 0).value();
  nr_cell_identity nci2    = nr_cell_identity::create(gnb_id1, 1).value();
  gnb_id_t         gnb_id2 = {cfg.node.gnb_id.id + 1, cfg.node.gnb_id.bit_length};
  nr_cell_identity nci3    = nr_cell_identity::create(gnb_id2, 0).value();

  cell_meas_config cell_cfg_1;
  cell_cfg_1.periodic_report_cfg_id             = uint_to_report_cfg_id(1);
  cell_cfg_1.serving_cell_cfg.gnb_id_bit_length = gnb_id1.bit_length;
  cell_cfg_1.serving_cell_cfg.nci               = nci1;
  cell_cfg_1.ncells.push_back({nci2, {uint_to_report_cfg_id(2)}});
  // Add external cell (for inter CU handover tests)
  cell_cfg_1.ncells.push_back({nci3, {uint_to_report_cfg_id(2)}});
  cfg.mobility.meas_manager_config.cells.emplace(nci1, cell_cfg_1);

  cell_meas_config cell_cfg_2;
  cell_cfg_2.periodic_report_cfg_id             = uint_to_report_cfg_id(1);
  cell_cfg_2.serving_cell_cfg.gnb_id_bit_length = gnb_id1.bit_length;
  cell_cfg_2.serving_cell_cfg.nci               = nci2;
  cell_cfg_2.ncells.push_back({nci1, {uint_to_report_cfg_id(2)}});
  cfg.mobility.meas_manager_config.cells.emplace(nci2, cell_cfg_2);

  // Add an external cell
  cell_meas_config cell_cfg_3;
  cell_cfg_3.periodic_report_cfg_id             = uint_to_report_cfg_id(1);
  cell_cfg_3.serving_cell_cfg.gnb_id_bit_length = gnb_id2.bit_length;
  cell_cfg_3.serving_cell_cfg.nci               = nci3;
  cell_cfg_3.serving_cell_cfg.pci               = 3;
  cell_cfg_3.serving_cell_cfg.ssb_arfcn         = 632628;
  cell_cfg_3.serving_cell_cfg.band              = nr_band::n78;
  cell_cfg_3.serving_cell_cfg.ssb_scs           = subcarrier_spacing::kHz15;
  cell_cfg_3.serving_cell_cfg.ssb_mtc           = rrc_ssb_mtc{{rrc_periodicity_and_offset::periodicity_t::sf20, 0}, 5};

  cell_cfg_3.ncells.push_back({nci1, {uint_to_report_cfg_id(2)}});
  cfg.mobility.meas_manager_config.cells.emplace(nci3, cell_cfg_3);

  // Add periodic event
  rrc_report_cfg_nr periodic_report_cfg;
  auto&             periodical_cfg = periodic_report_cfg.periodical.emplace();

  periodical_cfg.rs_type                = srs_cu_cp::rrc_nr_rs_type::ssb;
  periodical_cfg.report_interv          = 1024;
  periodical_cfg.report_amount          = -1;
  periodical_cfg.report_quant_cell.rsrp = true;
  periodical_cfg.report_quant_cell.rsrq = true;
  periodical_cfg.report_quant_cell.sinr = true;
  periodical_cfg.max_report_cells       = 4;

  periodic_report_cfg.periodical = periodical_cfg;
  cfg.mobility.meas_manager_config.report_config_ids.emplace(uint_to_report_cfg_id(1), periodic_report_cfg);

  rrc_report_cfg_nr a3_report_cfg;
  auto&             event_trigger_cfg = a3_report_cfg.event_triggered.emplace();
  auto&             event_a3          = a3_report_cfg.event_triggered.value().event_id.event_a3.emplace();

  event_a3.a3_offset.rsrp.emplace() = 6;
  event_a3.hysteresis               = 0;
  event_a3.time_to_trigger          = 100;

  event_trigger_cfg.rs_type                = srs_cu_cp::rrc_nr_rs_type::ssb;
  event_trigger_cfg.report_interv          = 1024;
  event_trigger_cfg.report_amount          = -1;
  event_trigger_cfg.report_quant_cell.rsrp = true;
  event_trigger_cfg.report_quant_cell.rsrq = true;
  event_trigger_cfg.report_quant_cell.sinr = true;
  event_trigger_cfg.max_report_cells       = 4;

  rrc_meas_report_quant report_quant_rs_idxes;
  report_quant_rs_idxes.rsrp              = true;
  report_quant_rs_idxes.rsrq              = true;
  report_quant_rs_idxes.sinr              = true;
  event_trigger_cfg.report_quant_rs_idxes = report_quant_rs_idxes;

  a3_report_cfg.event_triggered = event_trigger_cfg;
  cfg.mobility.meas_manager_config.report_config_ids.emplace(uint_to_report_cfg_id(2), a3_report_cfg);

  // create CU-CP.
  cu_cp_obj = std::make_unique<cu_cp_impl>(std::move(cfg));

  // Connect CU-CP to AMF stub.
  dummy_amf = std::make_unique<amf_test_stub>(cu_cp_obj->get_ngap_message_handler());
  n2_gw.attach_handler(dummy_amf.get());

  // Start CU-CP.
  cu_cp_obj->start();

  // Attach F1-C gateway to CU-CP
  f1c_gw.attach_cu_cp_du_repo(cu_cp_obj->get_f1c_handler());

  // Attach E1AP gateway to CU-CP
  e1ap_gw.attach_cu_cp_cu_up_repo(cu_cp_obj->get_e1_handler());
}

cu_cp_test::~cu_cp_test()
{
  std::promise<void> p;
  std::future<void>  f = p.get_future();

  // cu_cp->stop() should not be called from same execution context as of the CU-CP executor.
  task_worker stop_worker{"stop_worker", 128};
  bool        res = stop_worker.push_task([this, &p]() {
    cu_cp_obj->stop();
    p.set_value();
  });
  report_fatal_error_if_not(res, "Task was not dispatched");

  start_auto_tick(f);

  // flush logger after each test
  srslog::flush();
}

void cu_cp_test::attach_ue(gnb_du_ue_f1ap_id_t du_ue_id,
                           gnb_cu_ue_f1ap_id_t cu_ue_id,
                           rnti_t              crnti,
                           du_index_t          du_index)
{
  // Inject Initial UL RRC message
  f1ap_message init_ul_rrc_msg = generate_init_ul_rrc_message_transfer(du_ue_id, crnti);
  test_logger.info("Injecting Initial UL RRC message");
  f1c_gw.get_du(du_index).on_new_message(init_ul_rrc_msg);

  // Inject UL RRC message containing RRC Setup Complete
  f1ap_message ul_rrc_msg =
      generate_ul_rrc_message_transfer(cu_ue_id, du_ue_id, srb_id_t::srb1, generate_rrc_setup_complete());
  test_logger.info("Injecting UL RRC message (RRC Setup Complete)");
  f1c_gw.get_du(du_index).on_new_message(ul_rrc_msg);
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
      make_byte_buffer("00013a0abf002b96882dac46355c4f34464ddaf7b43fde37ae8000000000").value());
  f1c_gw.get_du(du_index).on_new_message(ul_rrc_msg_transfer);

  // Inject DL NAS Transport message (ue security mode command)
  dl_nas_transport = generate_downlink_nas_transport_message(amf_ue_id, ran_ue_id);
  cu_cp_obj->get_ngap_message_handler().handle_message(dl_nas_transport);

  // Inject UL RRC msg transfer (ue security mode complete)
  ul_rrc_msg_transfer = generate_ul_rrc_message_transfer(
      cu_ue_id,
      du_ue_id,
      srb_id_t::srb1,
      make_byte_buffer("00023a1cbf0243241cb5003f002f3b80048290a1b283800000f8b880103f0020bc800680807888787f800008192a3b4"
                       "c080080170170700c0080a980808000000000")
          .value());
  f1c_gw.get_du(du_index).on_new_message(ul_rrc_msg_transfer);
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

  // Inject UE Context Setup Response
  f1ap_message ue_ctxt_setup_response = generate_ue_context_setup_response(cu_ue_id, du_ue_id);
  f1c_gw.get_du(du_index).on_new_message(ue_ctxt_setup_response);

  // Inject Security Mode Complete
  f1ap_message ul_rrc_msg_transfer = generate_ul_rrc_message_transfer(
      cu_ue_id, du_ue_id, srb_id_t::srb1, make_byte_buffer("00032a00fd5ec7ff").value());
  f1c_gw.get_du(du_index).on_new_message(ul_rrc_msg_transfer);

  // Inject UE capability info
  ul_rrc_msg_transfer = generate_ul_rrc_message_transfer(
      cu_ue_id,
      du_ue_id,
      srb_id_t::srb1,
      make_byte_buffer("00044c821930680ce811d1968097e360e1480005824c5c00060fc2c00637fe002e00131401a0000000880058d006007"
                       "a071e439f0000240400e0300000000100186c0000700809df000000000000030368000800004b2ca000a07143c001c0"
                       "03c000000100200409028098a8660c")
          .value());
  f1c_gw.get_du(du_index).on_new_message(ul_rrc_msg_transfer);
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

  // Pass E1SetupRequest to the CU-CP
  e1ap_message e1setup_msg = generate_valid_cu_up_e1_setup_request();
  e1ap_gw.get_cu_up(0).on_new_message(e1setup_msg);
}

void cu_cp_test::test_du_attach(du_index_t du_index, gnb_du_id_t gnb_du_id, nr_cell_identity nrcell_id, pci_t pci)
{
  // Store current number of DUs.
  size_t nof_dus = f1c_gw.nof_connections();

  // Create a new DU connection to the CU-CP, creating a new DU processor in the CU-CP in the process.
  f1c_gw.request_new_du_connection();

  size_t expected_nof_dus = nof_dus + 1;
  ASSERT_EQ(f1c_gw.nof_connections(), expected_nof_dus);

  // Pass F1SetupRequest to the CU-CP
  f1ap_message f1setup_msg = test_helpers::generate_f1_setup_request(gnb_du_id, nrcell_id, pci);
  f1c_gw.get_du(du_index).on_new_message(f1setup_msg);
}

void cu_cp_test::add_pdu_sessions(std::vector<pdu_session_id_t> psis,
                                  du_index_t                    du_index,
                                  gnb_du_ue_f1ap_id_t           du_ue_id,
                                  gnb_cu_ue_f1ap_id_t           cu_ue_id,
                                  amf_ue_id_t                   amf_ue_id,
                                  ran_ue_id_t                   ran_ue_id,
                                  gnb_cu_cp_ue_e1ap_id_t        cu_cp_ue_e1ap_id,
                                  gnb_cu_up_ue_e1ap_id_t        cu_up_ue_e1ap_id)
{
  bool initial_pdu_session = true;

  for (const auto psi : psis) {
    // Inject PDU Session Resource Setup Request
    ngap_message pdu_session_resource_setup_request =
        generate_valid_pdu_session_resource_setup_request_message(amf_ue_id, ran_ue_id, psi);
    cu_cp_obj->get_ngap_message_handler().handle_message(pdu_session_resource_setup_request);

    if (initial_pdu_session) {
      initial_pdu_session = false;

      // check that the Bearer Context Setup was sent to the CU-UP
      ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
      ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
                asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_setup_request);

      // Inject Bearer Context Setup Response
      e1ap_message bearer_context_setup_resp =
          generate_bearer_context_setup_response(cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);
      e1ap_gw.get_cu_up(0).on_new_message(bearer_context_setup_resp);
    } else {
      // check that the Bearer Context Modification was sent to the CU-UP
      ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
      ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
                asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_mod_request);
      // Inject Bearer Context Modification Response
      e1ap_message bearer_context_mod_resp =
          generate_bearer_context_modification_response(cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);
      e1ap_gw.get_cu_up(0).on_new_message(bearer_context_mod_resp);
    }

    // check that the UE Context Modification Request was sent to the DU
    ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.type(), asn1::f1ap::f1ap_pdu_c::types_opts::options::init_msg);
    ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
              asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_mod_request);

    // Inject UE Context Modification Response
    f1ap_message ue_context_mod_resp = generate_ue_context_modification_response(cu_ue_id, du_ue_id);
    f1c_gw.get_du(du_index).on_new_message(ue_context_mod_resp);

    // check that the Bearer Context Modification was sent to the CU-UP
    ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
    ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
              asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_mod_request);

    // check that the UE Context Modification Request contains the UE capabilities
    ASSERT_TRUE(f1c_gw.last_tx_pdus(0).back().pdu.init_msg().value.ue_context_mod_request()->cu_to_du_rrc_info_present);
    ASSERT_NE(f1c_gw.last_tx_pdus(0)
                  .back()
                  .pdu.init_msg()
                  .value.ue_context_mod_request()
                  ->cu_to_du_rrc_info.ue_cap_rat_container_list.size(),
              0U);

    // Inject Bearer Context Modification Response
    e1ap_message bearer_context_mod_resp =
        generate_bearer_context_modification_response(cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);
    e1ap_gw.get_cu_up(0).on_new_message(bearer_context_mod_resp);

    // check that the RRC Reconfiguration was sent to the DU
    ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.type(), asn1::f1ap::f1ap_pdu_c::types_opts::options::init_msg);
    ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
              asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::dl_rrc_msg_transfer);

    // Inject RRC Reconfiguration Complete
    f1ap_message ul_rrc_msg_transfer = generate_ul_rrc_message_transfer(
        cu_ue_id, du_ue_id, srb_id_t::srb1, make_byte_buffer("00070e00cc6fcda5").value());
    f1c_gw.get_du(du_index).on_new_message(ul_rrc_msg_transfer);

    // check that the PDU Session Resource Setup Response was sent to the AMF
    ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.type(), asn1::ngap::ngap_pdu_c::types_opts::options::successful_outcome);
    ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.successful_outcome().value.type().value,
              asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::pdu_session_res_setup_resp);
  }
}

void cu_cp_test::test_preamble_all_connected(du_index_t du_index, pci_t pci)
{
  test_amf_connection();

  test_e1ap_attach();

  test_du_attach(du_index, int_to_gnb_du_id(0x11), nr_cell_identity::create(gnb_id_t{411, 22}, 0).value(), pci);
}

void cu_cp_test::test_preamble_ue_creation(du_index_t          du_index,
                                           gnb_du_ue_f1ap_id_t du_ue_id,
                                           gnb_cu_ue_f1ap_id_t cu_ue_id,
                                           rnti_t              crnti,
                                           amf_ue_id_t         amf_ue_id,
                                           ran_ue_id_t         ran_ue_id)
{
  // Attach UE
  attach_ue(du_ue_id, cu_ue_id, crnti, du_index);
  ASSERT_EQ(cu_cp_obj->get_metrics_handler().request_metrics_report().ues.size(), 1);

  authenticate_ue(amf_ue_id, ran_ue_id, du_index, du_ue_id, cu_ue_id);

  setup_security(amf_ue_id, ran_ue_id, du_index, du_ue_id, cu_ue_id);
}

void cu_cp_test::test_preamble_ue_full_attach(du_index_t                    du_index,
                                              gnb_du_ue_f1ap_id_t           du_ue_id,
                                              gnb_cu_ue_f1ap_id_t           cu_ue_id,
                                              rnti_t                        crnti,
                                              amf_ue_id_t                   amf_ue_id,
                                              ran_ue_id_t                   ran_ue_id,
                                              std::vector<pdu_session_id_t> psis_to_setup,
                                              gnb_cu_cp_ue_e1ap_id_t        cu_cp_ue_e1ap_id,
                                              gnb_cu_up_ue_e1ap_id_t        cu_up_ue_e1ap_id)
{
  // Create UE
  test_preamble_ue_creation(du_index, du_ue_id, cu_ue_id, crnti, amf_ue_id, ran_ue_id);

  // Inject Registration Complete
  f1ap_message ul_rrc_msg_transfer = generate_ul_rrc_message_transfer(
      cu_ue_id, du_ue_id, srb_id_t::srb1, make_byte_buffer("00053a053f015362c51680bf00218086b09a5b").value());
  f1c_gw.get_du(du_index).on_new_message(ul_rrc_msg_transfer);

  // Inject PDU Session Establishment Request
  ul_rrc_msg_transfer = generate_ul_rrc_message_transfer(
      cu_ue_id,
      du_ue_id,
      srb_id_t::srb1,
      make_byte_buffer("00063a253f011ffa9203013f0033808018970080e0ffffc9d8bd8013404010880080000840830000000041830000000"
                       "00000800001800005000006000006800008800900c092838339b939b0b83700e03a21bb")
          .value());
  f1c_gw.get_du(du_index).on_new_message(ul_rrc_msg_transfer);

  // Inject Configuration Update Command
  ngap_message dl_nas_transport_msg = generate_downlink_nas_transport_message(
      amf_ue_id,
      ran_ue_id,
      make_byte_buffer("7e0205545bfc027e0054430f90004f00700065006e00350047005346004732800131235200490100").value());
  cu_cp_obj->get_ngap_message_handler().handle_message(dl_nas_transport_msg);

  add_pdu_sessions(
      std::move(psis_to_setup), du_index, du_ue_id, cu_ue_id, amf_ue_id, ran_ue_id, cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);
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
  auto&            paging_cell_item = paging_msg->paging_cell_list[0].value().paging_cell_item();
  nr_cell_identity nci              = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
  if (paging_cell_item.nr_cgi.nr_cell_id.to_number() != nci.value()) {
    test_logger.error("NR CGI NCI mismatch {} != {}", paging_cell_item.nr_cgi.nr_cell_id.to_number(), nci);
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

void cu_cp_test::start_auto_tick(const std::future<void>& stop_signal)
{
  do {
    timers.tick();
    ctrl_worker.run_pending_tasks();
  } while (stop_signal.wait_for(std::chrono::milliseconds{1}) != std::future_status::ready);
}
