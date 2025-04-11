/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_metrics_handler_impl.h"

using namespace srsran;
using namespace srs_du;

namespace {

class mac_metrics_null_notifier final : public mac_metrics_notifier
{
public:
  void on_new_metrics_report(const mac_metric_report& metrics) override
  {
    // do nothing.
  }
};

// Null notifier for mac metrics, used in case no mac metrics notifier is provided in the config.
mac_metrics_null_notifier null_mac_notifier;

} // namespace

du_manager_metrics_handler_impl::du_manager_metrics_handler_impl(mac_metrics_notifier* mac_notifier_) :
  mac_notifier(mac_notifier_ != nullptr ? *mac_notifier_ : null_mac_notifier)
{
}

void du_manager_metrics_handler_impl::handle_mac_metrics_report(const mac_metric_report& report)
{
  mac_notifier.on_new_metrics_report(report);
}
