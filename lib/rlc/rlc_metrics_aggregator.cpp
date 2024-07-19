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
                                               rlc_metrics_notifier* rlc_metrics_notif_,
                                               task_executor&        ue_executor_) :
  du(du_), ue(ue_), rb(rb_), rlc_metrics_notif(rlc_metrics_notif_), ue_executor(ue_executor_)
{
  m_lower.counter  = UINT32_MAX;
  m_higher.counter = UINT32_MAX;
}

void rlc_metrics_aggregator::push_tx_high_metrics(rlc_tx_metrics_higher m_higher_)
{
  fmt::print("high metrics {}\n", m_higher_.counter);
  if (not ue_executor.execute([this, m_higher_]() { push_tx_high_metrics_impl(m_higher_); })) {
    // TODO log error
  }
}

// TODO make sure this is run in the UE executor
void rlc_metrics_aggregator::push_tx_low_metrics(rlc_tx_metrics_lower m_lower_)
{
  if (not ue_executor.execute([this, m_lower_]() { push_tx_low_metrics_impl(m_lower_); })) {
    // TODO log error
  }
}

void rlc_metrics_aggregator::push_tx_high_metrics_impl(rlc_tx_metrics_higher m_higher_)
{
  fmt::print("high metrics {}\n", m_higher_.counter);
  m_higher = m_higher_;
  if (m_lower.counter != m_higher.counter) {
    return;
  }
  push_report();
}

void rlc_metrics_aggregator::push_tx_low_metrics_impl(rlc_tx_metrics_lower m_lower_)
{
  fmt::print("low metrics {}\n", m_lower_.counter);
  m_lower = m_lower_;
  if (m_lower.counter != m_higher.counter) {
    return;
  }
  push_report();
}

void rlc_metrics_aggregator::push_report()
{
  rlc_metrics report = {du, ue, rb, {m_higher, m_lower}, {}, 0, std::chrono::milliseconds{1000}};
  rlc_metrics_notif->report_metrics(report);
}
