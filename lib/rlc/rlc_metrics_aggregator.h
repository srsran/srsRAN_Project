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
#include "srsran/ran/gnb_du_id.h"
#include "srsran/rlc/rlc_metrics.h"

namespace srsran {
class rlc_metrics_aggregator
{
public:
  rlc_metrics_aggregator(gnb_du_id_t           du,
                         du_ue_index_t         ue,
                         rb_id_t               rb,
                         timer_duration        metrics_period_,
                         rlc_metrics_notifier* rlc_metrics_notif_,
                         task_executor&        ue_executor_);

  // \brief push metrics from the lower RLC executor to the aggregator
  // This will be called will transfer the execution by pushing a copy to
  // ue executor
  void push_tx_low_metrics(rlc_tx_metrics_lower m_lower_);
  // \brief push metrics from the high RLC executors to the aggregator
  // As these are called from the UE executor no execution transfer is required.
  void push_tx_high_metrics(rlc_tx_metrics_higher m_higher_);
  // \brief push metrics from the high RLC executors to the aggregator
  // As these are called from the UE executor no execution transfer is required.
  void push_rx_high_metrics(rlc_rx_metrics m_rx_high_);

private:
  // \brief build and push metrics report to the metrics notifier.
  // Must be run from the UE executor.
  void push_report();

  void push_tx_high_metrics_impl(rlc_tx_metrics_higher m_higher_);
  void push_tx_low_metrics_impl(rlc_tx_metrics_lower m_lower_);
  void push_rx_high_metrics_impl(rlc_rx_metrics m_rx_high_);

  gnb_du_id_t           du;
  du_ue_index_t         ue;
  rb_id_t               rb;
  rlc_tx_metrics_lower  m_lower;
  rlc_tx_metrics_higher m_higher;
  rlc_rx_metrics        m_rx_high;
  timer_duration        metrics_period;

  rlc_metrics_notifier* rlc_metrics_notif;

  task_executor& ue_executor;

  rlc_bearer_logger logger;
};
} // namespace srsran
