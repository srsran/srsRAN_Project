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

rlc_metrics_aggregator::rlc_metrics_aggregator(rlc_metrics_notifier* rlc_metrics_notif_) :
  rlc_metrics_notif(rlc_metrics_notif_)
{
}

void rlc_metrics_aggregator::push_tx_low_metrics(rlc_tx_metrics_lower m_lower) {}

void rlc_metrics_aggregator::push_tx_high_metrics(rlc_tx_metrics_higher m_higher) {}
