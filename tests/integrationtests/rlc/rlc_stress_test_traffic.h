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
#include "srsran/pdcp/pdcp_rx.h"
#include "srsran/pdcp/pdcp_tx.h"
#include "srsran/srslog/srslog.h"
#include <random>

namespace srsran {

class stress_traffic_sink : public pdcp_rx_upper_data_notifier
{
  rlc_bearer_logger logger;

public:
  stress_traffic_sink(uint32_t ue_id, rb_id_t rb_id) : logger("TRAFF", {0, ue_id, rb_id, "UL"}) {}

  // pdcp_rx_upper_data_notifier interface
  void on_new_sdu(byte_buffer pdu) final;
};

class stress_traffic_source
{
  const stress_test_args& args;
  uint32_t                sdu_size = 0;
  uint8_t                 payload  = 0x0; // increment for each SDU byte

  std::mt19937                    rgen;
  std::uniform_int_distribution<> int_dist;

  pdcp_tx_upper_data_interface* pdcp_tx_upper = nullptr;

  rlc_bearer_logger logger;

public:
  explicit stress_traffic_source(const stress_test_args& args_, uint32_t ue_id, rb_id_t rb_id) :
    args(args_),
    rgen(args_.seed),
    int_dist(args_.min_sdu_size, args_.max_sdu_size),
    logger("TRAFF", {0, ue_id, rb_id, "DL"})
  {
  }

  void set_pdcp_tx_upper(pdcp_tx_upper_data_interface* pdcp_tx_upper_) { this->pdcp_tx_upper = pdcp_tx_upper_; }

  void send_pdu();
};
} // namespace srsran
