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

#include "lib/du_manager/ran_resource_management/du_pucch_resource_manager.h"
#include "scheduler_test_doubles.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "srsran/adt/circular_array.h"
#include "srsran/scheduler/scheduler_factory.h"
#include "srsran/support/benchmark_utils.h"
#include "srsran/support/math/gcd.h"
#include <getopt.h>

using namespace srsran;

struct bench_params {
  unsigned nof_repetitions        = 100;
  unsigned nof_ues                = 32;
  unsigned dl_bs                  = 1000000;
  unsigned max_dl_grants_per_slot = 100;
  bool     debug                  = false;
};

static void usage(const char* prog, const bench_params& params)
{
  fmt::print("Usage: {} [-R repetitions] [-u UEs] [-b DL buffer] [-m Max DL UEs per slot] [-d]\n", prog);
  fmt::print("\t-R Repetitions [Default {}]\n", params.nof_repetitions);
  fmt::print("\t-u Number of UEs [Default {}]\n", params.nof_ues);
  fmt::print("\t-b Average RLC DL buffer state [Default {}]\n", params.dl_bs);
  fmt::print("\t-m Maximum number of DL UE grants per slot [Default {}]\n", params.max_dl_grants_per_slot);
  fmt::print("\t-d Debug mode [Default {}]\n", params.debug);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv, bench_params& params)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:u:b:m:d:h")) != -1) {
    switch (opt) {
      case 'R':
        params.nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'u':
        params.nof_ues = std::strtol(optarg, nullptr, 10);
        break;
      case 'b':
        params.dl_bs = std::strtol(optarg, nullptr, 10);
        break;
      case 'm':
        params.max_dl_grants_per_slot = std::strtol(optarg, nullptr, 10);
        break;
      case 'd':
        params.debug = std::strtol(optarg, nullptr, 10) > 0;
        break;
      case 'h':
      default:
        usage(argv[0], params);
        exit(0);
    }
  }
}

class multi_ue_sched_simulator
{
public:
  multi_ue_sched_simulator(const scheduler_expert_config&    expert_cfg_,
                           const cell_config_builder_params& builder_params_) :
    expert_cfg(expert_cfg_),
    builder_params(builder_params_),
    logger(srslog::fetch_basic_logger("SCHED")),
    sch(create_scheduler(scheduler_config{expert_cfg, cfg_notif, metric_notif})),
    next_sl_tx(builder_params.scs_common, 0)
  {
    du_cell_cfgs                                       = {config_helpers::make_default_du_cell_config(builder_params)};
    du_cell_cfgs[0].pucch_cfg.f2_params.max_code_rate  = max_pucch_code_rate::dot_35;
    du_cell_cfgs[0].pucch_cfg.nof_csi_resources        = 2;
    du_cell_cfgs[0].pucch_cfg.nof_sr_resources         = 2;
    du_cell_cfgs[0].pucch_cfg.nof_ue_pucch_f1_res_harq = 3;
    du_cell_cfgs[0].pucch_cfg.nof_ue_pucch_f2_res_harq = 6;

    sched_cell_configuration_request_message cell_cfg_msg =
        test_helpers::make_default_sched_cell_configuration_request(builder_params);

    cell_cfg_msg.pucch_guardbands = config_helpers::build_pucch_guardbands_list(
        du_cell_cfgs[0].pucch_cfg, cell_cfg_msg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length());
    sch->handle_cell_configuration_request(cell_cfg_msg);

    du_cell_cfgs                                      = {config_helpers::make_default_du_cell_config(builder_params)};
    du_cell_cfgs[0].pucch_cfg.f2_params.max_code_rate = max_pucch_code_rate::dot_35;
    du_cell_cfgs[0].pucch_cfg.nof_csi_resources       = 4;
    pucch_res_mng.emplace(du_cell_cfgs);

    logger.set_context(next_sl_tx.sfn(), next_sl_tx.slot_index());
  }

  void add_ue()
  {
    sched_ue_creation_request_message ue_cfg_msg = test_helpers::create_default_sched_ue_creation_request(
        builder_params, {lcid_t::LCID_SRB2, lcid_t::LCID_MIN_DRB});
    ue_cfg_msg.ue_index           = to_du_ue_index(ue_count);
    ue_cfg_msg.crnti              = to_rnti(0x4601 + ue_count);
    ue_cfg_msg.starts_in_fallback = false;

    auto& pcell = (*ue_cfg_msg.cfg.cells)[0];

    // Generate PUCCH resources for the UE.
    srs_du::cell_group_config cell_group;
    cell_group.cells.emplace(0);
    cell_group.cells[0].serv_cell_cfg = pcell.serv_cell_cfg;
    bool success                      = pucch_res_mng->alloc_resources(cell_group);
    srsran_assert(success, "Failed to allocate resources for UE={}", ue_count);
    pcell.serv_cell_cfg = cell_group.cells[0].serv_cell_cfg;

    sch->handle_ue_creation_request(ue_cfg_msg);
    ue_count++;
  }

  void push_dl_bs(unsigned dl_bs_val)
  {
    for (unsigned ue_index = 0; ue_index != ue_count; ++ue_index) {
      dl_buffer_state_indication_message dl_bs{};
      dl_bs.ue_index = to_du_ue_index(ue_index);
      dl_bs.lcid     = lcid_t::LCID_MIN_DRB;
      dl_bs.bs       = dl_bs_val;
      sch->handle_dl_buffer_state_indication(dl_bs);
    }
  }

  void run_slot() { result = &sch->slot_indication(next_sl_tx, to_du_cell_index(0)); }

  void process_results()
  {
    slot_point result_sl_tx = next_sl_tx;
    ++next_sl_tx;

    // Store previous results if any.
    if (result != nullptr) {
      sched_results[result_sl_tx.to_uint()] = *result;
    }

    // Process past UCI results.
    slot_point          sl_rx = next_sl_tx - dl_pipeline_delay - uci_process_delay;
    const sched_result& res   = sched_results[sl_rx.to_uint()];
    if (res.success) {
      uci_indication ind;
      ind.slot_rx    = sl_rx;
      ind.cell_index = to_du_cell_index(0);
      for (const auto& pucch : res.ul.pucchs) {
        uci_indication::uci_pdu pdu;
        pdu.ue_index = to_du_ue_index(static_cast<unsigned>(pucch.crnti) - 0x4601);
        pdu.crnti    = pucch.crnti;
        if (pucch.format == pucch_format::FORMAT_1) {
          uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu f1{};
          f1.sr_detected = false;
          if (pucch.format_1.harq_ack_nof_bits > 0 and pucch.format_1.sr_bits == sr_nof_bits::no_sr) {
            f1.ul_sinr = 10;
            f1.harqs.resize(pucch.format_1.harq_ack_nof_bits, mac_harq_ack_report_status::ack);
          } else if (pucch.format_1.harq_ack_nof_bits > 0) {
            // ACK+SR
            f1.ul_sinr = -10;
            f1.harqs.resize(pucch.format_1.harq_ack_nof_bits, mac_harq_ack_report_status::dtx);
          } else {
            f1.ul_sinr     = -10;
            f1.sr_detected = false;
          }
          pdu.pdu = f1;
          ind.ucis.push_back(pdu);
        } else if (pucch.format == pucch_format::FORMAT_2) {
          uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu f2{};
          f2.ul_sinr = 10;
          if (pucch.format_2.harq_ack_nof_bits > 0) {
            f2.harqs.resize(pucch.format_2.harq_ack_nof_bits, mac_harq_ack_report_status::ack);
          }
          if (pucch.csi_rep_cfg.has_value()) {
            f2.csi = csi_report_data{nullopt,
                                     4,
                                     nullopt,
                                     csi_report_pmi{csi_report_pmi::typeI_single_panel_4ports_mode1{0, nullopt, 0}},
                                     15,
                                     nullopt};
          }
          pdu.pdu = f2;
          ind.ucis.push_back(pdu);
        }
      }
      for (const ul_sched_info& ul_grant : res.ul.puschs) {
        if (ul_grant.uci.has_value()) {
          uci_indication::uci_pdu pdu;
          pdu.ue_index = to_du_ue_index(static_cast<unsigned>(ul_grant.pusch_cfg.rnti) - 0x4601);
          pdu.crnti    = ul_grant.pusch_cfg.rnti;
          uci_indication::uci_pdu::uci_pusch_pdu pusch_pdu{};
          pusch_pdu.harqs.resize(ul_grant.uci->harq_ack_nof_bits, mac_harq_ack_report_status::ack);
          pusch_pdu.csi =
              csi_report_data{nullopt,
                              4,
                              nullopt,
                              csi_report_pmi{csi_report_pmi::typeI_single_panel_4ports_mode1{0, nullopt, 0}},
                              15,
                              nullopt};
          ind.ucis.push_back(pdu);
        }
      }

      // Forward UCI indication to scheduler.
      sch->handle_uci_indication(ind);
    }

    logger.set_context(next_sl_tx.sfn(), next_sl_tx.slot_index());
  }

  mac_scheduler& sched() const { return *sch; }

private:
  const unsigned dl_pipeline_delay = 4;
  const unsigned uci_process_delay = 2;

  sched_cfg_dummy_notifier                    cfg_notif;
  sched_dummy_metric_notifier                 metric_notif;
  scheduler_expert_config                     expert_cfg;
  cell_config_builder_params                  builder_params;
  std::vector<du_cell_config>                 du_cell_cfgs;
  srslog::basic_logger&                       logger;
  optional<srs_du::du_pucch_resource_manager> pucch_res_mng;

  std::unique_ptr<mac_scheduler> sch;

  unsigned            ue_count = 0;
  slot_point          next_sl_tx;
  const sched_result* result = nullptr;

  circular_array<sched_result, 64> sched_results;
};

void benchmark_tdd(benchmarker& bm, const bench_params& params)
{
  scheduler_expert_config sched_cfg              = config_helpers::make_default_scheduler_expert_config();
  sched_cfg.ue.max_pdcch_alloc_attempts_per_slot = params.max_dl_grants_per_slot;

  cell_config_builder_params builder_params{};
  builder_params.dl_arfcn             = 520002;
  builder_params.band                 = nr_band::n41;
  builder_params.channel_bw_mhz       = bs_channel_bandwidth_fr1::MHz100;
  builder_params.scs_common           = subcarrier_spacing::kHz30;
  builder_params.tdd_ul_dl_cfg_common = tdd_ul_dl_config_common{builder_params.scs_common, {10, 7, 8, 2, 0}};
  builder_params.nof_dl_ports         = 4;
  multi_ue_sched_simulator sim{sched_cfg, builder_params};

  // Add UEs.
  for (unsigned ue_count = 0; ue_count != params.nof_ues; ++ue_count) {
    sim.add_ue();
  }

  // Update UE buffer states.
  sim.push_dl_bs(params.dl_bs);

  // Run benchmark.
  bm.new_measure(
      fmt::format("TDD scheduling {} UEs", params.nof_ues),
      1,
      [&sim]() mutable { sim.run_slot(); },
      [&]() {
        sim.process_results();
        sim.push_dl_bs(params.dl_bs);
      });
}

int main(int argc, char** argv)
{
  srslog::init();

  bench_params params{};
  parse_args(argc, argv, params);

  srslog::fetch_basic_logger("SCHED", true)
      .set_level(params.debug ? srslog::basic_levels::debug : srslog::basic_levels::warning);

  std::unique_ptr<benchmarker> bm = std::make_unique<benchmarker>("scheduler", params.nof_repetitions);

  benchmark_tdd(*bm, params);

  // Output results.
  bm->print_percentiles_time();
}
