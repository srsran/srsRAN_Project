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
#include "srsran/ran/slot_point.h"

namespace srsran {

struct lower_phy_timing_context;
struct resource_grid_context;

/// \brief Lower physical layer downlink processor - Notifier interface.
///
/// Notifies time boundary events.
class downlink_processor_notifier
{
public:
  /// Default destructor.
  virtual ~downlink_processor_notifier() = default;

  /// \brief Notifies a new TTI boundary event.
  ///
  /// See \ref lower_phy_timing_notifier::on_tti_boundary for more information.
  /// \param[in] context Notification context.
  virtual void on_tti_boundary(const lower_phy_timing_context& context) = 0;
};

} // namespace srsran