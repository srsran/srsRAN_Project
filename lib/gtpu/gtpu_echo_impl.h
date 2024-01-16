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

#include "gtpu_echo_rx_impl.h"
#include "gtpu_echo_tx_impl.h"
#include "srsran/gtpu/gtpu_echo.h"
#include "srsran/gtpu/gtpu_tunnel_tx.h"
#include "srsran/pcap/dlt_pcap.h"

namespace srsran {

/// Implementation of the GTP-U path management entity, i.e. echo request/response, supported extension headers notif.
class gtpu_echo_impl final : public gtpu_echo
{
public:
  gtpu_echo_impl(dlt_pcap& gtpu_pcap, gtpu_tunnel_tx_upper_layer_notifier& tx_upper) :
    logger(srslog::fetch_basic_logger("GTPU"))
  {
    tx = std::make_unique<gtpu_echo_tx>(gtpu_pcap, tx_upper);
    rx = std::make_unique<gtpu_echo_rx>(*tx.get());
  }
  ~gtpu_echo_impl() override = default;

  gtpu_tunnel_rx_upper_layer_interface* get_rx_upper_layer_interface() override { return rx.get(); }

private:
  srslog::basic_logger& logger;

  std::unique_ptr<gtpu_echo_tx> tx = {};
  std::unique_ptr<gtpu_echo_rx> rx = {};
};
} // namespace srsran
