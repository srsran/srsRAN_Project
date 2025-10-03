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
