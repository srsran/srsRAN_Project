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
