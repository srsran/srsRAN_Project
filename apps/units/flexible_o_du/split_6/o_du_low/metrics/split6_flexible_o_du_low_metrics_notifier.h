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

namespace srsran {

struct split6_flexible_o_du_low_metrics;

/// Split 6 flexible O-DU low metrics notifier.
class split6_flexible_o_du_low_metrics_notifier
{
public:
  virtual ~split6_flexible_o_du_low_metrics_notifier() = default;

  /// Notifies new split 6 flexible O-DU low metrics.
  virtual void on_new_metrics(const split6_flexible_o_du_low_metrics& metrics) = 0;
};

} // namespace srsran
