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

#include "du_high_cu_test_simulator.h"
#include "tests/test_doubles/du/test_du_high_worker_manager.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/mac/mac_test_messages.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "srsran/cu_cp/cu_cp_configuration_helpers.h"
#include "srsran/cu_cp/cu_cp_factory.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/du/du_high/du_high_clock_controller.h"
#include "srsran/du/du_high/du_high_factory.h"
#include "srsran/mac/mac_cell_timing_context.h"
#include "srsran/scheduler/config/scheduler_expert_config_factory.h"
#include "srsran/support/io/io_broker_factory.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

static constexpr uint32_t task_worker_queue_size = 10000;

static void init_loggers()
{
  srslog::fetch_basic_logger("MAC", true).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("SCHED", true).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("RLC").set_level(srslog::basic_levels::info);
  srslog::fetch_basic_logger("DU-MNG").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("DU-F1").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("CU-CP-F1").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("CU-CP").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("RRC").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("NGAP").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("ASN1").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
  srslog::init();
}

du_high_cu_cp_worker_manager::du_high_cu_cp_worker_manager(unsigned nof_dus, timer_manager& timers) :
  test_worker(task_worker_queue_size)
{
  init_loggers();

  for (unsigned i = 0; i != nof_dus; ++i) {
    dus.push_back(test_helpers::create_multi_threaded_du_high_executor_mapper(
        test_helpers::du_high_worker_config{1, false, {}, timers}));
  }

  // CU-CP especific executor.
  // Note: Reuse one of the DU-high control executors.
  cu_cp_exec = &dus[0]->get_exec_mapper().du_control_executor();
}

du_high_cu_cp_worker_manager::~du_high_cu_cp_worker_manager()
{
  stop();
}

void du_high_cu_cp_worker_manager::stop()
{
  for (auto& w : dus) {
    w->stop();
  }
  test_worker.stop();
}

du_high_cu_test_simulator::du_high_cu_test_simulator(const du_high_cu_cp_test_simulator_config& cfg_) :
  cfg(cfg_),
  logger(srslog::fetch_basic_logger("TEST")),
  workers(cfg.dus.size(), timers),
  broker(create_io_broker(io_broker_type::epoll)),
  timer_ctrl(srs_du::create_du_high_clock_controller(timers, *broker, workers.dus[0]->timer_executor()))
{
  // Prepare CU-CP config.
  srs_cu_cp::cu_cp_configuration cu_cfg = config_helpers::make_default_cu_cp_config();
  cu_cfg.services.cu_cp_executor        = workers.cu_cp_exec;
  cu_cfg.services.timers                = &timers;
  cu_cfg.ngap.ngaps.push_back(srs_cu_cp::cu_cp_configuration::ngap_config{
      &n2_gw, {{7, {{plmn_identity::test_value(), {{slice_service_type{1}}}}}}}});

  // Instatiate CU-CP.
  cu_cp_inst = create_cu_cp(cu_cfg);

  // Start CU-CP.
  cu_cp_inst->start();

  // Connect AMF by injecting a ng_setup_response
  cu_cp_inst->get_ng_handler()
      .get_ngap_message_handler(plmn_identity::test_value())
      ->handle_message(srs_cu_cp::generate_ng_setup_response());

  // Connect F1-C to CU-CP.
  f1c_gw.attach_cu_cp_du_repo(cu_cp_inst->get_f1c_handler());
}

du_high_cu_test_simulator::~du_high_cu_test_simulator()
{
  logger.info("Stopping test simulator.");

  for (auto& du : dus) {
    du->du_high_inst->stop();
  }

  workers.stop();
}

bool du_high_cu_test_simulator::add_ue(unsigned du_index, rnti_t rnti)
{
  f1c_gw.clear_messages();

  du_sim& du_ctxt = *dus[du_index];

  // Send UL-CCCH message.
  du_ctxt.du_high_inst->get_pdu_handler().handle_rx_data_indication(
      test_helpers::create_ccch_message(du_ctxt.next_slot, rnti));

  // Wait for Init UL RRC Message to come out of the F1AP.
  run_until([this, du_index]() { return not f1c_gw.get_last_cu_cp_rx_pdus(du_index).empty(); });

  srsran_assert(f1c_gw.get_last_cu_cp_rx_pdus(du_index).size() == 1, "Too many messages sent by the DU");

  return test_helpers::is_init_ul_rrc_msg_transfer_valid(f1c_gw.get_last_cu_cp_rx_pdus(du_index)[0], rnti);
}

void du_high_cu_test_simulator::start_dus()
{
  for (unsigned du_idx = 0; du_idx != cfg.dus.size(); ++du_idx) {
    dus.emplace_back(std::make_unique<du_sim>(workers.test_worker));
    auto& du_ctxt = *dus.back();

    // Setup DU-specific slot index.
    du_ctxt.next_slot = {0, test_rgen::uniform_int<unsigned>(0, 10239)};

    // Instantiate DU-high.
    srs_du::du_high_configuration& du_hi_cfg = du_ctxt.du_high_cfg;
    du_hi_cfg.ran.gnb_du_name                = fmt::format("srsgnb{}", du_idx + 1);
    du_hi_cfg.ran.gnb_du_id                  = (gnb_du_id_t)(du_idx + 1);
    du_hi_cfg.ran.cells                      = cfg.dus[du_idx];
    du_hi_cfg.ran.sched_cfg                  = config_helpers::make_default_scheduler_expert_config();

    srs_du::du_high_dependencies du_dependencies;
    du_dependencies.exec_mapper = &workers.dus[du_idx]->get_exec_mapper();
    du_dependencies.f1c_client  = &f1c_gw;
    du_dependencies.f1u_gw      = nullptr;
    du_dependencies.phy_adapter = &du_ctxt.phy;
    du_dependencies.timer_ctrl  = timer_ctrl.get();
    du_dependencies.mac_p       = &du_ctxt.mac_pcap;
    du_dependencies.rlc_p       = &du_ctxt.rlc_pcap;

    du_ctxt.du_high_inst = make_du_high(du_hi_cfg, du_dependencies);

    du_ctxt.du_high_inst->start();
  }
}

void du_high_cu_test_simulator::run_slot()
{
  for (unsigned i = 0; i != dus.size(); ++i) {
    srs_du::du_high& du_hi = *dus[i]->du_high_inst;

    // Signal slot indication to l2.
    du_hi.get_slot_handler(to_du_cell_index(0))
        .handle_slot_indication({dus[i]->next_slot, std::chrono::system_clock::now()});

    // Wait for slot indication to be processed and the l2 results to be sent back to the l1 (in this case, the test
    // main thread).
    const unsigned                            MAX_COUNT = 1000;
    const std::optional<mac_dl_sched_result>& dl_result = dus[i]->phy.cells[0].last_dl_res;
    for (unsigned count = 0; count < MAX_COUNT and (not dl_result.has_value() or dl_result->slot != dus[i]->next_slot);
         ++count) {
      // Process tasks dispatched to the test main thread (e.g. L2 slot result)
      workers.test_worker.run_pending_tasks();

      // Wait for tasks to arrive to test thread.
      std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
    EXPECT_TRUE(dl_result.has_value() and dl_result->slot == dus[i]->next_slot);

    // Increament the DU slot.
    ++dus[i]->next_slot;
  }
}

bool du_high_cu_test_simulator::run_until(unique_function<bool()> condition)
{
  const unsigned MAX_SLOT_COUNT = 1000;
  for (unsigned count = 0; count != MAX_SLOT_COUNT; ++count) {
    if (condition()) {
      return true;
    }
    run_slot();
  }
  return false;
}
