/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_high_cu_test_simulator.h"
#include "lib/du_high/du_high_executor_strategies.h"
#include "srsran/cu_cp/cu_cp_factory.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/du_high/du_high_factory.h"

using namespace srsran;

static constexpr uint32_t task_worker_queue_size = 10000;

static void init_loggers()
{
  srslog::fetch_basic_logger("MAC", true).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("SCHED", true).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("RLC").set_level(srslog::basic_levels::info);
  srslog::fetch_basic_logger("DU-MNG").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("DU-F1").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("CU-F1").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("CU-CP").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("RRC").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("NGAP").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("ASN1").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
  srslog::init();
}

du_high_cu_cp_worker_manager::du_high_cu_cp_worker_manager(unsigned nof_dus) : test_worker(task_worker_queue_size)
{
  init_loggers();
  auto make_worker_and_executor = [this](const std::string& name) {
    workers.insert(std::make_pair(name, std::make_unique<task_worker>(name, task_worker_queue_size)));
    executor_insts.insert(std::make_pair(name, std::make_unique<task_worker_executor>(*workers[name])));
    executors.insert(std::make_pair(name, executor_insts[name].get()));
  };

  // Add test executor.
  executors.insert(std::make_pair("TEST", &test_worker));

  // CU-CP especific executor.
  make_worker_and_executor("CU-CP");

  for (unsigned du_idx = 0; du_idx != nof_dus; ++du_idx) {
    std::string prefix_str = fmt::format("DU{}", du_idx + 1);
    make_worker_and_executor(prefix_str + "-CTRL");
    make_worker_and_executor(prefix_str + "-CELL");
    make_worker_and_executor(prefix_str + "-UE");

    auto du_hi_cell_mapper =
        std::make_unique<cell_executor_mapper>(std::initializer_list<task_executor*>{executors[prefix_str + "-CELL"]});
    auto du_hi_ue_mapper = std::make_unique<pcell_ue_executor_mapper>(
        std::initializer_list<task_executor*>{executors[prefix_str + "-UE"]});

    du_hi_exec_mappers.push_back(std::make_unique<du_high_executor_mapper_impl>(std::move(du_hi_cell_mapper),
                                                                                std::move(du_hi_ue_mapper),
                                                                                *executors[prefix_str + "-CTRL"],
                                                                                *executors[prefix_str + "-CTRL"]));
  }
}

du_high_cu_cp_worker_manager::~du_high_cu_cp_worker_manager()
{
  stop();
}

void du_high_cu_cp_worker_manager::stop()
{
  for (auto& w : workers) {
    w.second->stop();
  }
  test_worker.stop();
}

du_high_cu_test_simulator::du_high_cu_test_simulator(const du_high_cu_cp_test_simulator_config& cfg) :
  workers(cfg.dus.size())
{
  // Instantiate CU-CP.
  srs_cu_cp::cu_cp_configuration cu_cfg;
  cu_cfg.cu_cp_executor = workers.executors["CU-CP"];
  cu_cfg.f1ap_notifier  = &f1ap_cu_pdu_notifier;
  cu_cfg.ngap_notifier  = &ngap_amf_notifier;
  cu_cp_inst            = create_cu_cp(cu_cfg);

  for (unsigned du_idx = 0; du_idx != cfg.dus.size(); ++du_idx) {
    dus.emplace_back(std::make_unique<du_sim>(*workers.executors["TEST"]));
    auto& du_ctxt = *dus.back();

    // Instantiate DU-high.
    srs_du::du_high_configuration& du_hi_cfg = du_ctxt.du_high_cfg;
    du_hi_cfg.exec_mapper                    = workers.du_hi_exec_mappers[du_idx].get();
    du_hi_cfg.f1ap_notifier                  = &du_ctxt.f1ap_du_pdu_notifier;
    du_hi_cfg.f1u_gw                         = nullptr;
    du_hi_cfg.phy_adapter                    = &du_ctxt.phy;
    du_hi_cfg.timers                         = &timers;
    du_hi_cfg.metrics_notifier               = &du_ctxt.ue_metrics_notifier;
    du_hi_cfg.cells                          = cfg.dus[du_idx];
    du_hi_cfg.sched_cfg                      = config_helpers::make_default_scheduler_expert_config();
    du_hi_cfg.pcap                           = &du_ctxt.mac_pcap;
    du_ctxt.du_high_inst                     = make_du_high(du_hi_cfg);

    // Connect F1AP CU PDU notifier to DU.
    // TODO: Implement 1:N mapping of CU to DUs.
    f1ap_cu_pdu_notifier.attach_handler(cu_cp_inst.get(), &du_ctxt.du_high_inst->get_f1ap_message_handler());

    // Connect F1AP DU PDU notifier to CU-CP.
    du_ctxt.f1ap_du_pdu_notifier.attach_handler(
        &cu_cp_inst->get_dus().get_f1ap_message_handler(srs_cu_cp::uint_to_du_index(du_idx)));
  }
}