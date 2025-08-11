/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "apps/services/metrics/metrics_notifier.h"
#include "apps/services/metrics/metrics_producer.h"
#include "srsran/cu_cp/cu_cp_metrics_notifier.h"

namespace srsran {

/// CU-CP metrics producer implementation.
class cu_cp_metrics_producer_impl : public cu_cp_metrics_report_notifier, public app_services::metrics_producer
{
public:
  explicit cu_cp_metrics_producer_impl(app_services::metrics_notifier& notifier_) : notifier(notifier_) {}

  // See interface for documentation.
  void notify_metrics_report_request(const cu_cp_metrics_report& metrics) override;

  // See interface for documentation.
  void on_new_report_period() override {}

private:
  app_services::metrics_notifier& notifier;
};

} // namespace srsran
