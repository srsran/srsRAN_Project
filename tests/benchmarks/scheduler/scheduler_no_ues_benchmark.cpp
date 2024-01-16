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

#include "lib/scheduler/config/cell_configuration.h"
#include "scheduler_test_doubles.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/scheduler/scheduler_factory.h"
#include "srsran/support/benchmark_utils.h"
#include <getopt.h>

using namespace srsran;

struct bench_params {
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

static std::unique_ptr<benchmarker> bm;

void benchmark_sib_scheduling()
{
  sched_cfg_dummy_notifier       cfg_notif;
  sched_dummy_metric_notifier    metric_notif;
  std::unique_ptr<mac_scheduler> sch = create_scheduler(
      scheduler_config{config_helpers::make_default_scheduler_expert_config(), cfg_notif, metric_notif});

  // Add Cell.
  scheduler_expert_config                  sched_cfg    = config_helpers::make_default_scheduler_expert_config();
  sched_cell_configuration_request_message cell_cfg_msg = test_helpers::make_default_sched_cell_configuration_request();
  cell_configuration                       cell_cfg{sched_cfg, cell_cfg_msg};
  sch->handle_cell_configuration_request(cell_cfg_msg);

  auto& logger = srslog::fetch_basic_logger("SCHED", true);

  // Run benchmark.
  slot_point sl_tx{0, 0};
  bm->new_measure("SSB+SIB scheduling", 1, [&sch, &sl_tx, &logger]() mutable {
    logger.set_context(sl_tx.sfn(), sl_tx.slot_index());
    sch->slot_indication(sl_tx, to_du_cell_index(0));
    ++sl_tx;
  });
}

void benchmark_rach_scheduling()
{
  sched_cfg_dummy_notifier       cfg_notif;
  sched_dummy_metric_notifier    metric_notif;
  std::unique_ptr<mac_scheduler> sch = create_scheduler(
      scheduler_config{config_helpers::make_default_scheduler_expert_config(), cfg_notif, metric_notif});

  // Add Cell.
  scheduler_expert_config                  sched_cfg    = config_helpers::make_default_scheduler_expert_config();
  sched_cell_configuration_request_message cell_cfg_msg = test_helpers::make_default_sched_cell_configuration_request();
  cell_configuration                       cell_cfg{sched_cfg, cell_cfg_msg};
  sch->handle_cell_configuration_request(cell_cfg_msg);

  auto&                   logger = srslog::fetch_basic_logger("SCHED", true);
  slot_point              sl_tx{0, 0};
  rach_indication_message rach_ind = test_helpers::generate_rach_ind_msg(sl_tx - 4, to_rnti(0x4601));

  // Run benchmark.
  bm->new_measure("SSB+SIB+RACH scheduling", 1, [&sch, &sl_tx, &rach_ind, &logger]() mutable {
    logger.set_context(sl_tx.sfn(), sl_tx.slot_index());
    if (sl_tx.slot_index() != 0) {
      // Avoid slots with SIB1, otherwise there might not be space in PDCCH.
      sch->handle_rach_indication(rach_ind);
    }
    const sched_result* res = &sch->slot_indication(sl_tx, to_du_cell_index(0));

    // ack msg3s.
    if (not res->ul.puschs.empty()) {
      ul_crc_indication crc;
      crc.cell_index = to_du_cell_index(0);
      crc.sl_rx      = sl_tx - 4;
      for (const ul_sched_info& ulinfo : res->ul.puschs) {
        crc.crcs.push_back(ul_crc_pdu_indication{
            ulinfo.pusch_cfg.rnti, INVALID_DU_UE_INDEX, to_harq_id(ulinfo.pusch_cfg.harq_id), true});
      }
      sch->handle_crc_indication(crc);
    }

    // update slot.
    ++sl_tx;
    rach_ind.slot_rx++;
    rach_ind.occasions.back().preambles.back().tc_rnti =
        to_rnti(1 + (unsigned)rach_ind.occasions.back().preambles.back().tc_rnti);
  });
}

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("MAC", true).set_level(srslog::basic_levels::error);

  srslog::init();

  bench_params params{};
  parse_args(argc, argv, params);

  bm = std::make_unique<benchmarker>("scheduler", params.nof_repetitions);

  benchmark_sib_scheduling();
  benchmark_rach_scheduling();

  // Output results.
  bm->print_percentiles_time();
}
