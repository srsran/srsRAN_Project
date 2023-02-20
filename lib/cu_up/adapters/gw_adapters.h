/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/gateways/network_gateway.h"
#include "srsran/gtpu/gtpu_demux.h"

namespace srsran {
namespace srs_cu_up {

/// Adapter between Network Gateway (Data) and GTP-U demux
class network_gateway_data_gtpu_demux_adapter : public srsran::network_gateway_data_notifier
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

} // namespace srs_cu_up
} // namespace srsran
