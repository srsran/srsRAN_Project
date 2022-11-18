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

#include "gtpu_entity_rx.h"
#include "gtpu_entity_tx.h"
#include "srsgnb/gtpu/gtpu_config.h"
#include "srsgnb/gtpu/gtpu_entity.h"
#include "srsgnb/srslog/logger.h"

namespace srsgnb {

class gtpu_entity_impl : public gtpu_entity
{
public:
  gtpu_entity_impl(uint32_t                      ue_index,
                   gtpu_config                   cfg,
                   gtpu_rx_lower_layer_notifier& rx_lower,
                   gtpu_tx_upper_layer_notifier& tx_upper) :
    logger(srslog::fetch_basic_logger("GTPU"))
  {
    rx = std::make_unique<gtpu_entity_rx>(ue_index, cfg.rx, rx_lower);
    logger.info("GTP-U RX configured: {}", cfg.rx);
    tx = std::make_unique<gtpu_entity_tx>(ue_index, cfg.tx, tx_upper);
    logger.info("GTP-U TX configured: {}", cfg.tx);
  }
  ~gtpu_entity_impl() override = default;

  gtpu_rx_upper_layer_interface* get_rx_upper_layer_interface() final { return rx.get(); }
  gtpu_tx_lower_layer_interface* get_tx_lower_layer_interface() final { return tx.get(); };

private:
  srslog::basic_logger& logger;

  std::unique_ptr<gtpu_entity_rx> rx = {};
  std::unique_ptr<gtpu_entity_tx> tx = {};
};
} // namespace srsgnb
