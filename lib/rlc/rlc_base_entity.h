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

#include "rlc_bearer_logger.h"
#include "rlc_rx_entity.h"
#include "rlc_tx_entity.h"
#include "srsran/rlc/rlc_entity.h"
#include "srsran/support/timers.h"

namespace srsran {

/// Class used to store common members to all RLC entites.
/// It will contain the base class for TX and RX entities and getters
/// for them. It will also contain UE index, LCID and logging helpers
///
/// \param du_index UE identifier within the DU
/// \param lcid LCID for the bearer
class rlc_base_entity : public rlc_entity
{
public:
  rlc_base_entity(uint32_t              du_index_,
                  du_ue_index_t         ue_index_,
                  rb_id_t               rb_id_,
                  timer_duration        metrics_period_,
                  rlc_metrics_notifier* rlc_metrics_notifier_,
                  timer_factory         timers) :
    logger("RLC", {du_index_, ue_index_, rb_id_, "DL/UL"}),
    ue_index(ue_index_),
    rb_id(rb_id_),
    metrics_period(metrics_period_),
    metrics_timer(timers.create_timer())
  {
    rlc_metrics_notif = rlc_metrics_notifier_;
    if (metrics_period.count() != 0) {
      metrics_timer.set(metrics_period, [this](timer_id_t /*tid*/) { push_metrics(); });
      metrics_timer.run();
    }
  }
  ~rlc_base_entity() override                         = default;
  rlc_base_entity(const rlc_base_entity&)             = delete;
  rlc_base_entity& operator=(const rlc_base_entity&)  = delete;
  rlc_base_entity(const rlc_base_entity&&)            = delete;
  rlc_base_entity& operator=(const rlc_base_entity&&) = delete;

  void stop() final
  {
    if (tx != nullptr) {
      logger.log_debug("Stopping TX entity");
      tx->stop();
    } else {
      logger.log_error("No TX entity to stop");
    }
    if (rx != nullptr) {
      logger.log_debug("Stopping RX entity");
      rx->stop();
    } else {
      logger.log_error("No RX entity to stop");
    }
  }

  rlc_tx_upper_layer_data_interface* get_tx_upper_layer_data_interface() final { return tx.get(); };
  rlc_tx_lower_layer_interface*      get_tx_lower_layer_interface() final { return tx.get(); };
  rlc_rx_lower_layer_interface*      get_rx_lower_layer_interface() final { return rx.get(); };

  rlc_metrics get_metrics() final
  {
    rlc_metrics metrics = {};
    metrics.ue_index    = ue_index;
    metrics.rb_id       = rb_id;
    metrics.tx          = tx->get_metrics();
    metrics.rx          = rx->get_metrics();
    return metrics;
  }

  void reset_metrics()
  {
    tx->reset_metrics();
    rx->reset_metrics();
  }

protected:
  rlc_bearer_logger              logger;
  du_ue_index_t                  ue_index;
  rb_id_t                        rb_id;
  std::unique_ptr<rlc_tx_entity> tx = {};
  std::unique_ptr<rlc_rx_entity> rx = {};
  timer_duration                 metrics_period;

private:
  unique_timer          metrics_timer;
  rlc_metrics_notifier* rlc_metrics_notif;

  void push_metrics()
  {
    rlc_metrics m = get_metrics();
    if (rlc_metrics_notif) {
      rlc_metrics_notif->report_metrics(m);
    }
    if (m.tx.mode == rlc_mode::am) {
      logger.log_info(
          "TX metrics period={}ms num_sdus={} sdu_rate={}kbps, dropped_sdus={} discarded_sdus={} "
          "num_pdus_no_segm={} pdu_rate_no_segm={}kbps num_pdus_with_segm={} pdu_rate_with_segm={}kbps num_retx={} "
          "retx_rate={}kbps ctrl_pdus={} ctrl_rate={}kbps",
          metrics_period.count(),
          m.tx.num_sdus,
          (double)m.tx.num_sdu_bytes * 8 / (double)metrics_period.count(),
          m.tx.num_dropped_sdus,
          m.tx.num_discarded_sdus,
          m.tx.num_pdus_no_segmentation,
          (double)m.tx.num_pdu_bytes_no_segmentation * 8 / (double)metrics_period.count(),
          m.tx.mode_specific.am.num_pdus_with_segmentation,
          (double)m.tx.mode_specific.am.num_pdu_bytes_with_segmentation * 8 / (double)metrics_period.count(),
          m.tx.mode_specific.am.num_retx_pdus,
          (double)m.tx.mode_specific.am.num_retx_pdu_bytes * 8 / (double)metrics_period.count(),
          m.tx.mode_specific.am.num_ctrl_pdus,
          (double)m.tx.mode_specific.am.num_ctrl_pdu_bytes * 8 / (double)metrics_period.count());
      logger.log_info("RX metrics period={}ms num_sdus={} sdu_rate={}kbps num_pdus={} pdu_rate={}kbps "
                      "ctrl_pdus={}, ctrl_rate={}kbps",
                      metrics_period.count(),
                      m.rx.num_sdus,
                      (double)m.rx.num_sdu_bytes * 8 / (double)metrics_period.count(),
                      m.rx.num_pdus,
                      (double)m.rx.num_pdu_bytes * 8 / (double)metrics_period.count(),
                      m.tx.mode_specific.am.num_ctrl_pdus,
                      (double)m.rx.mode_specific.am.num_ctrl_pdu_bytes * 8 / (double)metrics_period.count());
    }
    reset_metrics();
    metrics_timer.run();
  }
};

} // namespace srsran
