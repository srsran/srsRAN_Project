/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
    args(args_), logger("MAC", {0, ue_id, rb_id, "DL"}), rgen(args_.seed), bsr(0)
  {
  }

  std::vector<byte_buffer_chain> pdu_rx_list;
  std::vector<byte_buffer_chain> run_tx_tti(uint32_t tti);
  void                           run_rx_tti();
  void                           push_rx_pdus(std::vector<byte_buffer_chain> list_pdus);

  // rlc_tx_lower_layer_notifier interface
  void on_buffer_state_update(unsigned bsr_) final { this->bsr.store(bsr_, std::memory_order_relaxed); }

  void set_rlc_tx_lower(rlc_tx_lower_layer_interface* rlc_tx_lower_) { this->rlc_tx_lower = rlc_tx_lower_; }
  void set_rlc_rx_lower(rlc_rx_lower_layer_interface* rlc_rx_lower_) { this->rlc_rx_lower = rlc_rx_lower_; }
};

} // namespace srsran
