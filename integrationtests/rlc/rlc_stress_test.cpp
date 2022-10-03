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
#include "srsgnb/ran/bearer_logger.h"
#include "srsgnb/rlc/rlc_factory.h"
#include <random>
#include <unistd.h>

using namespace srsgnb;

class stress_stack
{
public:
  stress_stack(stress_test_args args, uint32_t id, timer_manager& timers_) :
    stack_id(id), timers(timers_), logger("STACK", id, lcid_t{})
  {
    // MAC
    mac = std::make_unique<mac_dummy>(args, id);

    // PDCP
    traffic_sink   = std::make_unique<pdcp_traffic_sink>(id);
    traffic_source = std::make_unique<pdcp_traffic_source>(args, id);

    // RLC
    rlc_config                  cnfg = get_rlc_config_from_args(args);
    rlc_entity_creation_message msg  = {};
    msg.ue_index                     = static_cast<du_ue_index_t>(stack_id);
    msg.lcid                         = lcid_t{};
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
  // Stack ID for logging
  uint16_t stack_id;

  // MAC
  std::unique_ptr<mac_dummy> mac = nullptr;

  // RLC
  std::unique_ptr<rlc_entity> rlc;

  // PDCP
  std::unique_ptr<pdcp_traffic_sink>   traffic_sink   = nullptr;
  std::unique_ptr<pdcp_traffic_source> traffic_source = nullptr;

  // Timers
  timer_manager& timers;

  srsgnb::bearer_logger logger;
};

void stress_test(stress_test_args args)
{
  // Have one common timer for both UE and gNB
  // emulators for now.
  timer_manager timers;

  // Create the UE/gNB emulators
  stress_stack ue_emulator(args, 0, timers);
  stress_stack gnb_emulator(args, 1, timers);

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

  init_log_from_args(args);

  stress_test(args);

  return 0;
}
