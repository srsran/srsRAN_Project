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
  srslog::sink* log_sink =
      (args.log_filename == "stdout") ? srslog::create_stdout_sink() : srslog::create_file_sink(args.log_filename);
  if (log_sink == nullptr) {
    return;
  }
  srslog::log_channel* chan = srslog::create_log_channel("main_channel", *log_sink);
  if (chan == nullptr) {
    return;
  }
  srslog::set_default_sink(*log_sink);

  timer_manager timers;

  rlc_config cnfg = {};
  if (args.mode == "TM") {
    cnfg.mode = srsgnb::rlc_mode::tm;
  } else if (args.mode == "UM6") {
    cnfg.mode                  = srsgnb::rlc_mode::um_bidir;
    cnfg.um.rx.sn_field_length = rlc_um_sn_size::size6bits;
    cnfg.um.tx.sn_field_length = rlc_um_sn_size::size6bits;
  } else if (args.mode == "UM12") {
    cnfg.mode                  = srsgnb::rlc_mode::um_bidir;
    cnfg.um.rx.sn_field_length = rlc_um_sn_size::size12bits;
    cnfg.um.tx.sn_field_length = rlc_um_sn_size::size12bits;
  } else if (args.mode == "AM12") {
    cnfg.mode                  = srsgnb::rlc_mode::am;
    cnfg.am.rx.sn_field_length = rlc_am_sn_size::size12bits;
    cnfg.am.tx.sn_field_length = rlc_am_sn_size::size12bits;
  } else if (args.mode == "AM18") {
    cnfg.mode                  = srsgnb::rlc_mode::am;
    cnfg.am.rx.sn_field_length = rlc_am_sn_size::size18bits;
    cnfg.am.tx.sn_field_length = rlc_am_sn_size::size18bits;
  } else {
    fprintf(stderr, "Unsupported RLC mode %s, exiting.\n", args.mode.c_str());
    exit(-1);
  }

  // RLC gNB
  std::unique_ptr<rlc_rx_upper_layer_data_notifier> pdcp_rx = std::make_unique<pdcp_rx_dummy>();
  rlc_entity_creation_message                       msg;
  msg.rx_upper_dn                 = pdcp_rx.get();
  msg.config                      = cnfg;
  msg.timers                      = &timers;
  std::unique_ptr<rlc_entity> rlc = create_rlc_entity(msg);

  // RLC UE

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
