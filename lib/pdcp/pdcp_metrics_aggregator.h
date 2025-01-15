/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "pdcp_bearer_logger.h"
#include "srsran/pdcp/pdcp_metrics.h"
#include "srsran/pdcp/pdcp_rx_metrics.h"
#include "srsran/pdcp/pdcp_tx_metrics.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/timers.h"

namespace srsran {
class pdcp_metrics_aggregator
{
public:
  pdcp_metrics_aggregator(uint32_t               ue_index_,
                          rb_id_t                rb_id_,
                          timer_duration         metrics_period_,
                          pdcp_metrics_notifier* pdcp_metrics_notif_,
                          task_executor&         ue_executor_);

  void push_tx_metrics(pdcp_tx_metrics_container m_tx_);

  void push_rx_metrics(pdcp_rx_metrics_container m_rx_);

  const timer_duration& get_metrics_period() const { return metrics_period; };

private:
  void push_report();

  void push_tx_metrics_impl(pdcp_tx_metrics_container m_tx_);
  void push_rx_metrics_impl(pdcp_rx_metrics_container m_rx_);

  uint32_t                  ue_index;
  pdcp_rx_metrics_container m_rx;
  pdcp_tx_metrics_container m_tx;
  timer_duration            metrics_period;
  pdcp_metrics_notifier*    pdcp_metrics_notif;
  task_executor&            ue_executor;
  pdcp_bearer_logger        logger;
};
} // namespace srsran
