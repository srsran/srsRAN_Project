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

#include "srsgnb/gateways/network_gateway.h"
#include "srsgnb/gtpu/gtpu_tunnel_rx.h"
#include "srsgnb/gtpu/gtpu_tunnel_tx.h"
#include "srsgnb/sdap/sdap.h"

namespace srsgnb {
namespace srs_cu_up {

/// Adapter between GTP-U and Network Gateway
class gtpu_network_gateway_adapter : public gtpu_tunnel_tx_upper_layer_notifier
{
public:
  gtpu_network_gateway_adapter()  = default;
  ~gtpu_network_gateway_adapter() = default;

  void connect_network_gateway(network_gateway_data_handler& gw_handler_) { gw_handler = &gw_handler_; }

  void on_new_pdu(byte_buffer pdu) override
  {
    srsgnb_assert(gw_handler != nullptr, "Network Gateway handler must not be nullptr");
    gw_handler->handle_pdu(std::move(pdu));
  }

private:
  network_gateway_data_handler* gw_handler = nullptr;
};

/// Adapter between GTP-U and SDAP
class gtpu_sdap_adapter : public gtpu_tunnel_rx_lower_layer_notifier
{
public:
  gtpu_sdap_adapter()  = default;
  ~gtpu_sdap_adapter() = default;

  void connect_sdap(sdap_tx_sdu_handler& sdap_handler_) { sdap_handler = &sdap_handler_; }

  void on_new_sdu(byte_buffer sdu) override
  {
    srsgnb_assert(sdap_handler != nullptr, "SDAP handler must not be nullptr");
    sdap_handler->handle_sdu(std::move(sdu));
  }

private:
  sdap_tx_sdu_handler* sdap_handler = nullptr;
};

} // namespace srs_cu_up
} // namespace srsgnb
