/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "lib/rlc/rlc_bearer_logger.h"
#include "rlc_stress_test_args.h"
#include "srsran/rlc/rlc_rx.h"
#include "srsran/rlc/rlc_tx.h"
#include <random>

namespace srsran {
class mac_dummy : public rlc_tx_lower_layer_notifier
{
  const stress_test_args& args;
  rlc_bearer_logger       logger;

  std::mt19937                          rgen;
  std::uniform_real_distribution<float> real_dist;

  std::atomic<unsigned> bsr;

  rlc_tx_lower_layer_interface* rlc_tx_lower = nullptr;
  rlc_rx_lower_layer_interface* rlc_rx_lower = nullptr;

public:
  mac_dummy(const stress_test_args& args_, uint32_t ue_id, rb_id_t rb_id) :
    args(args_), logger("MAC", {ue_id, rb_id, "DL"}), rgen(args_.seed), bsr(0)
  {
  }

  std::vector<byte_buffer_slice_chain> pdu_rx_list;
  std::vector<byte_buffer_slice_chain> run_tx_tti(uint32_t tti);
  void                                 run_rx_tti();
  void                                 push_rx_pdus(std::vector<byte_buffer_slice_chain> list_pdus);

  // rlc_tx_lower_layer_notifier interface
  void on_buffer_state_update(unsigned bsr_) final { this->bsr.store(bsr_, std::memory_order_relaxed); }

  void set_rlc_tx_lower(rlc_tx_lower_layer_interface* rlc_tx_lower_) { this->rlc_tx_lower = rlc_tx_lower_; }
  void set_rlc_rx_lower(rlc_rx_lower_layer_interface* rlc_rx_lower_) { this->rlc_rx_lower = rlc_rx_lower_; }
};

} // namespace srsran
