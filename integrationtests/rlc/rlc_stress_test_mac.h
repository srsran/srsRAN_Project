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
#include "srsgnb/rlc/rlc_rx.h"
#include "srsgnb/rlc/rlc_tx.h"
#include "srsgnb/srslog/srslog.h"
#include <random>

namespace srsgnb {
class mac_dummy : public rlc_tx_lower_layer_notifier
{
  stress_test_args&     args;
  srslog::basic_logger& logger;

  std::mt19937                          rgen;
  std::uniform_real_distribution<float> real_dist;

  std::atomic<unsigned>                bsr;
  std::vector<byte_buffer_slice_chain> pdu_list;

  rlc_tx_lower_layer_interface* rlc_tx_lower = nullptr;
  rlc_rx_lower_layer_interface* rlc_rx_lower = nullptr;

public:
  mac_dummy(stress_test_args& args) :
    args(args), logger(srslog::fetch_basic_logger("MAC", false)), rgen(args.seed), bsr(0){};

  void run_tx_tti();
  void run_rx_tti();

  // rlc_tx_lower_layer_notifier interface
  void on_buffer_state_update(unsigned bsr) final { this->bsr.store(bsr, std::memory_order_relaxed); }

  void set_rlc_tx_lower(rlc_tx_lower_layer_interface* rlc_tx_lower) { this->rlc_tx_lower = rlc_tx_lower; }
  void set_rlc_rx_lower(rlc_rx_lower_layer_interface* rlc_rx_lower) { this->rlc_rx_lower = rlc_rx_lower; }
};

} // namespace srsgnb
