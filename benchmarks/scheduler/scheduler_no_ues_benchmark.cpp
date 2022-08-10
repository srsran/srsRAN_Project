/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/cell/cell_configuration.h"
#include "srsgnb/scheduler/scheduler_factory.h"
#include "srsgnb/support/benchmark_utils.h"
#include "unittests/scheduler/config_generators.h"
#include <getopt.h>

using namespace srsgnb;

class sched_cfg_dummy_notifier : public sched_configuration_notifier
{
public:
  void on_ue_config_complete(du_ue_index_t ue_index) override {}
  void on_ue_delete_response(du_ue_index_t ue_index) override {}
};

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

std::unique_ptr<benchmarker> bm;

void benchmark_sib_scheduling()
{
  sched_cfg_dummy_notifier       cfg_notif;
  std::unique_ptr<mac_scheduler> sch = create_scheduler(cfg_notif);

  // Add Cell.
  sched_cell_configuration_request_message cell_cfg_msg = make_default_sched_cell_configuration_request();
  cell_configuration                       cell_cfg{cell_cfg_msg};
  sch->handle_cell_configuration_request(cell_cfg_msg);

  // Run benchmark.
  slot_point sl_tx{0, 0};
  bm->new_measure("SIB scheduling", 1, [&sch, &sl_tx]() mutable {
    sch->slot_indication(sl_tx, to_du_cell_index(0));
    ++sl_tx;
  });

  // Output results.
  bm->print_percentiles_time();
}

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::error);

  srslog::init();

  bench_params params{};
  parse_args(argc, argv, params);

  bm = std::make_unique<benchmarker>("scheduler", params.nof_repetitions);

  benchmark_sib_scheduling();
}
