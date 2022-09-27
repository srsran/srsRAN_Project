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

class pdcp_traffic_sink : public rlc_rx_upper_layer_data_notifier
{
  srslog::basic_logger& logger;

public:
  pdcp_traffic_sink() : logger(srslog::fetch_basic_logger("PDCP_RX", false)) {}

  // rlc_rx_upper_layer_data_notifier interface
  void on_new_sdu(byte_buffer_slice_chain pdu) final;
};

class pdcp_traffic_source : public rlc_tx_upper_layer_data_notifier, public rlc_tx_upper_layer_control_notifier
{
  stress_test_args& args;
  uint32_t          pdcp_count = 0;
  uint32_t          sdu_size   = 0;
  uint8_t           payload    = 0x0; // increment for each SDU

  std::mt19937                    rgen;
  std::uniform_int_distribution<> int_dist;

  rlc_tx_upper_layer_data_interface* rlc_tx_upper = nullptr;

public:
  pdcp_traffic_source(stress_test_args& args) :
    args(args), rgen(args.seed), int_dist(args.min_sdu_size, args.max_sdu_size)
  {
  }

  void set_rlc_tx_upper(rlc_tx_upper_layer_data_interface* rlc_tx_upper) { this->rlc_tx_upper = rlc_tx_upper; }

  void send_pdu();

  // rlc_tx_upper_layer_data_notifier interface
  void on_delivered_sdu(uint32_t pdcp_count) final {}

  // rlc_tx_upper_layer_control_notifier interface
  void on_protocol_failure() final {}
  void on_max_retx() final {}
};
} // namespace srsgnb
