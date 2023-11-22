/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Benchmarks of the DU-high latency.

#include "lib/du_high/du_high_executor_strategies.h"
#include "lib/du_high/du_high_impl.h"
#include "tests/unittests/f1ap/du/f1ap_du_test_helpers.h"
#include "tests/unittests/mac/mac_test_helpers.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/du_high/du_high_configuration.h"
#include "srsran/f1u/du/f1u_gateway.h"
#include "srsran/support/benchmark_utils.h"
#include "srsran/support/event_tracing.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/test_utils.h"

using namespace srsran;
using namespace srs_du;

/// \brief Parameters of the benchmark.
struct bench_params {
  /// \brief Number of runs for the benchmark. Each repetition corresponds to a slot.
  unsigned nof_repetitions = 100;
  /// \brief Number to DU UEs to consider for benchmark.
  unsigned nof_ues = 1;
  /// \brief Set duplex mode (FDD or TDD) to use for benchmark.
  duplex_mode dplx_mode = duplex_mode::FDD;
};

static void usage(const char* prog, const bench_params& params)
{
  fmt::print("Usage: {} [-R repetitions] [-U nof. ues] [-D Duplex mode] [-s silent]\n", prog);
  fmt::print("\t-R Repetitions [Default {}]\n", params.nof_repetitions);
  fmt::print("\t-U Nof. DU UEs [Default {}]\n", params.nof_ues);
  fmt::print("\t-D Duplex mode (FDD/TDD) [Default {}]\n", to_string(params.dplx_mode));
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv, bench_params& params)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:U:D:h")) != -1) {
    switch (opt) {
      case 'R':
        params.nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'U':
        params.nof_ues = std::strtol(optarg, nullptr, 10);
        break;
      case 'D': {
        if (std::string(optarg) == "FDD") {
          params.dplx_mode = duplex_mode::FDD;
        } else if (std::string(optarg) == "TDD") {
          params.dplx_mode = duplex_mode::TDD;
        } else {
          usage(argv[0], params);
          exit(0);
        }
        break;
      }
      case 'h':
      default:
        usage(argv[0], params);
        exit(0);
    }
  }
}

/// \brief Simulator of the CU-CP from the perspective of the DU. This class should reply to the F1AP messages
/// that the DU sends in order for the DU normal operation to proceed.
class cu_cp_simulator : public srs_du::f1c_connection_client
{
public:
  cu_cp_simulator() : du_rx_pdu_notifier(nullptr)
  {
    for (auto& flag : ue_created_flag_list) {
      flag.store(false, std::memory_order_relaxed);
    }
  }

private:
  class f1ap_du_tx_pdu_notifier : public f1ap_message_notifier
  {
  public:
    f1ap_du_tx_pdu_notifier(cu_cp_simulator& parent_) : parent(parent_) {}

    void on_new_message(const f1ap_message& msg) override { parent.handle_message(msg); }

  private:
    cu_cp_simulator& parent;
  };

public:
  std::unique_ptr<f1ap_message_notifier>        du_rx_pdu_notifier;
  std::array<std::atomic<bool>, MAX_NOF_DU_UES> ue_created_flag_list;
  unsigned                                      next_gnb_cu_ue_f1ap_id = 0;

  std::unique_ptr<f1ap_message_notifier>
  handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier_) override
  {
    du_rx_pdu_notifier = std::move(du_rx_pdu_notifier_);
    return std::make_unique<f1ap_du_tx_pdu_notifier>(*this);
  }

private:
  unsigned get_next_gnb_cu_ue_f1ap_id() { return next_gnb_cu_ue_f1ap_id++; }

  void handle_message(const f1ap_message& msg)
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

  void handle_init_msg(const asn1::f1ap::init_msg_s& init_msg)
  {
    switch (init_msg.value.type().value) {
      case asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request: {
        f1ap_message f1ap_msg;
        f1ap_msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
        f1ap_msg.pdu.successful_outcome().value.f1_setup_resp()->cells_to_be_activ_list_present = true;
        f1ap_msg.pdu.successful_outcome().value.f1_setup_resp()->cells_to_be_activ_list.resize(1);
        du_rx_pdu_notifier->on_new_message(f1ap_msg);
      } break;
      case asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::init_ul_rrc_msg_transfer: {
        // Send UE Context Modification to create DRB1.
        f1ap_message msg = generate_ue_context_modification_request({drb_id_t::drb1});
        // Note: Use UM because AM requires status PDUs.
        auto& drb1 = msg.pdu.init_msg()
                         .value.ue_context_mod_request()
                         ->drbs_to_be_setup_mod_list[0]
                         ->drbs_to_be_setup_mod_item();
        drb1.rlc_mode.value = asn1::f1ap::rlc_mode_opts::rlc_um_bidirectional;
        drb1.qos_info.choice_ext()->drb_info().drb_qos.qos_characteristics.non_dyn_5qi().five_qi =
            7; // UM in default configs
        msg.pdu.init_msg().value.ue_context_mod_request()->gnb_cu_ue_f1ap_id = get_next_gnb_cu_ue_f1ap_id();
        msg.pdu.init_msg().value.ue_context_mod_request()->gnb_du_ue_f1ap_id =
            init_msg.value.init_ul_rrc_msg_transfer()->gnb_du_ue_f1ap_id;
        du_rx_pdu_notifier->on_new_message(msg);
      } break;
      default:
        report_fatal_error("Unreachable code in this benchmark");
    }
  }

  void handle_success_outcome(const asn1::f1ap::successful_outcome_s& succ_outcome)
  {
    switch (succ_outcome.value.type().value) {
      case asn1::f1ap::f1ap_elem_procs_o::successful_outcome_c::types_opts::ue_context_mod_resp: {
        ue_created_flag_list[succ_outcome.value.ue_context_mod_resp()->gnb_du_ue_f1ap_id].store(
            true, std::memory_order_relaxed);
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
  void handle_sdu(byte_buffer_chain sdu) override {}
};

/// \brief Simulator of the CU-UP from the perspective of the DU.
class cu_up_simulator : public f1u_du_gateway
{
public:
  static_vector<f1u_dummy_bearer, MAX_NOF_DU_UES>             bearer_list;
  static_vector<srs_du::f1u_rx_sdu_notifier*, MAX_NOF_DU_UES> du_notif_list;

  f1u_bearer* create_du_bearer(uint32_t                       ue_index,
                               drb_id_t                       drb_id,
                               srs_du::f1u_config             config,
                               const up_transport_layer_info& dl_tnl,
                               const up_transport_layer_info& ul_tnl,
                               srs_du::f1u_rx_sdu_notifier&   du_rx,
                               timer_factory                  timers) override
  {
    du_notif_list.push_back(&du_rx);
    bearer_list.emplace_back();
    return &bearer_list.back();
  }

  void remove_du_bearer(const up_transport_layer_info& dl_tnl) override {}
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

  task_worker                  ctrl_worker{"CTRL", task_worker_queue_size};
  task_worker                  dl_worker{"DU-DL#0", task_worker_queue_size};
  task_worker                  ul_worker{"DU-UL#0", task_worker_queue_size};
  task_worker_executor         ctrl_exec{ctrl_worker};
  task_worker_executor         dl_exec{dl_worker};
  task_worker_executor         ul_exec{ul_worker};
  du_high_executor_mapper_impl du_high_exec_mapper{
      std::make_unique<cell_executor_mapper>(std::initializer_list<task_executor*>{&dl_exec}),
      std::make_unique<pcell_ue_executor_mapper>(std::initializer_list<task_executor*>{&ul_exec}),
      ctrl_exec,
      ctrl_exec,
      ctrl_exec};
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

  mac_dl_sched_result slot_dl_result;
  mac_ul_sched_result slot_ul_result;

  unsigned nof_dl_grants = 0;
  uint64_t nof_dl_bytes  = 0;

  void clear_previous_scheduler_results()
  {
    slot_dl_result.dl_res = nullptr;
    slot_ul_result.ul_res = nullptr;
  }

  mac_cell_result_notifier& get_cell(du_cell_index_t cell_index) override { return *this; }

  void on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res) override { slot_dl_result = dl_res; }

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
  void on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res) override { slot_ul_result = ul_res; }

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
  du_high_bench(const cell_config_builder_params& builder_params = {}) : params(builder_params)
  {
    // Set slot point based on the SCS.
    next_sl_tx = slot_point{to_numerology_value(params.scs_common), 0};

    // Instantiate a DU-high object.
    cfg.gnb_du_id    = 1;
    cfg.gnb_du_name  = fmt::format("srsgnb{}", cfg.gnb_du_id);
    cfg.du_bind_addr = fmt::format("127.0.0.{}", cfg.gnb_du_id);
    cfg.exec_mapper  = &workers.du_high_exec_mapper;
    cfg.f1c_client   = &sim_cu_cp;
    cfg.f1u_gw       = &sim_cu_up;
    cfg.phy_adapter  = &sim_phy;
    cfg.timers       = &timers;
    cfg.cells        = {config_helpers::make_default_du_cell_config(params)};
    cfg.sched_cfg    = config_helpers::make_default_scheduler_expert_config();
    cfg.qos          = config_helpers::make_default_du_qos_config_list(1000);
    cfg.mac_p        = &mac_pcap;
    cfg.rlc_p        = &rlc_pcap;
    du_hi            = std::make_unique<du_high_impl>(cfg);

    // Create PDCP PDU.
    pdcp_pdu.append(test_rgen::random_vector<uint8_t>(1500));

    // Start DU-high operation.
    du_hi->start();

    // Connect PHY back to DU-High.
    sim_phy.ctrl_info_handler = &du_hi->get_control_info_handler(to_du_cell_index(0));
  }

  void stop()
  {
    du_hi->stop();
    workers.stop();
  }

  ~du_high_bench() { stop(); }

  /// \brief Run a slot indication until completion.
  void run_slot()
  {
    // Clear results.
    sim_phy.clear_previous_scheduler_results();

    // Push slot indication to DU-high.
    du_hi->get_slot_handler(to_du_cell_index(0)).handle_slot_indication(next_sl_tx);

    // Wait DU-high to finish handling the slot.
    sim_phy.wait_slot_complete();
  }

  template <typename StopCondition>
  bool run_slot_until(const StopCondition& cond_func, unsigned slot_timeout = 1000)
  {
    unsigned count = 0;
    for (; count < slot_timeout; ++count) {
      run_slot();
      process_results();
      if (cond_func()) {
        break;
      }
    }
    return count < slot_timeout;
  }

  void process_results()
  {
    // Forwards HARQ-ACKs to the DU-High given the scheduled PUCCHs.
    mac_uci_indication_message uci{};
    uci.sl_rx = sim_phy.slot_ul_result.slot;
    if (sim_phy.slot_ul_result.ul_res != nullptr) {
      for (const pucch_info& pucch : sim_phy.slot_ul_result.ul_res->pucchs) {
        mac_uci_pdu& uci_pdu = uci.ucis.emplace_back();
        uci_pdu.rnti         = pucch.crnti;
        switch (pucch.format) {
          case pucch_format::FORMAT_1: {
            mac_uci_pdu::pucch_f0_or_f1_type f1{};
            if (pucch.format_1.harq_ack_nof_bits > 0) {
              f1.harq_info.emplace();
              // Set PUCCHs with SR as DTX.
              const uci_pucch_f0_or_f1_harq_values ack_val = pucch.format_1.sr_bits == sr_nof_bits::no_sr
                                                                 ? uci_pucch_f0_or_f1_harq_values::ack
                                                                 : uci_pucch_f0_or_f1_harq_values::dtx;
              f1.harq_info->harqs.resize(pucch.format_1.harq_ack_nof_bits, ack_val);
            }
            // Do not forward positive SRs to scheduler.
            if (pucch.format_1.sr_bits != sr_nof_bits::no_sr) {
              f1.sr_info.emplace();
              f1.sr_info->sr_detected = false;
            }
            uci_pdu.pdu = f1;
          } break;
          case pucch_format::FORMAT_2: {
            mac_uci_pdu::pucch_f2_or_f3_or_f4_type f2{};
            if (pucch.format_2.harq_ack_nof_bits > 0) {
              f2.harq_info.emplace();
              f2.harq_info->harq_status = uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass;
              f2.harq_info->payload.resize(pucch.format_2.harq_ack_nof_bits);
              f2.harq_info->payload.fill(0, pucch.format_2.harq_ack_nof_bits, true);
            }
            if (pucch.format_2.sr_bits != sr_nof_bits::no_sr) {
              f2.sr_info.emplace();
              f2.sr_info->resize(sr_nof_bits_to_uint(pucch.format_2.sr_bits));
              f2.sr_info->fill(0, sr_nof_bits_to_uint(pucch.format_2.sr_bits), true);
            }
            if (pucch.csi_rep_cfg.has_value()) {
              f2.uci_part1_or_csi_part1_info.emplace();
              f2.uci_part1_or_csi_part1_info->status       = uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass;
              f2.uci_part1_or_csi_part1_info->payload_type = mac_uci_pdu::pucch_f2_or_f3_or_f4_type::
                  uci_payload_or_csi_information::payload_type_t::csi_part_payload;
              f2.uci_part1_or_csi_part1_info->payload.resize(4);
              f2.uci_part1_or_csi_part1_info->payload.fill(0, 4, true);
            }
            uci_pdu.pdu = f2;
          } break;
          default:
            report_fatal_error("PUCCH format not supported");
        }
      }
      if (not sim_phy.slot_ul_result.ul_res->pucchs.empty()) {
        pending_ucis.push_back(uci);
      }
    }
    for (const auto& uci_ind : pending_ucis) {
      if (uci_ind.sl_rx == next_sl_tx - tx_rx_delay) {
        du_hi->get_control_info_handler(to_du_cell_index(0)).handle_uci(uci_ind);
      }
    }

    // Advance slot.
    ++next_sl_tx;
    slot_count++;
  }

  /// \brief Add a UE to the DU-high and wait for the DU-high to finish the setup of the UE.
  void add_ue(du_ue_index_t ue_idx)
  {
    using namespace std::chrono_literals;

    // Wait until it's a full UL slot to send Msg3.
    auto next_msg3_opportunity_condition = [this]() {
      return not cfg.cells[to_du_cell_index(0)].tdd_ul_dl_cfg_common.has_value() or
             not is_tdd_full_ul_slot(cfg.cells[to_du_cell_index(0)].tdd_ul_dl_cfg_common.value(),
                                     slot_point(next_sl_tx - tx_rx_delay - 1).slot_index());
    };
    run_slot_until(next_msg3_opportunity_condition);

    // Received Msg3 with UL-CCCH message.
    mac_rx_data_indication rx_ind;
    rx_ind.sl_rx      = next_sl_tx - tx_rx_delay;
    rx_ind.cell_index = to_du_cell_index(0);
    rx_ind.pdus.push_back(
        mac_rx_pdu{to_rnti(0x4601 + ue_idx), 0, 0, {0x34, 0x1e, 0x4f, 0xc0, 0x4f, 0xa6, 0x06, 0x3f, 0x00, 0x00, 0x00}});
    du_hi->get_pdu_handler().handle_rx_data_indication(std::move(rx_ind));

    // Wait for UE Context Modification Response to arrive to CU.
    while (not sim_cu_cp.ue_created_flag_list[ue_idx]) {
      // Need to run one slot for scheduler to handle pending events.
      run_slot();
      process_results();
      std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
  }

  // \brief Push a DL PDUs to DU-high via F1-U interface.
  void push_pdcp_pdus()
  {
    for (const auto& du_notif : sim_cu_up.du_notif_list) {
      for (unsigned i = 0; i != 10; ++i) {
        du_notif->on_new_sdu(pdcp_tx_pdu{.buf = pdcp_pdu.copy(), .pdcp_sn = 0});
      }
    }
  }

  unsigned                               tx_rx_delay = 4;
  cell_config_builder_params             params;
  du_high_configuration                  cfg{};
  cu_cp_simulator                        sim_cu_cp;
  cu_up_simulator                        sim_cu_up;
  phy_simulator                          sim_phy;
  timer_manager                          timers;
  du_high_single_cell_worker_manager     workers;
  std::unique_ptr<du_high_impl>          du_hi;
  slot_point                             next_sl_tx{0, 0};
  unsigned                               slot_count = 0;
  null_mac_pcap                          mac_pcap;
  null_rlc_pcap                          rlc_pcap;
  std::deque<mac_uci_indication_message> pending_ucis;

  byte_buffer pdcp_pdu;
};

/// \brief Generate custom cell configuration builder params based on duplex mode.
static cell_config_builder_params generate_custom_cell_config_builder_params(duplex_mode dplx_mode)
{
  cell_config_builder_params params{};
  params.scs_common       = dplx_mode == duplex_mode::FDD ? subcarrier_spacing::kHz15 : subcarrier_spacing::kHz30;
  params.dl_arfcn         = dplx_mode == duplex_mode::FDD ? 530000 : 520002;
  params.band             = band_helper::get_band_from_dl_arfcn(params.dl_arfcn);
  params.channel_bw_mhz   = bs_channel_bandwidth_fr1::MHz20;
  const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
      params.channel_bw_mhz, params.scs_common, band_helper::get_freq_range(*params.band));
  static const uint8_t                              ss0_idx = 0;
  optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc =
      band_helper::get_ssb_coreset0_freq_location(params.dl_arfcn,
                                                  *params.band,
                                                  nof_crbs,
                                                  params.scs_common,
                                                  params.scs_common,
                                                  ss0_idx,
                                                  params.max_coreset0_duration);
  if (!ssb_freq_loc.has_value()) {
    report_error("Unable to derive a valid SSB pointA and k_SSB for cell id ({}).\n", params.pci);
  }
  params.offset_to_point_a   = (*ssb_freq_loc).offset_to_point_A;
  params.k_ssb               = (*ssb_freq_loc).k_ssb;
  params.coreset0_index      = (*ssb_freq_loc).coreset0_idx;
  params.search_space0_index = ss0_idx;

  return params;
}

/// \brief Benchmark DU-high with DL only traffic using an RLC UM bearer.
void benchmark_dl_only_rlc_um(benchmarker& bm, unsigned nof_ues, duplex_mode dplx_mode)
{
  auto                benchname = fmt::format("DL only, {} UE, RLC UM", nof_ues);
  test_delimit_logger test_delim(benchname.c_str());
  du_high_bench       bench{generate_custom_cell_config_builder_params(dplx_mode)};
  for (unsigned ue_count = 0; ue_count < nof_ues; ++ue_count) {
    bench.add_ue(to_du_ue_index(ue_count));
  }

  // Run benchmark.
  bm.new_measure(
      benchname,
      1,
      [&bench]() mutable {
        // Run slot to completion.
        bench.run_slot();
      },
      [&bench]() {
        // Push DL PDUs.
        bench.push_pdcp_pdus();

        // Advance slot
        bench.process_results();
      });

  // Stop benchmark.
  bench.stop();
  srslog::flush();

  const unsigned sim_time_msec = bench.slot_count / get_nof_slots_per_subframe(bench.cfg.cells[0].scs_common);
  fmt::print("Stats: slots={}, #PDSCHs={}, dl_bitrate={:.2} Mbps\n",
             bench.slot_count,
             bench.sim_phy.nof_dl_grants,
             bench.sim_phy.nof_dl_bytes * 8 * 1.0e-6 / (sim_time_msec * 1.0e-3));
}

int main(int argc, char** argv)
{
  // Set DU-high logging.
  srslog::fetch_basic_logger("RLC").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("MAC", true).set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("SCHED", true).set_level(srslog::basic_levels::warning);
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
  benchmark_dl_only_rlc_um(bm, params.nof_ues, params.dplx_mode);

  // Output results.
  bm.print_percentiles_time();
}
