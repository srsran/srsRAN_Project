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

#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

struct lower_phy_baseband_metrics;

/// Lower physical layer interface used to notify basebend measurements.
class lower_phy_metrics_notifier
{
public:
  /// Default destructor.
  virtual ~lower_phy_metrics_notifier() = default;

  /// \brief Notifies a new transmit symbol measurement.
  ///
  /// \param[in] metrics Measurements of the transmitted symbol.
  virtual void on_new_transmit_metrics(const lower_phy_baseband_metrics& metrics) = 0;

  /// \brief Notifies a new receive symbol measurement.
  ///
  /// \param[in] metrics Measurements of the received symbol.
  virtual void on_new_receive_metrics(const lower_phy_baseband_metrics& metrics) = 0;
};

} // namespace srsran
