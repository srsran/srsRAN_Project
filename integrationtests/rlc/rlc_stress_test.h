/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "rlc_stress_test_args.h"
#include "rlc_stress_test_f1.h"
#include "rlc_stress_test_mac.h"
#include "rlc_stress_test_rrc.h"
#include "rlc_stress_test_traffic.h"
#include "srsgnb/pdcp/pdcp_factory.h"
#include "srsgnb/rlc/rlc_factory.h"
#include "srsgnb/support/executors/task_worker.h"
#include <condition_variable>
#include <mutex>
#include <random>
#include <unistd.h>

namespace srsgnb {

/// \brief Stack emulator used to stress test the RLC TX/RX entities.
/// This emulator will try to mimic a real gNB stack as closely as possible.
/// To do this, it will contain two executors (threads) the PCell executor and a UE executor.
/// The PCell executor will emulate the MAC TX by pulling PDUs from RLC;
/// while the UE executor will emulate the MAC RX and PDCP RX and TX.
class stress_stack
{
public:
  stress_stack(const stress_test_args& args, uint32_t id);

  ~stress_stack() = default;

  void start();
  void wait_for_finish();

  void run_upper_tti(uint32_t tti);
  void run_lower_tti(uint32_t tti);
  void push_pdus(std::vector<byte_buffer_slice_chain> list_pdus);

  void set_peer_stack(stress_stack* peer_stack_) { peer_stack = peer_stack_; }

  pdcp_metrics_container get_pdcp_metrics() { return pdcp->get_metrics(); }
  rlc_metrics            get_rlc_metrics() { return rlc->get_metrics(); }

  // Mutex and condition variables for stopping workers
  std::mutex              mutex_pcell;
  std::mutex              mutex_ue;
  std::condition_variable cv_pcell;
  std::condition_variable cv_ue;
  std::atomic<bool>       stopping_pcell = {false};
  std::atomic<bool>       stopping_ue    = {false};

private:
  // Stack ID for logging
  uint16_t stack_id;

  // Arguments
  const stress_test_args& args;

  // Executors
  uint16_t                       task_worker_queue_size = 15000;
  std::string                    ue_name;
  std::string                    pcell_name;
  task_worker                    ue_worker;
  task_worker                    pcell_worker;
  std::unique_ptr<task_executor> ue_executor;
  std::unique_ptr<task_executor> pcell_executor;

  // Security
  security::sec_128_as_config sec_cfg;

  // Peer stack
  stress_stack* peer_stack = nullptr;

  // Timers
  timer_manager timers;

  // MAC
  std::unique_ptr<mac_dummy> mac = nullptr;

  // RLC
  std::unique_ptr<rlc_entity> rlc = nullptr;

  // F1
  std::unique_ptr<f1_dummy> f1 = nullptr;

  // PDCP
  std::unique_ptr<pdcp_entity> pdcp = nullptr;

  // RRC
  std::unique_ptr<rrc_dummy> rrc = nullptr;

  // Traffic generators
  std::unique_ptr<stress_traffic_sink>   traffic_sink   = nullptr;
  std::unique_ptr<stress_traffic_source> traffic_source = nullptr;

  // Logging
  srsgnb::bearer_logger logger;
};
} // namespace srsgnb
