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

#include "gtpu_tunnel_nru_rx_impl.h"
#include "gtpu_tunnel_nru_tx_impl.h"
#include "srsran/gtpu/gtpu_config.h"
#include "srsran/gtpu/gtpu_tunnel_nru.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/srslog/logger.h"

namespace srsran {

class gtpu_tunnel_nru_impl : public gtpu_tunnel_nru
{
public:
  gtpu_tunnel_nru_impl(srs_cu_up::ue_index_t                       ue_index,
                       gtpu_config                                 cfg,
                       dlt_pcap&                                   gtpu_pcap,
                       gtpu_tunnel_nru_rx_lower_layer_notifier&    rx_lower,
                       gtpu_tunnel_common_tx_upper_layer_notifier& tx_upper,
                       timer_factory                               ue_dl_timer_factory) :
    logger(srslog::fetch_basic_logger("GTPU"))
  {
    rx = std::make_unique<gtpu_tunnel_nru_rx_impl>(ue_index, cfg.rx, rx_lower, ue_dl_timer_factory);
    tx = std::make_unique<gtpu_tunnel_nru_tx_impl>(ue_index, cfg.tx, gtpu_pcap, tx_upper);
  }
  ~gtpu_tunnel_nru_impl() override = default;

  gtpu_tunnel_common_rx_upper_layer_interface* get_rx_upper_layer_interface() final { return rx.get(); }
  gtpu_tunnel_nru_tx_lower_layer_interface*    get_tx_lower_layer_interface() final { return tx.get(); };

private:
  srslog::basic_logger& logger;

  std::unique_ptr<gtpu_tunnel_nru_rx_impl> rx = {};
  std::unique_ptr<gtpu_tunnel_nru_tx_impl> tx = {};
};
} // namespace srsran
