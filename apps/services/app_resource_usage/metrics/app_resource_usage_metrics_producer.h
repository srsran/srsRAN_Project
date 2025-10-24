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

#pragma once

#include "apps/services/app_resource_usage/app_resource_usage.h"
#include "apps/services/metrics/metrics_notifier.h"
#include "apps/services/metrics/metrics_producer.h"

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
