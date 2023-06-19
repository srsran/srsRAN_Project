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

#include "srsran/gateways/udp_network_gateway.h"
#include "srsran/gtpu/gtpu_tunnel_rx.h"
#include "srsran/gtpu/gtpu_tunnel_tx.h"
#include "srsran/sdap/sdap.h"

namespace srsran {
namespace srs_cu_up {

/// Adapter between GTP-U and Network Gateway
class gtpu_network_gateway_adapter : public gtpu_tunnel_tx_upper_layer_notifier
{
public:
  gtpu_network_gateway_adapter()  = default;
  ~gtpu_network_gateway_adapter() = default;

  void connect_network_gateway(udp_network_gateway_data_handler& gw_handler_) { gw_handler = &gw_handler_; }

  void on_new_pdu(byte_buffer pdu, const sockaddr_storage& addr) override
  {
    srsran_assert(gw_handler != nullptr, "Network Gateway handler must not be nullptr");
    gw_handler->handle_pdu(std::move(pdu), (sockaddr*)&addr, sizeof(sockaddr_storage));
  }

private:
  udp_network_gateway_data_handler* gw_handler = nullptr;
};

/// Adapter between GTP-U and SDAP
class gtpu_sdap_adapter : public gtpu_tunnel_ngu_rx_lower_layer_notifier
{
public:
  gtpu_sdap_adapter()  = default;
  ~gtpu_sdap_adapter() = default;

  void connect_sdap(sdap_tx_sdu_handler& sdap_handler_) { sdap_handler = &sdap_handler_; }

  void on_new_sdu(byte_buffer sdu, qos_flow_id_t qos_flow_id) override
  {
    srsran_assert(sdap_handler != nullptr, "SDAP handler must not be nullptr");
    sdap_handler->handle_sdu(std::move(sdu), qos_flow_id);
  }

private:
  sdap_tx_sdu_handler* sdap_handler = nullptr;
};

} // namespace srs_cu_up
} // namespace srsran
