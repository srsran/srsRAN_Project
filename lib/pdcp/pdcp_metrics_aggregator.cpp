/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcp_metrics_aggregator.h"
#include "srsran/pdcp/pdcp_entity.h"

using namespace srsran;

pdcp_metrics_aggregator::pdcp_metrics_aggregator(uint32_t               ue_index_,
                                                 rb_id_t                rb_id_,
                                                 timer_duration         metrics_period_,
                                                 pdcp_metrics_notifier* pdcp_metrics_notif_,
                                                 task_executor&         ue_executor_) :
  metrics_period(metrics_period_),
  pdcp_metrics_notif(pdcp_metrics_notif_),
  ue_executor(ue_executor_),
  logger("PDCP", {ue_index_, rb_id_, "UL"})
{
  m_tx.counter = 0;
  m_rx.counter = 0;
}

void pdcp_metrics_aggregator::push_tx_metrics(pdcp_tx_metrics_container m_tx_)
{
  if (not ue_executor.execute([this, m_tx_]() { push_tx_metrics_impl(m_tx_); })) {
    logger.log_error("Could not push TX metrics");
  }
}

void pdcp_metrics_aggregator::push_rx_metrics(pdcp_rx_metrics_container m_rx_)
{
  if (not ue_executor.execute([this, m_rx_]() { push_rx_metrics_impl(m_rx_); })) {
    logger.log_error("Could not push RX metrics");
  }
}

void pdcp_metrics_aggregator::push_tx_metrics_impl(pdcp_tx_metrics_container m_tx_)
{
  m_tx = m_tx_;
  m_tx.counter++;
  push_report();
}

void pdcp_metrics_aggregator::push_rx_metrics_impl(pdcp_rx_metrics_container m_rx_)
{
  m_rx = m_rx_;
  m_rx.counter++;
  push_report();
}

void pdcp_metrics_aggregator::push_report()
{
  pdcp_metrics_container metrics = {ue_index, m_tx, m_rx, metrics_period};
  pdcp_metrics_notif->report_metrics(metrics);
}