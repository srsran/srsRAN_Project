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

#include "lib/rlc/rlc_bearer_logger.h"
#include "rlc_stress_test_args.h"
#include "srsgnb/pdcp/pdcp_rx.h"
#include "srsgnb/pdcp/pdcp_tx.h"
#include "srsgnb/srslog/srslog.h"
#include <random>

namespace srsgnb {

class stress_traffic_sink : public pdcp_rx_upper_data_notifier
{
  rlc_bearer_logger logger;

public:
  stress_traffic_sink(uint32_t ue_id, rb_id_t rb_id) : logger("TRAFF", {ue_id, rb_id, "UL"}) {}

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
    logger("TRAFF", {ue_id, rb_id, "DL"})
  {
  }

  void set_pdcp_tx_upper(pdcp_tx_upper_data_interface* pdcp_tx_upper_) { this->pdcp_tx_upper = pdcp_tx_upper_; }

  void send_pdu();
};
} // namespace srsgnb
