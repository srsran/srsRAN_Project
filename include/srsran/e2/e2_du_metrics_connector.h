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

#include "srsran/adt/span.h"
#include "srsran/e2/e2_du.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include <deque>

namespace srsran {

constexpr int MAX_UE_METRICS = 10;

/// \brief Class used to receive metrics reports from scheduler and sends them to the e2 interface.
class e2_du_metrics_connector : public e2_du_metrics_notifier, public e2_du_metrics_interface
{
public:
  e2_du_metrics_connector();
  ~e2_du_metrics_connector() = default;

  void report_metrics(const scheduler_cell_metrics& metrics) override;

  void report_metrics(const rlc_metrics& metrics) override;

  void connect_e2_du_meas_provider(std::unique_ptr<e2_du_metrics_notifier> meas_provider) override;

private:
  std::unique_ptr<e2_du_metrics_notifier> e2_meas_provider;
};
} // namespace srsran
