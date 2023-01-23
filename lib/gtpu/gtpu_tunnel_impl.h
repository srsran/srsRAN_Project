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

#include "gtpu_tunnel_rx_impl.h"
#include "gtpu_tunnel_tx_impl.h"
#include "srsgnb/gtpu/gtpu_config.h"
#include "srsgnb/gtpu/gtpu_tunnel.h"
#include "srsgnb/srslog/logger.h"

namespace srsgnb {

class gtpu_tunnel_impl : public gtpu_tunnel
{
public:
  gtpu_tunnel_impl(uint32_t                             ue_index,
                   gtpu_config                          cfg,
                   gtpu_tunnel_rx_lower_layer_notifier& rx_lower,
                   gtpu_tunnel_tx_upper_layer_notifier& tx_upper) :
    logger(srslog::fetch_basic_logger("GTPU"))
  {
    rx = std::make_unique<gtpu_tunnel_rx>(ue_index, cfg.rx, rx_lower);
    tx = std::make_unique<gtpu_tunnel_tx>(ue_index, cfg.tx, tx_upper);
  }
  ~gtpu_tunnel_impl() override = default;

  gtpu_tunnel_rx_upper_layer_interface* get_rx_upper_layer_interface() final { return rx.get(); }
  gtpu_tunnel_tx_lower_layer_interface* get_tx_lower_layer_interface() final { return tx.get(); };

private:
  srslog::basic_logger& logger;

  std::unique_ptr<gtpu_tunnel_rx> rx = {};
  std::unique_ptr<gtpu_tunnel_tx> tx = {};
};
} // namespace srsgnb
