/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/gateways/udp_network_gateway.h"
#include "srsran/gtpu/gtpu_tunnel_common_rx.h"
#include "srsran/gtpu/gtpu_tunnel_common_tx.h"
#include "srsran/gtpu/gtpu_tunnel_ngu_rx.h"
#include "srsran/sdap/sdap.h"

namespace srsran::srs_cu_up {

/// Adapter between GTP-U and Network Gateway
class gtpu_network_gateway_adapter : public gtpu_tunnel_common_tx_upper_layer_notifier
{
public:
  gtpu_network_gateway_adapter()           = default;
  ~gtpu_network_gateway_adapter() override = default;

  void connect_network_gateway(udp_network_gateway_data_handler& gw_handler_) { gw_handler = &gw_handler_; }

  void disconnect() { gw_handler = nullptr; }

  void on_new_pdu(byte_buffer pdu, const sockaddr_storage& addr) override
  {
    if (gw_handler != nullptr) {
      gw_handler->handle_pdu(std::move(pdu), addr);
    } else {
      srslog::fetch_basic_logger("GTPU", false).debug("Dropped UL GTP-U PDU. Adapter is disconnected.");
    }
  }

private:
  udp_network_gateway_data_handler* gw_handler = nullptr;
};

/// Adapter between GTP-U and SDAP
class gtpu_sdap_adapter : public gtpu_tunnel_ngu_rx_lower_layer_notifier
{
public:
  gtpu_sdap_adapter()           = default;
  ~gtpu_sdap_adapter() override = default;

  void connect_sdap(sdap_tx_sdu_handler& sdap_handler_) { sdap_handler = &sdap_handler_; }

  void on_new_sdu(byte_buffer sdu, qos_flow_id_t qos_flow_id) override
  {
    srsran_assert(sdap_handler != nullptr, "SDAP handler must not be nullptr");
    sdap_handler->handle_sdu(std::move(sdu), qos_flow_id);
  }

private:
  sdap_tx_sdu_handler* sdap_handler = nullptr;
};

} // namespace srsran::srs_cu_up
