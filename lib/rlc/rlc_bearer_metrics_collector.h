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

#include "rlc_bearer_logger.h"
#include "srsran/adt/lockfree_triple_buffer.h"
#include "srsran/ran/gnb_du_id.h"
#include "srsran/rlc/rlc_metrics.h"

namespace srsran {
class rlc_bearer_metrics_collector
{
public:
  rlc_bearer_metrics_collector(gnb_du_id_t           du,
                               du_ue_index_t         ue,
                               rb_id_t               rb,
                               timer_duration        metrics_period_,
                               rlc_metrics_notifier* rlc_metrics_notif_,
                               task_executor&        ue_executor_);

  /// \brief push metrics from the lower RLC executor to the collector.
  ///
  /// This will transfer the execution to UE executor; a copy of the metrics is passed through a triple buffer.
  void push_tx_low_metrics(const rlc_tx_metrics_lower& m_lower_);

  /// \brief push metrics from the high RLC executors to the collector.
  ///
  /// As these are called from the UE executor no execution transfer is required.
  void push_tx_high_metrics(const rlc_tx_metrics_higher& m_higher_);

  /// \brief push metrics from the high RLC executors to the collector.
  ///
  /// As these are called from the UE executor no execution transfer is required.
  void push_rx_high_metrics(const rlc_rx_metrics& m_rx_high_);

  /// \brief get metrics report period from the collector.
  const timer_duration& get_metrics_period() const { return metrics_period; }

private:
  /// \brief build and push metrics report to the metrics notifier.
  ///
  /// Must be run from the UE executor.
  void push_report();

  void push_tx_high_metrics_impl(const rlc_tx_metrics_higher& m_higher_);
  void push_tx_low_metrics_impl(const rlc_tx_metrics_lower& m_lower_);
  void push_rx_high_metrics_impl(const rlc_rx_metrics& m_rx_high_);

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

  /// Triple buffer for alloc-free forwarding of \c rlc_tx_metrics_lower from cell executor to ue executor
  lockfree_triple_buffer<rlc_tx_metrics_lower> metrics_lower_triple_buf;
};
} // namespace srsran
