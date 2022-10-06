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
#include "srsgnb/support/executors/task_worker.h"
#include "srsgnb/support/srsgnb_assert.h"
#include <condition_variable>
#include <mutex>
#include <random>
#include <unistd.h>

using namespace srsgnb;

/// \brief Stack emulator used to stress test the RLC TX/RX entities.
/// This will contain two executors (threads) to accuratly emulate a real stack.
/// One for the MAC TX that will pull PDUs from RLC, another for MAC RX and
/// PDCP RX and TX.
class stress_stack
{
public:
  stress_stack(const stress_test_args& args, uint32_t id) :
    stack_id(id),
    upper_name("Upper-Worker" + std::to_string(id)),
    lower_name("Lower-Worker" + std::to_string(id)),
    upper_worker{upper_name, task_worker_queue_size, true},
    lower_worker{lower_name, task_worker_queue_size, true},
    logger("STACK", id, lcid_t{})
  {
    upper_executor = make_task_executor(upper_worker);
    lower_executor = make_task_executor(lower_worker);

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

    srslog::flush();
  }

  ~stress_stack() {}

  void start()
  {
    srsgnb_assert(peer_stack != nullptr, "Peer stack was not set when starting the stack.");
    srsgnb_assert(peer_stack != this, "Peer cannot be itself.");
    for (uint32_t i = 0; i < 20; i++) {
      traffic_source->send_pdu();
    }
    lower_worker.start();
    upper_worker.start();

    // Schedule the TTI when the thread are started.
    lower_executor->defer([this]() { run_lower_tti(); });
  }

  void stop()
  {
    fprintf(stderr, "Waiting to lower worker. Stack id=%d\n", stack_id);
    std::unique_lock<std::mutex> lk_lower(mutex_lower);
    cv_lower.wait(lk_lower, [this] { return stopping_lower; });
    fprintf(stderr,
            "Lower thread no longer processing. Stack id=%d, stopping=%s\n",
            stack_id,
            stopping_lower ? "true" : "false");

    fprintf(stderr, "Waiting to stop upper worker. Stack id=%d\n", stack_id);
    std::unique_lock<std::mutex> lk_upper(mutex_upper);
    cv_upper.wait(lk_upper, [this] { return stopping_upper; });
    fprintf(stderr, "Upper thread no longer processing. Stack id=%d\n", stack_id);

    lower_worker.stop();
    upper_worker.stop();
  }

  void run_upper_tti(uint32_t tti)
  {
    logger.log_warning("Running upper TTI={}, PDU RX queue size={}", tti, mac->pdu_rx_list.size());
    lower_executor->defer([this]() { run_lower_tti(); });
    if (tti < 500) {
      for (uint32_t i = 0; i < 20; i++) {
        traffic_source->send_pdu();
      }
      mac->run_rx_tti();
      timers.tick_all(); // timers are run from the upper executor
    } else {
      // Manual unlocking is done before notifying, to avoid waking up
      // the waiting thread only to block again (see notify_one for details)
      std::unique_lock<std::mutex> lk(mutex_upper);
      stopping_upper = true;
      lk.unlock();
      cv_lower.notify_all();
    }
  }

  void run_lower_tti()
  {
    static uint32_t tti = 0;
    if (tti < 500) {
      logger.log_warning("Running lower TTI={}", tti);
      std::vector<byte_buffer_slice_chain> pdu_list = mac->run_tx_tti(tti);
      logger.log_info("Generated PDU list size={}", pdu_list.size());
      upper_executor->defer([this]() { run_upper_tti(tti); });
      peer_stack->push_pdus(std::move(pdu_list));
      tti++;
    } else {
      logger.log_warning("Stopping lower TTI={}", tti);
      // Manual unlocking is done before notifying, to avoid waking up
      // the waiting thread only to block again (see notify_one for details)
      std::unique_lock<std::mutex> lk(mutex_lower);
      stopping_lower = true;
      lk.unlock();
      cv_lower.notify_all();
    }
  }

  void push_pdus(std::vector<byte_buffer_slice_chain> list_pdus)
  {
    auto push_fnc = [this, list_pdus = std::move(list_pdus)]() mutable { mac->push_rx_pdus(std::move(list_pdus)); };
    upper_executor->defer(std::move(push_fnc));
  }

  void set_peer_stack(stress_stack* peer_stack_) { peer_stack = peer_stack_; }

  // Mutex and condition variables for stopping workers
  std::mutex              mutex_lower;
  std::mutex              mutex_upper;
  std::condition_variable cv_lower;
  std::condition_variable cv_upper;
  bool                    stopping_lower = false;
  bool                    stopping_upper = false;

private:
  // Stack ID for logging
  uint16_t stack_id;

  // Executors
  uint16_t                       task_worker_queue_size = 9000;
  std::string                    upper_name;
  std::string                    lower_name;
  task_worker                    upper_worker;
  task_worker                    lower_worker;
  std::unique_ptr<task_executor> upper_executor;
  std::unique_ptr<task_executor> lower_executor;

  stress_stack* peer_stack = nullptr;

  // Timers
  timer_manager timers;

  // MAC
  std::unique_ptr<mac_dummy> mac = nullptr;

  // RLC
  std::unique_ptr<rlc_entity> rlc;

  // PDCP
  std::unique_ptr<pdcp_traffic_sink>   traffic_sink   = nullptr;
  std::unique_ptr<pdcp_traffic_source> traffic_source = nullptr;

  // Logging
  srsgnb::bearer_logger logger;
};

void stress_test(const stress_test_args& args)
{
  // Create the UE/gNB emulators
  stress_stack ue_emulator(args, 0);
  stress_stack gnb_emulator(args, 1);
  ue_emulator.set_peer_stack(&gnb_emulator);
  gnb_emulator.set_peer_stack(&ue_emulator);

  fprintf(stderr, "Starting UE emulator threads\n");
  fprintf(stderr, "Starting gNB emulator threads\n");

  //  Launch transmission
  ue_emulator.start();
  gnb_emulator.start();

  //  Wait for threads to finish
  ue_emulator.stop();
  gnb_emulator.stop();

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
