/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_low_metrics_collector_impl.h"
#include "srsran/du/du_low/du_low_metrics.h"

using namespace srsran;
using namespace srs_du;

void du_low_metrics_collector_impl::collect_metrics(du_low_metrics& metrics)
{
  if (upper_phy_collector == nullptr) {
    return;
  }

  upper_phy_collector->collect_metrics(metrics.upper_metrics);
}
