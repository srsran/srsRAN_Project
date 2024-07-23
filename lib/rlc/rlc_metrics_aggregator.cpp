/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
    // TODO log error
  }
}

void rlc_metrics_aggregator::push_tx_low_metrics(rlc_tx_metrics_lower m_lower_)
{
  if (not ue_executor.execute([this, m_lower_]() { push_tx_low_metrics_impl(m_lower_); })) {
    // TODO log error
  }
}

void rlc_metrics_aggregator::push_rx_high_metrics(rlc_rx_metrics m_rx_high_)
{
  if (not ue_executor.execute([this, m_rx_high_]() { push_rx_high_metrics_impl(m_rx_high_); })) {
    // TODO log error
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
  if (m_lower.counter != m_higher.counter || m_lower.counter != m_rx_high.counter ||
      m_higher.counter != m_rx_high.counter) {
    return;
  }
  rlc_metrics report = {du, ue, rb, {m_higher, m_lower}, m_rx_high, 0, metrics_period};
  rlc_metrics_notif->report_metrics(report);
}
