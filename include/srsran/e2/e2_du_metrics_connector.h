/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/adt/span.h"
#include "srsran/e2/e2.h"
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

  void report_metrics(span<const scheduler_ue_metrics> ue_metrics) override;

  void report_metrics(const rlc_metrics& metrics) override;

  void get_metrics(scheduler_ue_metrics& ue_metrics) override;

  void connect_e2_du_meas_provider(std::unique_ptr<e2_du_metrics_notifier> meas_provider) override;

private:
  std::deque<scheduler_ue_metrics>        ue_metrics_queue;
  std::unique_ptr<e2_du_metrics_notifier> e2_meas_provider;
};
} // namespace srsran
