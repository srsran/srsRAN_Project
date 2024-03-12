/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
                   timer_factory                   ue_ctrl_timer_factory) :
    logger("PDCP", {ue_index, rb_id, "DL/UL"}),
    metrics_period(config.custom.metrics_period),
    metrics_timer(ue_ctrl_timer_factory.create_timer())
  {
    tx = std::make_unique<pdcp_entity_tx>(
        ue_index, rb_id, config.get_tx_config(), tx_lower_dn, tx_upper_cn, ue_dl_timer_factory);
    rx = std::make_unique<pdcp_entity_rx>(
        ue_index, rb_id, config.get_rx_config(), rx_upper_dn, rx_upper_cn, ue_ul_timer_factory);

    // Tx/Rx interconnect
    tx->set_status_provider(rx.get());
    rx->set_status_handler(tx.get());

    if (config.custom.metrics_period.count() != 0) {
      metrics_timer.set(config.custom.metrics_period, [this](timer_id_t /*tid*/) { push_metrics(); });
      metrics_timer.run();
    }
  }
  ~pdcp_entity_impl() override = default;
  pdcp_tx_upper_control_interface& get_tx_upper_control_interface() final { return (*tx); };
  pdcp_tx_upper_data_interface&    get_tx_upper_data_interface() final { return (*tx); };
  pdcp_tx_lower_interface&         get_tx_lower_interface() final { return (*tx); };
  pdcp_rx_upper_control_interface& get_rx_upper_control_interface() final { return (*rx); };
  pdcp_rx_lower_interface&         get_rx_lower_interface() final { return (*rx); };

  pdcp_metrics_container get_metrics() final
  {
    pdcp_metrics_container m;
    m.tx = tx->get_metrics();
    m.rx = rx->get_metrics();
    return m;
  };

  void reset_metrics()
  {
    tx->reset_metrics();
    rx->reset_metrics();
  }

  void push_metrics()
  {
    pdcp_metrics_container m = get_metrics();
    log_metrics(m);
    reset_metrics();
    metrics_timer.run();
  }

  void log_metrics(const pdcp_metrics_container& m)
  {
    logger.log_info(
        "TX metrics period={}ms num_sdus={} sdu_rate={}kbps, num_pdus={}, pdus_rate={} discard_timeouts={} ",
        metrics_period.count(),
        m.tx.num_sdus,
        (double)m.tx.num_sdu_bytes * 8 / (double)metrics_period.count(),
        m.tx.num_pdus,
        (double)m.tx.num_pdu_bytes * 8 / (double)metrics_period.count(),
        m.tx.num_discard_timeouts);
    logger.log_info("RX metrics period={}ms num_pdus={} pdu_rate={} num_dropped_pdus={} num_sdus={} sdu_rate={} "
                    "num_integrity_verified_pdus={} num_integrity_failed_pdus={} num_t_reordering_timeouts={}",
                    metrics_period.count(),
                    m.rx.num_pdus,
                    (double)m.rx.num_pdu_bytes * 8 / (double)metrics_period.count(),
                    m.rx.num_dropped_pdus,
                    m.rx.num_sdus,
                    (double)m.rx.num_sdu_bytes * 8 / (double)metrics_period.count(),
                    m.rx.num_integrity_verified_pdus,
                    m.rx.num_integrity_failed_pdus,
                    m.rx.num_t_reordering_timeouts);
  }

private:
  std::unique_ptr<pdcp_entity_tx> tx = {};
  std::unique_ptr<pdcp_entity_rx> rx = {};

  pdcp_bearer_logger logger;
  timer_duration     metrics_period;
  unique_timer       metrics_timer;
};
} // namespace srsran
