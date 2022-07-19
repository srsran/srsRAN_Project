/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rlc_stress_test.h"
#include "srsgnb/rlc/rlc_config.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <random>

void stress_test(stress_test_args args)
{
  // auto log_sink =
  //     (args.log_filename == "stdout") ? srslog::create_stdout_sink() : srslog::create_file_sink(args.log_filename);
  // if (!log_sink) {
  //   return;
  // }
  // srslog::log_channel* chan = srslog::create_log_channel("main_channel", *log_sink);
  // if (!chan) {
  //  return;
  //}
  // srslog::set_default_sink(*log_sink);

  // srsgnb::rlc_config cnfg = {};
  // if (args.mode == "UM6") {
  //   cnfg_ = srsran::rlc_config_t::default_rlc_um_nr_config(6);
  // } else if (args.mode == "UM12") {
  //   cnfg_ = srsran::rlc_config_t::default_rlc_um_nr_config(12);
  // } else if (args.mode == "AM12") {
  //   cnfg_                       = srsran::rlc_config_t::default_rlc_am_nr_config(12);
  //   cnfg_.am_nr.max_retx_thresh = args.max_retx;
  // } else if (args.mode == "AM18") {
  //   cnfg_                       = srsran::rlc_config_t::default_rlc_am_nr_config(18);
  //   cnfg_.am_nr.max_retx_thresh = args.max_retx;
  // } else {
  //   std::cout << "Unsupported RLC mode " << args.mode << ", exiting." << std::endl;
  //   exit(-1);
  // }

  // Print and analyse metrics
  // TODO
}

int main(int argc, char** argv)
{
  // srsgnb_debug_handle_crash(argc, argv);

  stress_test_args args = {};
  parse_args(args, argc, argv);

  srslog::init();

  stress_test(args);

  exit(0);
}
