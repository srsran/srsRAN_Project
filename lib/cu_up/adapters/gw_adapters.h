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
#include "srsgnb/gtpu/gtpu_demux.h"

namespace srsgnb {
namespace srs_cu_up {

/// Adapter between Network Gateway (Data) and GTP-U demux
class network_gateway_data_gtpu_demux_adapter : public srsgnb::network_gateway_data_notifier
{
public:
  network_gateway_data_gtpu_demux_adapter()  = default;
  ~network_gateway_data_gtpu_demux_adapter() = default;

  void connect_gtpu_demux(gtpu_demux_rx_upper_layer_interface& gtpu_demux_) { gtpu_demux = &gtpu_demux_; }

  void on_new_pdu(byte_buffer pdu) override
  {
    srsgnb_assert(gtpu_demux != nullptr, "GTP-U handler must not be nullptr");
    gtpu_demux->handle_pdu(std::move(pdu));
  }

private:
  gtpu_demux_rx_upper_layer_interface* gtpu_demux = nullptr;
};

/// Adapter between Network Gateway (Control) and GTP-U demux
///
/// This interface is a dummy, since the GTP-U does not need any control info about connection establishment or loss
class network_gateway_control_gtpu_demux_adapter : public srsgnb::network_gateway_control_notifier
{
public:
  network_gateway_control_gtpu_demux_adapter()  = default;
  ~network_gateway_control_gtpu_demux_adapter() = default;

  void on_connection_established() override
  {
    // Nothing to do here
  }

  void on_connection_loss() override
  {
    // Nothing to do here
  }
};

} // namespace srs_cu_up
} // namespace srsgnb
