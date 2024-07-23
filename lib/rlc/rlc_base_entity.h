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
  rlc_base_entity(gnb_du_id_t           gnb_du_id_,
                  du_ue_index_t         ue_index_,
                  rb_id_t               rb_id_,
                  timer_duration        metrics_period_,
                  rlc_metrics_notifier* rlc_metrics_notifier_,
                  task_executor&        ue_executor_) :
    logger("RLC", {gnb_du_id_, ue_index_, rb_id_, "DL/UL"}),
    ue_index(ue_index_),
    rb_id(rb_id_),
    metrics_period(metrics_period_),
    metrics_agg(gnb_du_id_, ue_index, rb_id_, metrics_period_, rlc_metrics_notifier_, ue_executor_)
  {
    rlc_metrics_notif = rlc_metrics_notifier_;
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
  std::unique_ptr<rlc_tx_entity> tx;
  std::unique_ptr<rlc_rx_entity> rx;
  timer_duration                 metrics_period;
  rlc_metrics_aggregator         metrics_agg;

private:
  rlc_metrics_notifier* rlc_metrics_notif;
};

} // namespace srsran
