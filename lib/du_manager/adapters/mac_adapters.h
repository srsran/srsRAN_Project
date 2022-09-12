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

#include "srsgnb/mac/mac_sdu_handler.h"
#include "srsgnb/rlc/rlc_rx.h"
#include "srsgnb/rlc/rlc_tx.h"

namespace srsgnb {
namespace srs_du {

class mac_sdu_rx_adapter : public mac_sdu_rx_notifier
{
public:
  void connect(rlc_rx_lower_layer_interface& rlc_rx_) { rlc_handler = &rlc_rx_; }

  void on_new_sdu(byte_buffer_slice sdu) override
  {
    srsgnb_assert(rlc_handler != nullptr, "MAC Rx SDU notifier is disconnected");
    rlc_handler->handle_pdu(std::move(sdu));
  }

private:
  rlc_rx_lower_layer_interface* rlc_handler = nullptr;
};

class mac_sdu_tx_adapter : public mac_sdu_tx_builder
{
public:
  void connect(rlc_tx_lower_layer_interface& rlc_tx) { rlc_handler = &rlc_tx; }

  byte_buffer_slice_chain on_new_tx_sdu(unsigned nof_bytes) override
  {
    srsgnb_assert(rlc_handler != nullptr, "MAC Rx SDU notifier is disconnected");
    return rlc_handler->pull_pdu(nof_bytes);
  }

  unsigned on_buffer_state_update() override
  {
    srsgnb_assert(rlc_handler != nullptr, "MAC Rx SDU notifier is disconnected");
    return rlc_handler->get_buffer_state();
  }

private:
  rlc_tx_lower_layer_interface* rlc_handler = nullptr;
};

} // namespace srs_du
} // namespace srsgnb
