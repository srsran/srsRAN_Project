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

#include "pdcp_metrics_aggregator.h"
#include "srsran/pdcp/pdcp_entity.h"

using namespace srsran;

pdcp_metrics_aggregator::pdcp_metrics_aggregator(uint32_t               ue_index_,
                                                 rb_id_t                rb_id_,
                                                 timer_duration         metrics_period_,
                                                 pdcp_metrics_notifier* pdcp_metrics_notif_,
                                                 task_executor&         ue_executor_,
                                                 bool                   report_tx_rx_in_same_report_) :
  report_tx_rx_in_same_report(report_tx_rx_in_same_report_),
  ue_index(ue_index_),
  rb_id(rb_id_),
  metrics_period(metrics_period_),
  pdcp_metrics_notif(pdcp_metrics_notif_),
  ue_executor(ue_executor_),
  logger("PDCP", {ue_index_, rb_id_, "UL"})
{
  // Initialize to different values so the first call to push metrics does not force a metrics report until tx and rx
  // are present.
  m_tx.counter = 10;
  m_rx.counter = 50;
}

void pdcp_metrics_aggregator::push_tx_metrics(const pdcp_tx_metrics_container& m_tx_)
{
  if (not ue_executor.execute([this, m_tx_]() { push_tx_metrics_impl(m_tx_); })) {
    logger.log_error("Could not push TX metrics");
  }
}

void pdcp_metrics_aggregator::push_rx_metrics(const pdcp_rx_metrics_container& m_rx_)
{
  if (not ue_executor.execute([this, m_rx_]() { push_rx_metrics_impl(m_rx_); })) {
    logger.log_error("Could not push RX metrics");
  }
}

void pdcp_metrics_aggregator::push_tx_metrics_impl(const pdcp_tx_metrics_container& m_tx_)
{
  m_tx = m_tx_;
  push_report();
}

void pdcp_metrics_aggregator::push_rx_metrics_impl(const pdcp_rx_metrics_container& m_rx_)
{
  m_rx = m_rx_;
  push_report();
}

void pdcp_metrics_aggregator::push_report()
{
  // Notify tx and rx in the same PDCP metrics container.
  if (report_tx_rx_in_same_report && m_rx.counter != m_tx.counter) {
    return;
  }

  pdcp_metrics_container metrics = {ue_index, rb_id, m_tx, m_rx, metrics_period};
  pdcp_metrics_notif->report_metrics(metrics);
}
