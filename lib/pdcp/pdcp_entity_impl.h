/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "pdcp_entity_rx.h"
#include "pdcp_entity_tx.h"
#include "pdcp_metrics_aggregator.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/pdcp/pdcp_entity.h"
#include <cstdio>
#include <memory>

namespace srsran {

class pdcp_entity_impl : public pdcp_entity
{
public:
  pdcp_entity_impl(uint32_t                        ue_index,
                   rb_id_t                         rb_id,
                   pdcp_config                     config,
                   pdcp_tx_lower_notifier&         tx_lower_dn,
                   pdcp_tx_upper_control_notifier& tx_upper_cn,
                   pdcp_rx_upper_data_notifier&    rx_upper_dn,
                   pdcp_rx_upper_control_notifier& rx_upper_cn,
                   timer_factory                   ue_dl_timer_factory,
                   timer_factory                   ue_ul_timer_factory,
                   timer_factory                   ue_ctrl_timer_factory,
                   task_executor&                  ue_dl_executor,
                   task_executor&                  ue_ul_executor,
                   task_executor&                  ue_ctrl_executor,
                   task_executor&                  crypto_executor,
                   uint32_t                        max_nof_crypto_workers) :
    logger("PDCP", {ue_index, rb_id, "DL/UL"}),
    metrics_period(config.custom.metrics_period),
    metrics_timer(ue_ctrl_timer_factory.create_timer()),
    metrics_agg(ue_index, rb_id, metrics_period, config.custom.metrics_notifier, ue_ctrl_executor)
  {
    tx = std::make_unique<pdcp_entity_tx>(ue_index,
                                          rb_id,
                                          config.get_tx_config(),
                                          tx_lower_dn,
                                          tx_upper_cn,
                                          ue_ctrl_timer_factory,
                                          ue_dl_executor,
                                          crypto_executor,
                                          max_nof_crypto_workers,
                                          metrics_agg);
    rx = std::make_unique<pdcp_entity_rx>(ue_index,
                                          rb_id,
                                          config.get_rx_config(),
                                          rx_upper_dn,
                                          rx_upper_cn,
                                          ue_ctrl_timer_factory,
                                          ue_ul_executor,
                                          crypto_executor,
                                          max_nof_crypto_workers,
                                          metrics_agg);

    // Tx/Rx interconnect
    tx->set_status_provider(rx.get());
    rx->set_status_handler(tx.get());
  }
  ~pdcp_entity_impl() override { stop(); }
  pdcp_tx_upper_control_interface& get_tx_upper_control_interface() final { return (*tx); }
  pdcp_tx_upper_data_interface&    get_tx_upper_data_interface() final { return (*tx); }
  pdcp_tx_lower_interface&         get_tx_lower_interface() final { return (*tx); }
  pdcp_rx_upper_control_interface& get_rx_upper_control_interface() final { return (*rx); }
  pdcp_rx_lower_interface&         get_rx_lower_interface() final { return (*rx); }

  void stop() override
  {
    if (not stopped) {
      stopped = true;
      metrics_timer.stop();
      tx->stop();
      rx->stop();
    }
  }

  manual_event_flag& tx_crypto_awaitable() override { return tx->crypto_awaitable(); }
  manual_event_flag& rx_crypto_awaitable() override { return rx->crypto_awaitable(); }

private:
  std::unique_ptr<pdcp_entity_tx> tx;
  std::unique_ptr<pdcp_entity_rx> rx;

  pdcp_bearer_logger      logger;
  timer_duration          metrics_period;
  unique_timer            metrics_timer;
  pdcp_metrics_aggregator metrics_agg;
  bool                    stopped = false;
};
} // namespace srsran
