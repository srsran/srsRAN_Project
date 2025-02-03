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

#include "apps/services/app_resource_usage/app_resource_usage.h"
#include "apps/services/metrics/metrics_notifier.h"
#include "apps/services/metrics/metrics_producer.h"
#include "srsran/support/timers.h"

namespace srsran {

/// Resource usage metrics producer implementation.
class resource_usage_metrics_producer_impl : public app_services::metrics_producer
{
public:
  resource_usage_metrics_producer_impl(app_services::metrics_notifier&   notifier_,
                                       app_services::app_resource_usage& app_resource_usage_) :
    notifier(notifier_), app_resource_usage(app_resource_usage_)
  {
  }

  void on_new_report_period() override
  {
    resource_usage_metrics new_metrics = app_resource_usage.get_new_metrics();
    notifier.on_new_metric(resource_usage_metrics_impl(new_metrics));
  }

private:
  app_services::metrics_notifier&   notifier;
  app_services::app_resource_usage& app_resource_usage;
};

} // namespace srsran
