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

#include "srsran/pdcp/pdcp_entity.h"
#include "srsran/ran/cu_types.h"
#include "srsran/ran/gnb_id.h"
#include "srsran/ran/rb_id.h"

namespace srsran {

struct pdcp_metrics_container;

/// \brief Notifier interface used to report RLC metrics.
class pdcp_metrics_notifier
{
public:
  virtual ~pdcp_metrics_notifier() = default;

  /// \brief This method will be called periodically to report the latest PDCP metrics statistics.
  virtual void report_metrics(const pdcp_metrics_container& metrics) = 0;
};
} // namespace srsran
