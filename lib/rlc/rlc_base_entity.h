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
  rlc_base_entity(du_ue_index_t ue_index, rb_id_t rb_id, timer_factory timers) :
    logger("RLC", {ue_index, rb_id, "DL/UL"}), metrics_timer(timers.create_timer())
  {
    metrics_timer.set(metrics_period, [this](timer_id_t tid) { push_metrics(); });
    metrics_timer.run();
  }
  ~rlc_base_entity() override                         = default;
  rlc_base_entity(const rlc_base_entity&)             = delete;
  rlc_base_entity& operator=(const rlc_base_entity&)  = delete;
  rlc_base_entity(const rlc_base_entity&&)            = delete;
  rlc_base_entity& operator=(const rlc_base_entity&&) = delete;

  rlc_tx_upper_layer_data_interface* get_tx_upper_layer_data_interface() final { return tx.get(); };
  rlc_tx_lower_layer_interface*      get_tx_lower_layer_interface() final { return tx.get(); };
  rlc_rx_lower_layer_interface*      get_rx_lower_layer_interface() final { return rx.get(); };

  rlc_metrics get_metrics() final
  {
    rlc_metrics metrics = {};
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
  std::unique_ptr<rlc_tx_entity> tx = {};
  std::unique_ptr<rlc_rx_entity> rx = {};

private:
  timer_duration metrics_period = std::chrono::milliseconds(1000);
  unique_timer   metrics_timer;
  void           push_metrics()
  {
    rlc_metrics m = get_metrics();
    if (m.tx.mode == rlc_mode::am) {
      logger.log_warning("TX metrics period={}ms num_sdus={} sdu_rate={}kbps, num_pdus={} pdu_rate={}kbps num_retx={} "
                         "retx_rate={}kbps ctrl_pdus={} ctrl_rate={}kbps",
                         metrics_period.count(),
                         m.tx.num_sdus,
                         (double)m.tx.num_sdu_bytes * 8 / (double)metrics_period.count(),
                         m.tx.num_pdus,
                         (double)m.tx.num_pdu_bytes * 8 / (double)metrics_period.count(),
                         m.tx.mode_specific.am.num_retx_pdus,
                         (double)m.tx.mode_specific.am.num_retx_pdu_bytes * 8 / (double)metrics_period.count(),
                         m.tx.mode_specific.am.num_ctrl_pdus,
                         (double)m.tx.mode_specific.am.num_ctrl_pdu_bytes * 8 / (double)metrics_period.count());
      logger.log_warning("RX metrics period={}ms num_sdus={} sdu_rate={}kbps num_pdus={} pdu_rate={}kbps "
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
