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

#include "apps/services/buffer_pool/buffer_pool_manager.h"
#include "apps/services/buffer_pool/metrics/buffer_pool_metrics.h"
#include "apps/services/metrics/metrics_notifier.h"
#include "apps/services/metrics/metrics_producer.h"

namespace srsran {
namespace app_services {

/// Buffer pool metrics producer implementation.
class buffer_pool_metrics_producer_impl : public app_services::metrics_producer
{
public:
  explicit buffer_pool_metrics_producer_impl(app_services::metrics_notifier& notifier_) : notifier(notifier_) {}

  void on_new_report_period() override
  {
    size_t central_cache_size = buffer_pool_manager::get_central_cache_size();
    notifier.on_new_metric(buffer_pool_metrics_impl(central_cache_size));
  }

private:
  app_services::metrics_notifier& notifier;
};

} // namespace app_services
} // namespace srsran
