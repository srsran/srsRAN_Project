/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "o_du_metrics_producer.h"

using namespace srsran;

void o_du_metrics_producer_impl::on_new_metrics(const srs_du::o_du_metrics& metrics)
{
  o_du_metrics_impl metric_set(metrics);

  notifier.on_new_metric(metric_set);
}
