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
#include "rlc_metrics_aggregator.h"

using namespace srsran;

rlc_metrics_aggregator::rlc_metrics_aggregator(gnb_du_id_t           du_,
                                               du_ue_index_t         ue_,
                                               rb_id_t               rb_,
                                               timer_duration        metrics_period_,
                                               rlc_metrics_notifier* rlc_metrics_notif_,
                                               task_executor&        ue_executor_) :
  du(du_),
  ue(ue_),
  rb(rb_),
  metrics_period(metrics_period_),
  rlc_metrics_notif(rlc_metrics_notif_),
  ue_executor(ue_executor_),
  logger("RLC", {du_, ue_, rb_, "DL/UL"})
{
  m_lower.counter   = UINT32_MAX;
  m_higher.counter  = UINT32_MAX;
  m_rx_high.counter = UINT32_MAX;
}

void rlc_metrics_aggregator::push_tx_high_metrics(rlc_tx_metrics_higher m_higher_)
{
  if (not ue_executor.execute([this, m_higher_]() { push_tx_high_metrics_impl(m_higher_); })) {
    logger.log_error("Could not push TX high metrics");
  }
}

void rlc_metrics_aggregator::push_tx_low_metrics(rlc_tx_metrics_lower m_lower_)
{
  if (not ue_executor.execute([this, m_lower_]() { push_tx_low_metrics_impl(m_lower_); })) {
    logger.log_error("Could not push TX low metrics");
  }
}

void rlc_metrics_aggregator::push_rx_high_metrics(rlc_rx_metrics m_rx_high_)
{
  if (not ue_executor.execute([this, m_rx_high_]() { push_rx_high_metrics_impl(m_rx_high_); })) {
    logger.log_error("Could not push RX high metrics");
  }
}

void rlc_metrics_aggregator::push_tx_high_metrics_impl(rlc_tx_metrics_higher m_higher_)
{
  m_higher = m_higher_;
  push_report();
}

void rlc_metrics_aggregator::push_tx_low_metrics_impl(rlc_tx_metrics_lower m_lower_)
{
  m_lower = m_lower_;
  push_report();
}

void rlc_metrics_aggregator::push_rx_high_metrics_impl(rlc_rx_metrics m_rx_high_)
{
  m_rx_high = m_rx_high_;
  push_report();
}

void rlc_metrics_aggregator::push_report()
{
  if (m_lower.counter != m_higher.counter || m_lower.counter != m_rx_high.counter) {
    return;
  }
  rlc_metrics report = {du, ue, rb, {m_higher, m_lower}, m_rx_high, 0, metrics_period};
  rlc_metrics_notif->report_metrics(report);
}
