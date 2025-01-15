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

#include "srsran/e2/e2.h"
#include "srsran/rlc/rlc_metrics.h"
#include "srsran/scheduler/scheduler_metrics.h"

namespace srsran {

class e2_du_metrics_notifier : public scheduler_metrics_notifier, public rlc_metrics_notifier
{
public:
  virtual ~e2_du_metrics_notifier() = default;

  using rlc_metrics_notifier::report_metrics;
  using scheduler_metrics_notifier::report_metrics;
};

class e2_du_metrics_interface
{
public:
  virtual ~e2_du_metrics_interface() = default;

  /// \brief connects e2_du_metric_provider
  /// \param[in] meas_provider pointer to the e2_du_metric_provider
  virtual void connect_e2_du_meas_provider(std::unique_ptr<e2_du_metrics_notifier> meas_provider) = 0;
};
} // namespace srsran
