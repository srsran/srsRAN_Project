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
                          task_executor&         ue_executor_,
                          bool                   report_tx_rx_in_same_report_ = true);

  void push_tx_metrics(const pdcp_tx_metrics_container& m_tx_);

  void push_rx_metrics(const pdcp_rx_metrics_container& m_rx_);

  const timer_duration& get_metrics_period() const { return metrics_period; }

private:
  void push_report();

  void push_tx_metrics_impl(const pdcp_tx_metrics_container& m_tx_);
  void push_rx_metrics_impl(const pdcp_rx_metrics_container& m_rx_);

  const bool                report_tx_rx_in_same_report;
  uint32_t                  ue_index;
  rb_id_t                   rb_id;
  pdcp_rx_metrics_container m_rx;
  pdcp_tx_metrics_container m_tx;
  timer_duration            metrics_period;
  pdcp_metrics_notifier*    pdcp_metrics_notif;
  task_executor&            ue_executor;
  pdcp_bearer_logger        logger;
};
} // namespace srsran
