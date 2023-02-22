/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/mac/mac_sdu_handler.h"
#include "srsran/rlc/rlc_rx.h"
#include "srsran/rlc/rlc_tx.h"

namespace srsran {
namespace srs_du {

class mac_sdu_rx_adapter : public mac_sdu_rx_notifier
{
public:
  void connect(rlc_rx_lower_layer_interface& rlc_rx_) { rlc_handler = &rlc_rx_; }

  void on_new_sdu(byte_buffer_slice sdu) override
  {
    srsran_assert(rlc_handler != nullptr, "MAC Rx SDU notifier is disconnected");
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
    srsran_assert(rlc_handler != nullptr, "MAC Rx SDU notifier is disconnected");
    return rlc_handler->pull_pdu(nof_bytes);
  }

  unsigned on_buffer_state_update() override
  {
    srsran_assert(rlc_handler != nullptr, "MAC Rx SDU notifier is disconnected");
    return rlc_handler->get_buffer_state();
  }

private:
  rlc_tx_lower_layer_interface* rlc_handler = nullptr;
};

} // namespace srs_du
} // namespace srsran
