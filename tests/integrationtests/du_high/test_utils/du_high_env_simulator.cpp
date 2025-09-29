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

#include "du_high_env_simulator.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/mac/mac_test_messages.h"
#include "tests/unittests/f1ap/du/f1ap_du_test_helpers.h"
#include "tests/unittests/scheduler/test_utils/result_test_helpers.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/du/du_high/du_high_clock_controller.h"
#include "srsran/du/du_high/du_high_factory.h"
#include "srsran/du/du_high/du_qos_config_helpers.h"
#include "srsran/mac/mac_cell_timing_context.h"
#include "srsran/scheduler/config/scheduler_expert_config_factory.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/io/io_broker_factory.h"
#include "srsran/support/test_utils.h"

using namespace srsran;
using namespace srs_du;

static void init_loggers()
{
  srslog::fetch_basic_logger("MAC", true).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("SCHED", true).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("RLC").set_level(srslog::basic_levels::info);
  srslog::fetch_basic_logger("DU-MNG").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("DU-F1-U").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("DU-F1").set_level(srslog::basic_levels::info);
  srslog::fetch_basic_logger("ASN1").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
  srslog::init();
}

du_high_configuration srs_du::create_du_high_configuration(const du_high_env_sim_params& params)
{
  du_high_configuration cfg{};
  cfg.ran.sched_cfg.log_broadcast_messages = false;
  cfg.ran.cells.reserve(params.nof_cells);
  auto builder_params =
      params.builder_params.has_value() ? params.builder_params.value() : cell_config_builder_params{};
  for (unsigned i = 0; i < params.nof_cells; ++i) {
    builder_params.pci = (pci_t)i;
    auto du_cell_cfg   = config_helpers::make_default_du_cell_config(builder_params);
    if (params.prach_frequency_start.has_value()) {
      du_cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.msg1_frequency_start =
          params.prach_frequency_start.value();
    }
    du_cell_cfg.ue_ded_serv_cell_cfg.init_dl_bwp.pdsch_cfg->mcs_table = pdsch_mcs_table::qam256;
    cfg.ran.cells.push_back(du_cell_cfg);
    cfg.ran.cells.back().nr_cgi.nci = nr_cell_identity::create(i).value();
    if (params.pucch_cfg.has_value()) {
      cfg.ran.cells.back().pucch_cfg = params.pucch_cfg.value();
    }
    cfg.ran.mac_cfg.configs.push_back({10000, 10000, 10000});
    if (params.srs_period.has_value()) {
      cfg.ran.cells.back().srs_cfg.srs_period = params.srs_period;
    }
  }

  cfg.ran.qos       = config_helpers::make_default_du_qos_config_list(/* warn_on_drop */ true, 0);
  cfg.ran.sched_cfg = config_helpers::make_default_scheduler_expert_config();

  cfg.metrics.enable_f1ap  = true;
  cfg.metrics.enable_mac   = true;
  cfg.metrics.enable_rlc   = true;
  cfg.metrics.enable_sched = true;

  return cfg;
}

du_high_env_simulator::du_high_env_simulator(du_high_env_sim_params params) :
  du_high_env_simulator(create_du_high_configuration(params), params.active_cells_on_start)
{
}

du_high_env_simulator::du_high_env_simulator(const du_high_configuration& du_hi_cfg_, bool active_cells_on_start) :
  broker(create_io_broker(io_broker_type::epoll)),
  timer_ctrl(srs_du::create_du_high_clock_controller(timers, *broker, *workers.time_exec)),
  cu_notifier(workers.test_worker, active_cells_on_start),
  du_metrics(workers.test_worker),
  du_high_cfg(du_hi_cfg_),
  du_hi_dependencies([this]() {
    init_loggers();
    du_high_dependencies dependencies;
    dependencies.exec_mapper = workers.exec_mapper.get();
    dependencies.f1c_client  = &cu_notifier;
    dependencies.f1u_gw      = &cu_up_sim;
    dependencies.du_notifier = &du_metrics;
    dependencies.phy_adapter = &phy;
    dependencies.timer_ctrl  = timer_ctrl.get();
    dependencies.mac_p       = &mac_pcap;
    dependencies.rlc_p       = &rlc_pcap;
    return dependencies;
  }()),
  du_hi(make_du_high(du_high_cfg, du_hi_dependencies)),
  phy(du_high_cfg.ran.cells.size(), workers.test_worker),
  next_slot(to_numerology_value(du_high_cfg.ran.cells[0].scs_common),
            test_rgen::uniform_int<unsigned>(0, 10239) *
                get_nof_slots_per_subframe(du_high_cfg.ran.cells[0].scs_common))
{
  // Start DU and try to connect to CU.
  du_hi->start();

  // Ensure the result is saved in the notifier.
  run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); });
}

du_high_env_simulator::~du_high_env_simulator()
{
  du_hi->stop();

  timer_ctrl.reset();

  // Stop workers before starting to take down other components.
  workers.stop();
}

bool du_high_env_simulator::run_until(unique_function<bool()> condition, std::optional<unsigned> max_slot_count)
{
  unsigned max_count = max_slot_count.has_value() ? max_slot_count.value() : 1000;
  for (unsigned count = 0; count != max_count; ++count) {
    if (condition()) {
      return true;
    }
    run_slot();
  }
  return false;
}

bool du_high_env_simulator::add_ue(rnti_t rnti, du_cell_index_t cell_index)
{
  if (ues.count(rnti) > 0) {
    return false;
  }

  cu_notifier.f1ap_ul_msgs.clear();

  // Send UL-CCCH message.
  du_hi->get_pdu_handler().handle_rx_data_indication(test_helpers::create_ccch_message(next_slot, rnti, cell_index));

  // Wait for Init UL RRC Message to come out of the F1AP and ConRes CE to be scheduled.
  // Note: These events are concurrent.
  bool init_ul_rrc_msg_flag = false;
  auto init_ul_rrc_msg_sent = [this, rnti, &init_ul_rrc_msg_flag]() {
    if (init_ul_rrc_msg_flag) {
      return;
    }
    if (not cu_notifier.f1ap_ul_msgs.empty()) {
      report_fatal_error_if_not(
          test_helpers::is_init_ul_rrc_msg_transfer_valid(cu_notifier.f1ap_ul_msgs.rbegin()->second, rnti),
          "Init UL RRC Message is not valid");
      init_ul_rrc_msg_flag = true;
      return;
    }
  };
  bool conres_sent     = false;
  auto con_res_ce_sent = [this, rnti, cell_index, &conres_sent]() {
    if (conres_sent) {
      return;
    }
    phy_cell_test_dummy& phy_cell = phy.cells[cell_index];
    if (phy_cell.last_dl_res.has_value()) {
      auto& dl_res = *phy_cell.last_dl_res.value().dl_res;
      if (find_ue_pdsch(rnti, dl_res.ue_grants) != nullptr) {
        report_fatal_error_if_not(find_ue_pdsch_with_lcid(rnti, lcid_dl_sch_t::UE_CON_RES_ID, dl_res.ue_grants) !=
                                      nullptr,
                                  "UE ConRes not scheduled");
        conres_sent = true;
        return;
      }
    }
  };
  if (not run_until([&]() {
        init_ul_rrc_msg_sent();
        con_res_ce_sent();
        return conres_sent and init_ul_rrc_msg_flag;
      })) {
    test_logger.error("rnti={}: Unable to add UE. Timeout waiting for Init UL RRC Message or ConRes CE", rnti);
    return false;
  }

  gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(
      cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.init_msg().value.init_ul_rrc_msg_transfer()->gnb_du_ue_f1ap_id);
  gnb_cu_ue_f1ap_id_t cu_ue_id = int_to_gnb_cu_ue_f1ap_id(next_cu_ue_id++);
  EXPECT_TRUE(ues.insert(std::make_pair(rnti, ue_sim_context{rnti, du_ue_id, cu_ue_id, cell_index})).second);

  return true;
}

bool du_high_env_simulator::run_rrc_setup(rnti_t rnti)
{
  auto it = ues.find(rnti);
  if (it == ues.end()) {
    test_logger.error("rnti={}: Failed to run RRC Setup procedure. Cause: UE not found", rnti);
    return false;
  }
  const ue_sim_context& u = it->second;

  // Send DL RRC Message which contains RRC Setup.
  f1ap_message msg = generate_dl_rrc_message_transfer(
      *u.du_ue_id, *u.cu_ue_id, srb_id_t::srb0, byte_buffer::create({0x1, 0x2, 0x3}).value());

  return send_dl_rrc_msg_and_await_ul_rrc_msg(u, msg, 0);
}

bool du_high_env_simulator::run_rrc_reject(rnti_t rnti)
{
  return run_ue_context_release(rnti, srb_id_t::srb0);
}

bool du_high_env_simulator::run_rrc_reestablishment(rnti_t rnti, rnti_t old_rnti, reestablishment_stage stop_at)
{
  auto it = ues.find(rnti);
  if (it == ues.end()) {
    test_logger.error("rnti={}: Failed to run RRC Reestablishment. Cause: UE not found", rnti);
    return false;
  }
  const ue_sim_context& u      = it->second;
  auto                  old_it = ues.find(old_rnti);
  if (old_it == ues.end()) {
    test_logger.error(
        "rnti={}: Failed to run RRC Reestablishment. Cause: Old UE with c-rnti={} not found", rnti, old_rnti);
    return false;
  }
  const ue_sim_context& old_u = old_it->second;

  // Generate DL RRC Message Transfer (containing RRC Reestablishment)
  f1ap_message msg = generate_dl_rrc_message_transfer(
      *u.du_ue_id, *u.cu_ue_id, srb_id_t::srb1, byte_buffer::create({0x1, 0x2, 0x3}).value());
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->old_gnb_du_ue_f1ap_id_present = true;
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->old_gnb_du_ue_f1ap_id         = (uint64_t)old_u.du_ue_id.value();

  // Send RRC Reestablishment and await response.
  if (not send_dl_rrc_msg_and_await_ul_rrc_msg(u, msg, 0)) {
    return false;
  }
  if (stop_at == reestablishment_stage::reest_complete) {
    return true;
  }

  // Run F1AP UE Context Modification procedure.
  msg = test_helpers::generate_ue_context_modification_request(*u.du_ue_id, *u.cu_ue_id, {}, {drb_id_t::drb1}, {});
  cu_notifier.f1ap_ul_msgs.clear();
  du_hi->get_f1ap_du().handle_message(msg);
  bool ret = run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); });
  if (not ret) {
    test_logger.error("rnti={}: F1AP UE Context Modification Request not sent back to the CU-CP", u.rnti);
    return false;
  }
  if (not test_helpers::is_valid_ue_context_modification_response(
          cu_notifier.f1ap_ul_msgs.rbegin()->second, msg, test_helpers::ue_context_mod_context::reestablistment)) {
    test_logger.error("rnti={}: F1AP UE Context Modification Response sent back to the CU-CP is not valid", u.rnti);
    return false;
  }
  const asn1::f1ap::ue_context_mod_resp_s& resp =
      cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.successful_outcome().value.ue_context_mod_resp();
  EXPECT_FALSE(resp->drbs_failed_to_be_modified_list_present);

  // CU-CP sends RRC Reconfiguration and awaits RRC Reconfiguration Complete.
  msg = generate_dl_rrc_message_transfer(
      *u.du_ue_id, *u.cu_ue_id, srb_id_t::srb1, byte_buffer::create({0x1, 0x2, 0x3}).value());
  if (not send_dl_rrc_msg_and_await_ul_rrc_msg(u, msg, 1)) {
    return false;
  }

  return true;
}

bool du_high_env_simulator::await_dl_msg_sched(const ue_sim_context&   u,
                                               lcid_t                  lcid,
                                               std::optional<unsigned> max_slot_count)
{
  const auto& phy_cell = phy.cells[u.pcell_index];

  bool ret = run_until(
      [&]() {
        if (phy_cell.last_dl_res.has_value() and phy_cell.last_dl_res.value().dl_res != nullptr) {
          auto& dl_res = *phy_cell.last_dl_res.value().dl_res;
          return find_ue_pdsch_with_lcid(u.rnti, lcid, dl_res.ue_grants) != nullptr;
        }
        return false;
      },
      max_slot_count);
  if (not ret) {
    test_logger.error("rnti={}: Msg4 not sent to the PHY", u.rnti);
    return false;
  }
  return true;
}

bool du_high_env_simulator::send_dl_rrc_msg_and_await_ul_rrc_msg(const ue_sim_context& u,
                                                                 const f1ap_message&   dl_msg,
                                                                 uint32_t              rlc_ul_sn)
{
  lcid_t dl_lcid = uint_to_lcid(dl_msg.pdu.init_msg().value.dl_rrc_msg_transfer()->srb_id);
  lcid_t ul_lcid = dl_lcid == LCID_SRB0 ? LCID_SRB1 : dl_lcid;

  du_hi->get_f1ap_du().handle_message(dl_msg);

  // Wait for DL message to be sent to the PHY.
  if (not await_dl_msg_sched(u, dl_lcid)) {
    return false;
  }

  // Wait for DL message to be ACKed.
  unsigned dl_msg_k1 = 4;
  for (unsigned i = 0; i != dl_msg_k1; ++i) {
    run_slot();
  }

  // UE sends UL message. Wait until F1AP forwards UL RRC Message to CU-CP.
  cu_notifier.f1ap_ul_msgs.clear();
  du_hi->get_pdu_handler().handle_rx_data_indication(
      test_helpers::create_pdu_with_sdu(next_slot, u.rnti, ul_lcid, rlc_ul_sn));
  bool     ret       = run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); });
  srb_id_t ul_srb_id = int_to_srb_id(ul_lcid);
  if (not ret or not test_helpers::is_ul_rrc_msg_transfer_valid(cu_notifier.f1ap_ul_msgs.rbegin()->second, ul_srb_id)) {
    test_logger.error("rnti={}: F1AP UL RRC Message not sent or is invalid", u.rnti);
    return false;
  }

  return true;
}

bool du_high_env_simulator::run_ue_context_setup(rnti_t rnti)
{
  auto it = ues.find(rnti);
  if (it == ues.end()) {
    return false;
  }
  auto& u = it->second;

  // DU receives UE Context Setup Request.
  cu_notifier.f1ap_ul_msgs.clear();
  f1ap_message msg = test_helpers::generate_ue_context_setup_request(
      *u.cu_ue_id,
      u.du_ue_id,
      u.srbs[LCID_SRB1].next_pdcp_sn++,
      {drb_id_t::drb1},
      {plmn_identity::test_value(), nr_cell_identity::create(0).value()});
  asn1::f1ap::ue_context_setup_request_s& cmd = msg.pdu.init_msg().value.ue_context_setup_request();
  cmd->drbs_to_be_setup_list[0]
      .value()
      .drbs_to_be_setup_item()
      .qos_info.choice_ext()
      .value()
      .drb_info()
      .drb_qos.qos_characteristics.non_dyn_5qi()
      .five_qi = 7U;
  cmd->drbs_to_be_setup_list[0].value().drbs_to_be_setup_item().rlc_mode.value =
      asn1::f1ap::rlc_mode_opts::rlc_um_bidirectional;
  // UE supports 256QAM.
  cmd->cu_to_du_rrc_info.ue_cap_rat_container_list =
      byte_buffer::create({0x10, 0xc9, 0x83, 0x40, 0x67, 0x40, 0x8e, 0x8c, 0xb4, 0x04, 0xbf, 0x1b, 0x07, 0x0a, 0x40,
                           0x00, 0x2c, 0x12, 0x62, 0xe0, 0x00, 0x30, 0x7e, 0x16, 0x00, 0x31, 0xbf, 0xf0, 0x01, 0x70,
                           0x00, 0x98, 0xa0, 0x51, 0x00, 0x00, 0x00, 0x04, 0x40, 0x02, 0xc6, 0x80, 0x30, 0x03, 0xd0,
                           0x38, 0xf2, 0x1c, 0xf8, 0x00, 0x01, 0x20, 0x20, 0x07, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x08,
                           0x05, 0x03, 0x60, 0x00, 0x03, 0x80, 0x40, 0x4a, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x39, 0x51, 0x40, 0x00, 0x40, 0x00, 0x02, 0x59, 0x65, 0x40, 0x0d, 0x2a, 0xaa, 0x1e, 0x00,
                           0x0e, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x20, 0x48, 0x14})
          .value();
  this->du_hi->get_f1ap_du().handle_message(msg);

  // Wait until DU sends UE Context Setup Response and the whole RRC container is scheduled.
  const unsigned MAX_SLOT_COUNT   = 1000;
  const unsigned srb1_pdu_size    = cmd->rrc_container.size();
  unsigned       srb1_bytes_sched = 0;
  for (unsigned i = 0; i != MAX_SLOT_COUNT and (srb1_bytes_sched < srb1_pdu_size or cu_notifier.f1ap_ul_msgs.empty());
       ++i) {
    run_slot();

    // Sum all the bytes scheduled for SRB1.
    du_cell_index_t     cell_idx = it->second.pcell_index;
    const dl_msg_alloc* pdsch =
        find_ue_pdsch_with_lcid(rnti, LCID_SRB1, phy.cells[cell_idx].last_dl_res.value().dl_res->ue_grants);
    if (pdsch != nullptr) {
      for (const dl_msg_lc_info& lc_grant : pdsch->tb_list[0].lc_chs_to_sched) {
        if (lc_grant.lcid == LCID_SRB1) {
          srb1_bytes_sched += lc_grant.sched_bytes;
        }
      }
    }
  }

  if (cu_notifier.f1ap_ul_msgs.size() != 1) {
    // Response not sent back to CU-CP or too many responses were sent.
    test_logger.info("STATUS: No UE Context Setup Response was sent back to the CU-CP");
    return false;
  }
  if (not test_helpers::is_ue_context_setup_response_valid(cu_notifier.f1ap_ul_msgs.rbegin()->second)) {
    // Bad response.
    test_logger.error("STATUS: UE Context Setup Response sent back to the CU-CP is not valid");
    return false;
  }
  if (srb1_bytes_sched < srb1_pdu_size) {
    // Not enough SRB1 bytes were scheduled for the RRC container.
    test_logger.error("STATUS: Not enough SRB1 bytes were scheduled for the RRC container ({} < {})",
                      srb1_bytes_sched,
                      srb1_pdu_size);
    return false;
  }

  // Await for Reconfiguration Complete that signals the UE config update completion.
  if (srb1_pdu_size > 0) {
    cu_notifier.f1ap_ul_msgs.clear();
    du_hi->get_pdu_handler().handle_rx_data_indication(
        test_helpers::create_pdu_with_sdu(next_slot, u.rnti, LCID_SRB1, 1));
    bool ret = run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); });
    if (not ret or
        not test_helpers::is_ul_rrc_msg_transfer_valid(cu_notifier.f1ap_ul_msgs.rbegin()->second, srb_id_t::srb1)) {
      test_logger.error("rnti={}: F1AP UL RRC Message not sent or is invalid", u.rnti);
      return false;
    }
  }

  return true;
}

bool du_high_env_simulator::run_ue_context_release(rnti_t rnti, srb_id_t srb_id)
{
  auto it = ues.find(rnti);
  if (it == ues.end()) {
    return false;
  }
  auto& u = it->second;

  // Send UE Context Release Command which contains dummy RRC Release.
  cu_notifier.f1ap_ul_msgs.clear();
  f1ap_message msg = test_helpers::generate_ue_context_release_command(*u.cu_ue_id, *u.du_ue_id, srb_id);
  du_hi->get_f1ap_du().handle_message(msg);

  // Await for RRC container to be scheduled in the MAC.
  lcid_t lcid = srb_id_to_lcid(srb_id);
  if (not await_dl_msg_sched(u, lcid, 120)) {
    return false;
  }

  // Wait for UE Context Release Complete.
  bool ret = run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); });
  if (not ret) {
    test_logger.error("Did not receive UE context release complete");
    return false;
  }
  if (not test_helpers::is_valid_ue_context_release_complete(cu_notifier.f1ap_ul_msgs.rbegin()->second, msg)) {
    test_logger.error("UE context release complete message is not valid");
    return false;
  }

  ues.erase(rnti);

  return true;
}

void du_high_env_simulator::run_slot()
{
  // Dispatch a slot indication to all cells in the L2 (fork work across cells).
  for (unsigned i = 0; i != du_high_cfg.ran.cells.size(); ++i) {
    du_hi->get_slot_handler(to_du_cell_index(i)).handle_slot_indication({next_slot, std::chrono::system_clock::now()});
  }

  // Wait for slot indication to be processed and the l2 results to be sent back to the l1 (join cell results, in this
  // case, with the join point being the test main thread).
  for (unsigned i = 0; i != du_high_cfg.ran.cells.size(); ++i) {
    const unsigned MAX_COUNT = 100000;
    for (unsigned count = 0; count < MAX_COUNT and phy.cells[i].last_slot_res != next_slot; ++count) {
      // Process tasks dispatched to the test main thread (e.g. L2 slot result)
      workers.test_worker.run_pending_tasks();

      // Wait for tasks to arrive to test thread.
      std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
    EXPECT_EQ(phy.cells[i].last_slot_res, next_slot)
        << fmt::format("Slot={} failed to be processed (last processed slot={}). Is there a deadlock?",
                       next_slot,
                       phy.cells[i].last_slot_res);
    const std::optional<mac_dl_sched_result>& dl_result = phy.cells[i].last_dl_res;
    if (dl_result.has_value()) {
      EXPECT_EQ(dl_result->slot, next_slot);
    }

    // Process results.
    handle_slot_results(to_du_cell_index(i));

    // Signal pending tasks that a slot has completed.
    next_slot_signal.set();
  }

  // Erase tasks that have completed.
  for (auto it = pending_tasks.begin(); it != pending_tasks.end();) {
    if (it->ready()) {
      it = pending_tasks.erase(it);
    } else {
      ++it;
    }
  }

  // Advance to next slot.
  ++next_slot;

  // Ensure the pending control executor does not get starved.
  workers.flush_pending_control_tasks();
}

void du_high_env_simulator::handle_slot_results(du_cell_index_t cell_index)
{
  auto& phy_cell = phy.cells[cell_index];

  // Auto-generate UCI indications.
  if (phy_cell.last_ul_res.has_value() and phy_cell.last_ul_res->ul_res != nullptr) {
    const ul_sched_result& ul_res = *phy_cell.last_ul_res->ul_res;
    slot_point             sl_rx  = phy_cell.last_ul_res->slot;

    if (not ul_res.pucchs.empty()) {
      mac_uci_indication_message uci_ind = test_helpers::create_uci_indication(sl_rx, ul_res.pucchs);
      this->du_hi->get_control_info_handler(cell_index).handle_uci(uci_ind);
    }

    if (not ul_res.puschs.empty()) {
      mac_crc_indication_message crc_ind = test_helpers::create_crc_indication(sl_rx, ul_res.puschs);
      this->du_hi->get_control_info_handler(cell_index).handle_crc(crc_ind);

      std::optional<mac_uci_indication_message> uci_ind = test_helpers::create_uci_indication(sl_rx, ul_res.puschs);
      if (uci_ind.has_value()) {
        this->du_hi->get_control_info_handler(cell_index).handle_uci(uci_ind.value());
      }
    }

    if (not ul_res.srss.empty()) {
      mac_srs_indication_message srs_ind = test_helpers::create_srs_indication(sl_rx, ul_res.srss);
      this->du_hi->get_control_info_handler(cell_index).handle_srs(srs_ind);
    }
  }
}

void du_high_env_simulator::schedule_task(async_task<void> task)
{
  if (task.ready()) {
    // Task is already completed.
    return;
  }

  // Convert async task to eager task and schedule it.
  pending_tasks.push_back(launch_async([t = std::move(task)](coro_context<eager_async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);
    CORO_AWAIT(t);
    CORO_RETURN();
  }));
}

void du_high_env_simulator::run_until_all_pending_tasks_completion()
{
  while (not pending_tasks.empty()) {
    run_slot();
  }
}

async_task<void>
du_high_env_simulator::launch_ue_creation_task(rnti_t rnti, du_cell_index_t cell_index, bool assert_success)
{
  /// Note: This timeout needs to be higher than conRes timeout and the time for the DU to send init UL RRC message.
  static constexpr unsigned ue_creation_timeout = 500U;

  // Check if Init UL RRC Message has been sent for the given RNTI.
  // Note: We avoid re-checking messages that have already been checked via msgno.
  auto find_and_rem_init_ul_rrc_msg = [msgno = 0U](dummy_f1c_test_client& cu_notif, rnti_t rnti_val) mutable {
    std::unique_ptr<f1ap_message> ret;
    for (auto it = cu_notif.f1ap_ul_msgs.lower_bound(msgno); it != cu_notif.f1ap_ul_msgs.end(); ++it) {
      if (test_helpers::is_init_ul_rrc_msg_transfer_valid(it->second, rnti_val)) {
        ret = std::make_unique<f1ap_message>(it->second);
        cu_notif.f1ap_ul_msgs.erase(it);
        return ret;
      }
    }
    msgno = cu_notif.next_ul_message_number();
    return ret;
  };

  return launch_async([this,
                       rnti,
                       cell_index,
                       assert_success,
                       conres_sent = false,
                       count       = 0U,
                       find_and_rem_init_ul_rrc_msg,
                       init_ul_rrc_msg = std::unique_ptr<f1ap_message>{}](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    if (ues.count(rnti) > 0) {
      EXPECT_FALSE(assert_success) << fmt::format("rnti={}: UE already exists", rnti);
      CORO_EARLY_RETURN();
    }

    // Forward UL-CCCH message.
    du_hi->get_pdu_handler().handle_rx_data_indication(test_helpers::create_ccch_message(next_slot, rnti, cell_index));

    for (count = 0; (not conres_sent or not init_ul_rrc_msg) and count < ue_creation_timeout; ++count) {
      // On every run_slot.
      CORO_AWAIT(next_slot_signal);

      // Wait for Init UL RRC Message to come out of the F1AP.
      if (not init_ul_rrc_msg) {
        init_ul_rrc_msg = find_and_rem_init_ul_rrc_msg(cu_notifier, rnti);
      }

      // Wait for ConRes CE to be scheduled.
      if (not conres_sent) {
        phy_cell_test_dummy& phy_cell = phy.cells[cell_index];
        if (phy_cell.last_dl_res.has_value()) {
          auto& dl_res = *phy_cell.last_dl_res.value().dl_res;
          if (find_ue_pdsch(rnti, dl_res.ue_grants) != nullptr) {
            report_fatal_error_if_not(find_ue_pdsch_with_lcid(rnti, lcid_dl_sch_t::UE_CON_RES_ID, dl_res.ue_grants) !=
                                          nullptr,
                                      "rnti={}: UE ConRes not scheduled",
                                      rnti);
            conres_sent = true;
          }
        }
      }
    }
    EXPECT_TRUE(not assert_success or (conres_sent and init_ul_rrc_msg))
        << fmt::format("rnti={}: Unable to add UE. Timeout waiting for Init UL RRC Message or ConRes CE", rnti);

    // Add sim UE object to simulator.
    {
      auto du_ue_id =
          int_to_gnb_du_ue_f1ap_id(init_ul_rrc_msg->pdu.init_msg().value.init_ul_rrc_msg_transfer()->gnb_du_ue_f1ap_id);
      auto ret = ues.insert(std::make_pair(rnti,
                                           ue_sim_context{.rnti             = rnti,
                                                          .du_ue_id         = du_ue_id,
                                                          .cu_ue_id         = int_to_gnb_cu_ue_f1ap_id(next_cu_ue_id++),
                                                          .pcell_index      = cell_index,
                                                          .last_ul_f1ap_msg = std::move(init_ul_rrc_msg)}));
      EXPECT_TRUE(ret.second);
    }

    CORO_RETURN();
  });
}

async_task<void> du_high_env_simulator::launch_rrc_setup_task(rnti_t rnti, bool assert_success)
{
  const ue_sim_context* u = nullptr;
  return launch_async([this, rnti, assert_success, u](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    // Find UE with matching RNTI.
    {
      auto it = ues.find(rnti);
      if (it == ues.end()) {
        test_logger.error("rnti={}: Failed to run RRC Setup procedure. Cause: UE not found", rnti);
        EXPECT_FALSE(assert_success) << fmt::format("rnti={}: UE not found", rnti);
        CORO_EARLY_RETURN();
      }
      u = &it->second;
    }

    if (u->last_ul_f1ap_msg != nullptr) {
      // Determine whether it is an RRC Setup or RRC Reject.
      if (test_helpers::get_du_to_cu_container(*u->last_ul_f1ap_msg).empty()) {
        // RRC container is empty, which means that a RRC Reject needs to be sent.
        EXPECT_FALSE(assert_success) << fmt::format("rnti={}: Failed RRC Setup procedure", rnti);
        CORO_AWAIT(launch_ue_context_release_task(rnti, srb_id_t::srb0));
        CORO_EARLY_RETURN();
      }
    }

    // Send DL RRC Message which contains RRC Setup and await response.
    test_logger.info("rnti={}: RRC Setup procedure starting...", rnti);
    CORO_AWAIT_VALUE(bool ret,
                     launch_send_dl_rrc_msg_and_await_ul_rrc_msg_task(
                         *u,
                         generate_dl_rrc_message_transfer(
                             *u->du_ue_id, *u->cu_ue_id, srb_id_t::srb0, byte_buffer::create({0x1, 0x2, 0x3}).value()),
                         0));
    if (not ret) {
      EXPECT_FALSE(assert_success) << fmt::format("rnti={}: Failed RRC Setup procedure", rnti);
      CORO_EARLY_RETURN();
    }
    test_logger.info("rnti={}: RRC Setup procedure completed", rnti);

    CORO_RETURN();
  });
}

async_task<void>
du_high_env_simulator::launch_ue_context_release_task(rnti_t rnti, srb_id_t srb_id, bool assert_success)
{
  const ue_sim_context* u = nullptr;
  return launch_async([this,
                       rnti,
                       srb_id,
                       u,
                       success = false,
                       assert_success,
                       msgno   = 0U,
                       rel_cmd = f1ap_message{},
                       rel_rx  = unique_function<bool()>{}](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    {
      auto it = ues.find(rnti);
      if (it == ues.end()) {
        test_logger.error("rnti={}: Failed to run UE Context Release. Cause: UE not found", rnti);
        EXPECT_FALSE(assert_success) << fmt::format("rnti={}: UE not found", rnti);
        CORO_EARLY_RETURN();
      }
      u = &it->second;
    }

    {
      // Send UE Context Release Command which contains dummy RRC Release.
      msgno   = cu_notifier.next_ul_message_number();
      rel_cmd = test_helpers::generate_ue_context_release_command(*u->cu_ue_id, *u->du_ue_id, srb_id);
      du_hi->get_f1ap_du().handle_message(rel_cmd);
    }

    // Await for RRC container to be scheduled in the MAC.
    CORO_AWAIT_VALUE(success, launch_await_dl_msg_sched_task(*u, srb_id_to_lcid(srb_id), 120));
    if (not success) {
      EXPECT_FALSE(assert_success) << fmt::format("rnti={}: RRC container not scheduled", rnti);
      CORO_EARLY_RETURN();
    }

    // Wait for UE Context Release Complete.
    rel_rx = [this, msgno, &rel_cmd]() mutable {
      auto it = std::find_if(
          cu_notifier.f1ap_ul_msgs.lower_bound(msgno), cu_notifier.f1ap_ul_msgs.end(), [&rel_cmd](const auto& pair) {
            return test_helpers::is_valid_ue_context_release_complete(pair.second, rel_cmd);
          });
      if (it != cu_notifier.f1ap_ul_msgs.end()) {
        cu_notifier.f1ap_ul_msgs.erase(it);
        return true;
      }
      msgno = cu_notifier.next_ul_message_number();
      return false;
    };
    CORO_AWAIT_VALUE(success, launch_run_until_task(std::move(rel_rx), 500));
    if (not success) {
      test_logger.error("Did not receive UE context release complete");
      EXPECT_FALSE(assert_success) << fmt::format("rnti={}: UE context release complete missing", rnti);
      CORO_EARLY_RETURN();
    }

    ues.erase(rnti);

    CORO_RETURN();
  });
}

async_task<bool> du_high_env_simulator::launch_await_dl_msg_sched_task(const ue_sim_context&   u,
                                                                       lcid_t                  lcid,
                                                                       std::optional<unsigned> max_slot_count)
{
  return launch_async(
      [this, &u, lcid, max_count = max_slot_count.value_or(1000), phy_cell = &phy.cells[u.pcell_index], count = 0U](
          coro_context<async_task<bool>>& ctx) mutable {
        CORO_BEGIN(ctx);

        for (count = 0U; count < max_count; ++count) {
          // Check LCID was scheduled for the given UE.
          if (phy_cell->last_dl_res.has_value() and phy_cell->last_dl_res->dl_res != nullptr) {
            auto& dl_res = *phy_cell->last_dl_res.value().dl_res;
            if (find_ue_pdsch_with_lcid(u.rnti, lcid, dl_res.ue_grants) != nullptr) {
              CORO_EARLY_RETURN(true);
            }
          }

          // Wait for next slot.
          CORO_AWAIT(next_slot_signal);
        }

        CORO_RETURN(false);
      });
}

async_task<bool> du_high_env_simulator::launch_send_dl_rrc_msg_and_await_ul_rrc_msg_task(const ue_sim_context& u,
                                                                                         const f1ap_message&   dl_msg,
                                                                                         uint32_t rlc_ul_sn)
{
  srsran_assert(test_helpers::is_valid_dl_rrc_message_transfer(dl_msg), "Expected F1AP DL RRC Message");
  lcid_t                    dl_lcid   = uint_to_lcid(dl_msg.pdu.init_msg().value.dl_rrc_msg_transfer()->srb_id);
  lcid_t                    ul_lcid   = dl_lcid == LCID_SRB0 ? LCID_SRB1 : dl_lcid;
  constexpr static unsigned dl_msg_k1 = 4;

  return launch_async([this, &u, dl_msg, dl_lcid, ul_lcid, rlc_ul_sn, i = 0U, ul_msg_rx = unique_function<bool()>{}](
                          coro_context<async_task<bool>>& ctx) mutable {
    CORO_BEGIN(ctx);

    // Send DL RRC Message.
    du_hi->get_f1ap_du().handle_message(dl_msg);

    // Wait until DL message is scheduled in the MAC.
    CORO_AWAIT(launch_await_dl_msg_sched_task(u, dl_lcid));

    // Wait for DL message to be ACKed.
    for (i = 0U; i != dl_msg_k1; ++i) {
      CORO_AWAIT(next_slot_signal);
    }

    // UE sends UL message. Await until F1AP forwards UL RRC Message to CU-CP.
    du_hi->get_pdu_handler().handle_rx_data_indication(
        test_helpers::create_pdu_with_sdu(next_slot, u.rnti, ul_lcid, rlc_ul_sn));
    ul_msg_rx = [this, ul_lcid, msgno = cu_notifier.next_ul_message_number()]() mutable -> bool {
      for (auto it = cu_notifier.f1ap_ul_msgs.lower_bound(msgno); it != cu_notifier.f1ap_ul_msgs.end(); ++it) {
        if (test_helpers::is_ul_rrc_msg_transfer_valid(it->second, int_to_srb_id(ul_lcid))) {
          cu_notifier.f1ap_ul_msgs.erase(it);
          return true;
        }
      }
      msgno = cu_notifier.next_ul_message_number();
      return false;
    };
    CORO_AWAIT_VALUE(bool ret, launch_run_until_task(std::move(ul_msg_rx)));

    if (not ret) {
      test_logger.error("rnti={}: F1AP UL RRC Message not sent or is invalid", u.rnti);
    }

    CORO_RETURN(ret);
  });
}

async_task<bool> du_high_env_simulator::launch_run_until_task(unique_function<bool()> condition,
                                                              std::optional<unsigned> max_slot_count)
{
  static constexpr unsigned default_max_slot_count = 1000;
  return launch_async(
      [this, condition = std::move(condition), max_count = max_slot_count.value_or(default_max_slot_count), count = 0U](
          coro_context<async_task<bool>>& ctx) mutable {
        CORO_BEGIN(ctx);

        for (count = 0; count != max_count; ++count) {
          if (condition()) {
            CORO_EARLY_RETURN(true);
          }
          CORO_AWAIT(next_slot_signal);
        }

        CORO_RETURN(false);
      });
}
