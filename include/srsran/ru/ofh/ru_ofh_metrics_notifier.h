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

#include "srsran/ru/ofh/ru_ofh_metrics.h"

namespace srsran {

/// Open Fronthaul Radio Unit metrics notifier.
class ru_ofh_metrics_notifier
{
public:
  virtual ~ru_ofh_metrics_notifier() = default;

  /// Notifies new Open Fronthaul Radio Unit metrics.
  virtual void on_new_metrics(const ru_ofh_metrics& metrics) = 0;
};

} // namespace srsran
