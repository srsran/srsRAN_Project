/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Benchmarks of the DU-high latency.

#include "lib/du_high/du_high.h"
#include "lib/du_high/du_high_executor_strategies.h"
#include "unittests/f1ap/du/f1ap_du_test_helpers.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include "srsgnb/du_high/du_high_configuration.h"
#include "srsgnb/f1u/du/f1u_gateway.h"
#include "srsgnb/support/benchmark_utils.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace srs_du;

/// \brief Parameters of the benchmark.
struct bench_params {
  /// \brief Number of runs for the benchmark. Each repetition corresponds to a slot.
  unsigned nof_repetitions = 100;
};

static void usage(const char* prog, const bench_params& params)
{
  fmt::print("Usage: {} [-R repetitions] [-s silent]\n", prog);
  fmt::print("\t-R Repetitions [Default {}]\n", params.nof_repetitions);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv, bench_params& params)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:h")) != -1) {
    switch (opt) {
      case 'R':
        params.nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'h':
      default:
        usage(argv[0], params);
        exit(0);
    }
  }
}

/// \brief Simulator of the CU-CP from the perspective of the DU. This class should reply to the F1AP messages
/// that the DU sends in order for the DU normal operation to proceed.
class cu_cp_simulator : public f1c_message_notifier
{
public:
  task_executor*       ctrl_exec       = nullptr;
  f1c_message_handler* f1c_msg_handler = nullptr;
  std::atomic<bool>    ue_created{false};

  void on_new_message(const f1c_message& msg) override
  {
    switch (msg.pdu.type().value) {
      case asn1::f1ap::f1ap_pdu_c::types_opts::init_msg:
        handle_init_msg(msg.pdu.init_msg());
        break;
      case asn1::f1ap::f1ap_pdu_c::types_opts::successful_outcome:
        handle_success_outcome(msg.pdu.successful_outcome());
        break;
      default:
        report_fatal_error("Unreachable code in this benchmark");
    }
  }

private:
  void handle_init_msg(const asn1::f1ap::init_msg_s& init_msg)
  {
    switch (init_msg.value.type().value) {
      case asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request: {
        f1c_message f1c_msg;
        f1c_msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
        f1c_msg.pdu.successful_outcome().value.f1_setup_resp()->cells_to_be_activ_list_present = true;
        f1c_msg.pdu.successful_outcome().value.f1_setup_resp()->cells_to_be_activ_list.value.resize(1);
        f1c_msg_handler->handle_message(f1c_msg);
      } break;
      case asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::init_ul_rrc_msg_transfer: {
        // Send UE Context Modification to create DRB1.
        f1c_message msg = generate_f1_ue_context_modification_request({drb_id_t::drb1});
        // Note: Use UM because AM requires status PDUs.
        auto& drb1 = msg.pdu.init_msg()
                         .value.ue_context_mod_request()
                         ->drbs_to_be_setup_mod_list.value[0]
                         ->drbs_to_be_setup_mod_item();
        drb1.rlc_mode.value = asn1::f1ap::rlc_mode_opts::rlc_um_bidirectional;
        f1c_msg_handler->handle_message(msg);
      } break;
      default:
        report_fatal_error("Unreachable code in this benchmark");
    }
  }

  void handle_success_outcome(const asn1::f1ap::successful_outcome_s& succ_outcome)
  {
    switch (succ_outcome.value.type().value) {
      case asn1::f1ap::f1ap_elem_procs_o::successful_outcome_c::types_opts::ue_context_mod_resp: {
        ue_created = true;
      } break;
      default:
        report_fatal_error("Unreachable code in this benchmark");
    }
  }
};

/// \brief Dummy F1-U bearer for the purpose of benchmark.
class f1u_dummy_bearer : public f1u_bearer,
                         public f1u_rx_pdu_handler,
                         public f1u_tx_delivery_handler,
                         public f1u_tx_sdu_handler
{
public:
  f1u_rx_pdu_handler&      get_rx_pdu_handler() override { return *this; }
  f1u_tx_delivery_handler& get_tx_delivery_handler() override { return *this; }
  f1u_tx_sdu_handler&      get_tx_sdu_handler() override { return *this; }

  void handle_pdu(nru_dl_message msg) override {}
  void handle_transmit_notification(uint32_t highest_pdcp_sn) override {}
  void handle_delivery_notification(uint32_t highest_pdcp_sn) override {}
  void handle_sdu(byte_buffer_slice_chain sdu) override {}
};

/// \brief Simulator of the CU-UP from the perspective of the DU.
class cu_up_simulator : public f1u_du_gateway
{
public:
  f1u_dummy_bearer             bearer;
  srs_du::f1u_rx_sdu_notifier* du_notif = nullptr;

  f1u_bearer*
  create_du_bearer(uint32_t ue_index, uint32_t dl_teid, uint32_t ul_teid, srs_du::f1u_rx_sdu_notifier& du_rx) override
  {
    du_notif = &du_rx;
    return &bearer;
  }
  void remove_du_bearer(uint32_t dl_teid) override { du_notif = nullptr; }
};

/// \brief Instantiation of the DU-high workers and executors for the benchmark.
struct du_high_single_cell_worker_manager {
  static const uint32_t task_worker_queue_size = 10000;

  void stop()
  {
    ctrl_worker.stop();
    dl_worker.stop();
    ul_worker.stop();
  }

  task_worker              ctrl_worker{"CTRL", task_worker_queue_size};
  task_worker              dl_worker{"DU-DL#0", task_worker_queue_size};
  task_worker              ul_worker{"DU-UL#0", task_worker_queue_size};
  task_worker_executor     ctrl_exec{ctrl_worker};
  task_worker_executor     dl_exec{dl_worker};
  task_worker_executor     ul_exec{ul_worker};
  pcell_ul_executor_mapper ue_exec_mapper{&ul_exec};
  cell_dl_executor_mapper  cell_exec_mapper{&dl_exec};
};

/// \brief Emulator of the PHY, FAPI and UE from the perspective of the DU-high. This class should be able to provide
/// the required UE signalling (e.g. HARQ ACKs) for the DU-high normal operation.
class phy_simulator : public mac_result_notifier, public mac_cell_result_notifier
{
public:
  mac_cell_control_information_handler* ctrl_info_handler = nullptr;

  std::mutex              mutex;
  std::condition_variable cvar;
  bool                    slot_ended = false;

  unsigned nof_dl_grants = 0;
  uint64_t nof_dl_bytes  = 0;

  mac_cell_result_notifier& get_cell(du_cell_index_t cell_index) override { return *this; }

  void on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res) override
  {
    static constexpr unsigned k1 = 4;

    if (dl_res.dl_res->ue_grants.empty()) {
      return;
    }

    // Forwards HARQ-ACK to the DU-High "k1" slots after the current tx slot.
    mac_uci_indication_message uci{};
    uci.sl_rx = dl_res.slot + k1;
    for (const dl_msg_alloc& ue_grant : dl_res.dl_res->ue_grants) {
      mac_uci_pdu& pdu = uci.ucis.emplace_back();
      pdu.type         = srsgnb::mac_uci_pdu::pdu_type::pucch_f0_or_f1;
      pdu.rnti         = ue_grant.pdsch_cfg.rnti;
      pdu.pucch_f0_or_f1.harq_info.emplace();
      pdu.pucch_f0_or_f1.harq_info->harqs.resize(1);
      pdu.pucch_f0_or_f1.harq_info->harqs[0] = srsgnb::uci_pucch_f0_or_f1_harq_values::ack;
    }
    ctrl_info_handler->handle_uci(uci);
  }

  /// Notifies scheduled PDSCH PDUs.
  void on_new_downlink_data(const mac_dl_data_result& dl_data) override
  {
    // Save the number of DL grants and bytes transmitted.
    nof_dl_grants += dl_data.ue_pdus.size();
    for (const auto& pdu : dl_data.ue_pdus) {
      nof_dl_bytes += pdu.pdu.size();
    }
  }

  /// Notifies slot scheduled PUCCH/PUSCH grants.
  void on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res) override {}

  /// \brief Notifies the completion of all cell results for the given slot.
  void on_cell_results_completion(slot_point slot) override
  {
    {
      std::lock_guard<std::mutex> lock(mutex);
      slot_ended = true;
    }
    cvar.notify_one();
  }

  /// \brief This function blocks waiting for DU-high to finish the handling of the last slot_indication.
  void wait_slot_complete()
  {
    std::unique_lock<std::mutex> lock(mutex);
    while (not slot_ended) {
      cvar.wait(lock);
    }
    slot_ended = false;
  }
};

/// \brief TestBench for the DU-high.
class du_high_bench
{
public:
  du_high_bench()
  {
    // Instantiate a DU-high object.
    cfg.du_mng_executor = &workers.ctrl_exec;
    cfg.cell_executors  = &workers.cell_exec_mapper;
    cfg.ue_executors    = &workers.ue_exec_mapper;
    cfg.f1c_notifier    = &sim_cu_cp;
    cfg.f1u_gw          = &sim_cu_up;
    cfg.phy_adapter     = &sim_phy;
    cfg.cells           = {config_helpers::make_default_du_cell_config()};
    cfg.sched_cfg       = config_helpers::make_default_scheduler_expert_config();
    du_hi               = std::make_unique<du_high>(cfg);

    // Connect CU back to DU.
    sim_cu_cp.ctrl_exec       = &workers.ctrl_exec;
    sim_cu_cp.f1c_msg_handler = &du_hi->get_f1c_message_handler();

    // Create PDCP PDU.
    pdcp_pdu.append(test_rgen::random_vector<uint8_t>(1000));

    // Start DU-high operation.
    du_hi->start();

    // Connect PHY back to DU-High.
    sim_phy.ctrl_info_handler = &du_hi->get_control_information_handler(to_du_cell_index(0));
  }

  ~du_high_bench() { workers.stop(); }

  /// \brief Run a slot indication until completion.
  void run_slot()
  {
    // Push slot indication to DU-high.
    du_hi->get_slot_handler(to_du_cell_index(0)).handle_slot_indication(next_sl_tx);

    // Wait DU-high to finish handling the slot.
    sim_phy.wait_slot_complete();

    ++next_sl_tx;
    slot_count++;
  }

  /// \brief Add a UE to the DU-high and wait for the DU-high to finish the setup of the UE.
  void add_ue(rnti_t rnti)
  {
    using namespace std::chrono_literals;

    // Received Msg3 with UL-CCCH message.
    mac_rx_data_indication rx_ind;
    rx_ind.sl_rx      = next_sl_tx - 4;
    rx_ind.cell_index = to_du_cell_index(0);
    rx_ind.pdus.push_back(mac_rx_pdu{rnti, 0, 0, {0x34, 0x1e, 0x4f, 0xc0, 0x4f, 0xa6, 0x06, 0x3f, 0x00, 0x00, 0x00}});
    du_hi->get_pdu_handler(to_du_cell_index(0)).handle_rx_data_indication(std::move(rx_ind));

    // Wait for UE Context Modification Response to arrive to CU.
    unsigned count = 0;
    for (; not sim_cu_cp.ue_created; ++count) {
      // Need to run one slot for scheduler to handle pending events.
      run_slot();
      std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
  }

  // \brief Push a DL PDU to DU-high via F1-U interface.
  void push_pdcp_pdu() { sim_cu_up.du_notif->on_new_sdu(pdcp_tx_pdu{.buf = pdcp_pdu.copy(), .pdcp_count = 0}); }

  du_high_configuration              cfg{};
  cu_cp_simulator                    sim_cu_cp;
  cu_up_simulator                    sim_cu_up;
  phy_simulator                      sim_phy;
  du_high_single_cell_worker_manager workers;
  std::unique_ptr<du_high>           du_hi;
  slot_point                         next_sl_tx{0, 0};
  unsigned                           slot_count;

  byte_buffer pdcp_pdu;
};

/// \brief Benchmark DU-high for 1 UE, DL only traffic using an RLC UM bearer.
void benchmark_one_ue_dl_only_rlc_um(benchmarker& bm)
{
  std::string         benchname = "DL only, 1 UE, RLC UM";
  test_delimit_logger test_delim(benchname.c_str());
  du_high_bench       bench;
  bench.add_ue(to_rnti(0x4601));

  // Run benchmark.
  bm.new_measure(benchname, 1, [&bench]() mutable {
    // Push DL PDU.
    bench.push_pdcp_pdu();

    // Run slot to completion.
    bench.run_slot();
  });

  fmt::print("Stats: slots={}, #PDSCHs={}, Tx={} MB\n",
             bench.slot_count,
             bench.sim_phy.nof_dl_grants,
             bench.sim_phy.nof_dl_bytes / 1.0e6);
}

int main(int argc, char** argv)
{
  // Set DU-high logging.
  srslog::fetch_basic_logger("RLC").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("MAC", true).set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("DU-F1").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("UE-MNG").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("DU-MNG").set_level(srslog::basic_levels::warning);
  srslog::init();

  // Parses benchmark parameters.
  bench_params params{};
  parse_args(argc, argv, params);

  // Start benchmarker.
  benchmarker bm("DU-High", params.nof_repetitions);

  // Run scenarios.
  benchmark_one_ue_dl_only_rlc_um(bm);

  // Output results.
  bm.print_percentiles_time();
}
