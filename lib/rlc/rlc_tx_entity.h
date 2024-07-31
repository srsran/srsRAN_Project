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
#include "rlc_metrics_aggregator.h"
#include "rlc_tx_metrics_container.h"
#include "srsran/pcap/rlc_pcap.h"
#include "srsran/rlc/rlc_tx.h"

namespace srsran {

/// Base class used for transmitting RLC bearers.
/// It provides interfaces for the RLC bearers, for both higher layers and lower layers
/// It also stores interfaces for the higher layers, both for the user-plane
/// and the control plane.
class rlc_tx_entity : public rlc_tx_upper_layer_data_interface,
                      public rlc_tx_lower_layer_interface,
                      public rlc_tx_metrics
{
protected:
  rlc_tx_entity(gnb_du_id_t                          gnb_du_id,
                du_ue_index_t                        ue_index,
                rb_id_t                              rb_id_,
                rlc_tx_upper_layer_data_notifier&    upper_dn_,
                rlc_tx_upper_layer_control_notifier& upper_cn_,
                rlc_tx_lower_layer_notifier&         lower_dn_,
                rlc_metrics_aggregator&              metrics_agg_,
                bool                                 metrics_enabled,
                rlc_pcap&                            pcap_,
                task_executor&                       pcell_executor_,
                task_executor&                       ue_executor_,
                timer_manager&                       timers) :
    logger("RLC", {gnb_du_id, ue_index, rb_id_, "DL"}),
    metrics_high(metrics_enabled),
    metrics_low(metrics_enabled),
    rb_id(rb_id_),
    upper_dn(upper_dn_),
    upper_cn(upper_cn_),
    lower_dn(lower_dn_),
    pcap(pcap_),
    pcell_executor{pcell_executor_},
    ue_executor{ue_executor_},
    pcell_timer_factory{timers, pcell_executor},
    ue_timer_factory{timers, ue_executor},
    high_metrics_timer(pcell_timer_factory.create_timer()),
    low_metrics_timer(ue_timer_factory.create_timer()),
    metrics_agg(metrics_agg_)
  {
    if (metrics_enabled) {
      high_metrics_timer.set(std::chrono::milliseconds(1000), [this](timer_id_t tid) {
        metrics_agg.push_tx_high_metrics(metrics_high.get_and_reset_metrics());
        high_metrics_timer.run();
      });
      low_metrics_timer.set(std::chrono::milliseconds(1000), [this](timer_id_t tid) {
        metrics_agg.push_tx_low_metrics(metrics_low.get_and_reset_metrics());
        low_metrics_timer.run();
      });

      high_metrics_timer.run();
      low_metrics_timer.run();
    }
  }

  rlc_bearer_logger                    logger;
  rlc_tx_metrics_high_container        metrics_high;
  rlc_tx_metrics_low_container         metrics_low;
  rb_id_t                              rb_id;
  rlc_tx_upper_layer_data_notifier&    upper_dn;
  rlc_tx_upper_layer_control_notifier& upper_cn;
  rlc_tx_lower_layer_notifier&         lower_dn;
  rlc_pcap&                            pcap;
  task_executor&                       pcell_executor;
  task_executor&                       ue_executor;
  timer_factory                        pcell_timer_factory;
  timer_factory                        ue_timer_factory;

  unique_timer high_metrics_timer;
  unique_timer low_metrics_timer;

private:
  rlc_metrics_aggregator& metrics_agg;

public:
  /// \brief Stops all internal timers.
  ///
  /// This function is inteded to be called upon removal of the bearer before destroying it.
  /// It stops all timers with handlers that may delegate tasks to another executor that could face a deleted object at
  /// a later execution time.
  /// Before this function is called, the adjacent layers should already be disconnected so that no timer is restarted.
  ///
  /// Note: This function shall only be called from ue_executor.
  virtual void stop() = 0;

  rlc_tx_metrics get_metrics()
  {
    rlc_tx_metrics m;
    m.tx_high = metrics_high.get_hi_metrics();
    m.tx_low  = metrics_low.get_low_metrics();
    return m;
  }
};

} // namespace srsran
