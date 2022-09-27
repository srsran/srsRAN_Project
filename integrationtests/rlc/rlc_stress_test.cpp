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
  rlc_config    cnfg = get_rlc_config_from_args(args);

  // MAC12: RLC1->MAC12->RLC2
  std::unique_ptr<mac_dummy> mac12 = std::make_unique<mac_dummy>(args);

  // MAC21: RLC2->MAC21->RLC1
  std::unique_ptr<mac_dummy> mac21 = std::make_unique<mac_dummy>(args);

  // RLC1
  std::unique_ptr<pdcp_traffic_sink>   traffic_sink1   = std::make_unique<pdcp_traffic_sink>();
  std::unique_ptr<pdcp_traffic_source> traffic_source1 = std::make_unique<pdcp_traffic_source>(args);
  rlc_entity_creation_message          msg1            = {};
  msg1.rx_upper_dn                                     = traffic_sink1.get();
  msg1.tx_upper_cn                                     = traffic_source1.get();
  msg1.tx_upper_dn                                     = traffic_source1.get();
  msg1.tx_lower_dn                                     = mac12.get();
  msg1.config                                          = cnfg;
  msg1.timers                                          = &timers;
  std::unique_ptr<rlc_entity> rlc1                     = create_rlc_entity(msg1);
  traffic_source1->set_rlc_tx_upper(rlc1->get_tx_upper_layer_data_interface());
  mac12->set_rlc_tx_lower(rlc1->get_tx_lower_layer_interface());
  mac21->set_rlc_rx_lower(rlc1->get_rx_lower_layer_interface());

  // RLC2
  std::unique_ptr<pdcp_traffic_sink>   traffic_sink2   = std::make_unique<pdcp_traffic_sink>();
  std::unique_ptr<pdcp_traffic_source> traffic_source2 = std::make_unique<pdcp_traffic_source>(args);
  rlc_entity_creation_message          msg2            = {};
  msg2.rx_upper_dn                                     = traffic_sink2.get();
  msg2.tx_upper_cn                                     = traffic_source2.get();
  msg2.tx_upper_dn                                     = traffic_source2.get();
  msg2.tx_lower_dn                                     = mac21.get();
  msg2.config                                          = cnfg;
  msg2.timers                                          = &timers;
  std::unique_ptr<rlc_entity> rlc2                     = create_rlc_entity(msg2);
  traffic_source2->set_rlc_tx_upper(rlc2->get_tx_upper_layer_data_interface());
  mac21->set_rlc_tx_lower(rlc2->get_tx_lower_layer_interface());
  mac12->set_rlc_rx_lower(rlc2->get_rx_lower_layer_interface());

  // Launch transmission
  // Fixme: create worker thread and executer
  for (uint32_t n = 0; n < 100; n++) {
    traffic_source1->send_pdu();
    traffic_source2->send_pdu();
  }

  while (rlc1->get_tx_lower_layer_interface()->get_buffer_state() > 0) {
    // RLC1->MAC12->RLC2
    mac12->run_tx_tti();
    mac12->run_rx_tti();

    // RLC2->MAC21->RLC1
    mac21->run_tx_tti();
    mac21->run_rx_tti();

    timers.tick_all();
  }

  // Print and analyse metrics
  // TODO
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
