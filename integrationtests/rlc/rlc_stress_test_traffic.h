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
#include "srsgnb/pdcp/pdcp_rx.h"
#include "srsgnb/pdcp/pdcp_tx.h"
#include "srsgnb/ran/bearer_logger.h"
#include "srsgnb/srslog/srslog.h"
#include <random>

namespace srsgnb {

class stress_traffic_sink : public pdcp_rx_upper_data_notifier
{
  bearer_logger logger;

public:
  stress_traffic_sink(uint32_t id) : logger("TRAF", id, lcid_t{}) {}

  // pdcp_rx_upper_data_notifier interface
  void on_new_sdu(byte_buffer pdu) final;
};

class stress_traffic_source : public pdcp_tx_upper_control_notifier
{
  const stress_test_args& args;
  uint32_t                sdu_size = 0;
  uint8_t                 payload  = 0x0; // increment for each SDU byte

  std::mt19937                    rgen;
  std::uniform_int_distribution<> int_dist;

  pdcp_tx_upper_data_interface* pdcp_tx_upper = nullptr;

  srsgnb::bearer_logger logger;

public:
  explicit stress_traffic_source(const stress_test_args& args, uint32_t id) :
    args(args), rgen(args.seed), int_dist(args.min_sdu_size, args.max_sdu_size), logger("PDCP", id, lcid_t{})
  {
  }

  void set_pdcp_tx_upper(pdcp_tx_upper_data_interface* pdcp_tx_upper) { this->pdcp_tx_upper = pdcp_tx_upper; }

  void send_pdu();

  // pdcp_tx_upper_layer_control_notifier interface
  void on_max_hfn_reached() final{};
  void on_protocol_failure() final {}
};
} // namespace srsgnb
