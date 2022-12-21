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

#include "srsgnb/f1u/cu_up/f1u_gateway.h"
#include "srsgnb/gtpu/gtpu_demux.h"
#include "srsgnb/gtpu/gtpu_tunnel_tx.h"

namespace srsgnb {

/// Dummy GTP-U Rx Demux
class dummy_gtpu_demux_ctrl : public gtpu_demux_ctrl
{
public:
  dummy_gtpu_demux_ctrl()  = default;
  ~dummy_gtpu_demux_ctrl() = default;

  bool add_tunnel(uint32_t teid, gtpu_tunnel_rx_upper_layer_interface* tunnel) override { return true; }

  bool remove_tunnel(uint32_t teid) override { return true; }
};

/// Dummy adapter between GTP-U and Network Gateway
class dummy_gtpu_network_gateway_adapter : public gtpu_tunnel_tx_upper_layer_notifier
{
public:
  dummy_gtpu_network_gateway_adapter()  = default;
  ~dummy_gtpu_network_gateway_adapter() = default;

  void on_new_pdu(byte_buffer pdu) override {}
};

class dummy_f1u_gateway final : public f1u_cu_up_gateway
{
public:
  dummy_f1u_gateway()  = default;
  ~dummy_f1u_gateway() = default;

  srs_cu_up::f1u_bearer* create_cu_dl_bearer(uint32_t                             dl_teid,
                                             srs_cu_up::f1u_rx_delivery_notifier& cu_delivery,
                                             srs_cu_up::f1u_rx_sdu_notifier&      cu_rx) override
  {
    return nullptr;
  };
  void attach_cu_ul_bearer(uint32_t dl_teid, uint32_t ul_teid) override{};
};

} // namespace srsgnb
