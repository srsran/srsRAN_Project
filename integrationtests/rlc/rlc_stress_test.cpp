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
#include "srsgnb/rlc/rlc_factory.h"
#include <random>

using namespace srsgnb;

void stress_test(stress_test_args args)
{
  auto log_sink =
      (args.log_filename == "stdout") ? srslog::create_stdout_sink() : srslog::create_file_sink(args.log_filename);
  if (!log_sink) {
    return;
  }
  srslog::log_channel* chan = srslog::create_log_channel("main_channel", *log_sink);
  if (!chan) {
    return;
  }
  srslog::set_default_sink(*log_sink);

  //  rlc_config cnfg = {};
  //  if (args.mode == "TM") {
  //    cnfg.mode = srsgnb::rlc_mode::tm;
  //  } else if (args.mode == "UM") {
  //    cnfg.mode = srsgnb::rlc_mode::um_bidir;
  //  } else if (args.mode == "AM") {
  //    cnfg.mode = srsgnb::rlc_mode::am;
  //  } else {
  //    fprintf(stderr, "Unsupported RLC mode %s, exiting.\n", args.mode.c_str());
  //    exit(-1);
  //  }

  // RLC 1
  // rlc_entity_creation_message msg;
  // msg.config                      = cnfg;
  // std::unique_ptr<rlc_entity> rlc = create_rlc_entity(msg);
  //  Print and analyse metrics
  //  TODO
}

int main(int argc, char** argv)
{
  // srsgnb_debug_handle_crash(argc, argv);

  stress_test_args args = {};
  if (not parse_args(args, argc, argv)) {
    fprintf(stderr, "Could not parse command line options\n");
    return -1;
  }

  srslog::init();

  stress_test(args);

  return 0;
}
