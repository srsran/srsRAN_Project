/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/e2/e2_cu_metrics_connector.h"

using namespace srsran;

e2_cu_metrics_connector::e2_cu_metrics_connector() = default;

void e2_cu_metrics_connector::connect_e2_cu_meas_provider(std::unique_ptr<e2_cu_metrics_notifier> meas_provider)
{
  e2_meas_provider = std::move(meas_provider);
}