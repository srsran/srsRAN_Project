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

#include "srsran/du/du_high/du_manager/du_manager.h"
#include "srsran/mac/mac_metrics_notifier.h"

namespace srsran::srs_du {

class du_manager_metrics_handler_impl final : public du_manager_metrics_handler
{
public:
  du_manager_metrics_handler_impl(mac_metrics_notifier* mac_notifier_);

  void handle_mac_metrics_report(const mac_metric_report& report) override;

private:
  mac_metrics_notifier& mac_notifier;
};

} // namespace srsran::srs_du
