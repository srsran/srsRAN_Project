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

#include "srsran/f1ap/du/f1ap_du_metrics_collector.h"

namespace srsran {
namespace srs_du {

class f1ap_du_metrics_collector_impl final : public f1ap_metrics_collector
{
public:
  void handle_metrics_report_request(f1ap_metrics_report& report) override;
};

} // namespace srs_du
} // namespace srsran
