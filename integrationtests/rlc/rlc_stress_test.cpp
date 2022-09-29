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
#include <unistd.h>

using namespace srsgnb;

class stress_stack
{
public:
  stress_stack(stress_test_args args, timer_manager& timers_) : timers(timers_)
  {
    // MAC
    mac = std::make_unique<mac_dummy>(args);

    // PDCP
    traffic_sink   = std::make_unique<pdcp_traffic_sink>();
    traffic_source = std::make_unique<pdcp_traffic_source>(args);

    // RLC
    rlc_config                  cnfg = get_rlc_config_from_args(args);
    rlc_entity_creation_message msg  = {};
    msg.rx_upper_dn                  = traffic_sink.get();
    msg.tx_upper_cn                  = traffic_source.get();
    msg.tx_upper_dn                  = traffic_source.get();
    msg.tx_lower_dn                  = mac.get();
    msg.config                       = cnfg;
    msg.timers                       = &timers;
    rlc                              = create_rlc_entity(msg);
    traffic_source->set_rlc_tx_upper(rlc->get_tx_upper_layer_data_interface());
    mac->set_rlc_tx_lower(rlc->get_tx_lower_layer_interface());
    mac->set_rlc_rx_lower(rlc->get_rx_lower_layer_interface());
  }

  void start()
  { // TODO
  }
  void stop()
  { // TODO
  }

public:
  // TODO MAC, RLC and PDCP should be private
  // MAC
  std::unique_ptr<mac_dummy> mac = nullptr;

  // RLC
  std::unique_ptr<rlc_entity> rlc;

  // PDCP
  std::unique_ptr<pdcp_traffic_sink>   traffic_sink   = nullptr;
  std::unique_ptr<pdcp_traffic_source> traffic_source = nullptr;

  // Timers
  timer_manager& timers;
};

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

  auto& log_stack = srslog::fetch_basic_logger("STACK", false);
  log_stack.set_level(args.log_level);
  log_stack.set_hex_dump_max_size(args.log_hex_limit);

  auto& log_rlc = srslog::fetch_basic_logger("RLC", false);
  log_rlc.set_level(args.log_level);
  log_rlc.set_hex_dump_max_size(args.log_hex_limit);

  auto& log_mac = srslog::fetch_basic_logger("MAC", false);
  log_mac.set_level(args.log_level);
  log_mac.set_hex_dump_max_size(args.log_hex_limit);

  // Have one common timer for both UE and gNB
  // emulators for now.
  timer_manager timers;

  // Create the UE/gNB emulators
  stress_stack ue_emulator(args, timers);
  stress_stack gnb_emulator(args, timers);

  // Launch transmission
  // Fixme: create worker thread and executer
  ue_emulator.start();
  gnb_emulator.start();

  for (uint32_t n = 0; n < 100; n++) {
    ue_emulator.traffic_source->send_pdu();
    gnb_emulator.traffic_source->send_pdu();
  }

  while (ue_emulator.rlc->get_tx_lower_layer_interface()->get_buffer_state() > 0) {
    // RLC1->MAC12->RLC2
    std::vector<byte_buffer_slice_chain> pdu_list1 = ue_emulator.mac->run_tx_tti();
    gnb_emulator.mac->run_rx_tti(std::move(pdu_list1));

    // RLC2->MAC21->RLC1
    std::vector<byte_buffer_slice_chain> pdu_list2 = gnb_emulator.mac->run_tx_tti();
    ue_emulator.mac->run_rx_tti(std::move(pdu_list2));

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
