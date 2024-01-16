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

#include "srsran/phy/lower/lower_phy_timing_context.h"

namespace srsran {

/// Lower physical layer interface to notify timing related events.
class lower_phy_timing_notifier
{
public:
  /// Default destructor.
  virtual ~lower_phy_timing_notifier() = default;

  /// \brief Notifies a new TTI boundary event.
  ///
  /// Notifies that the processing of a new slot has started. It indicates in \c context the next slot available for
  /// transmission.
  ///
  /// \param[in] context Notification context.
  virtual void on_tti_boundary(const lower_phy_timing_context& context) = 0;

  /// \brief Notifies that an uplink half slot has been received and processed by the lower PHY.
  ///
  /// \param[in] context Notification context.
  virtual void on_ul_half_slot_boundary(const lower_phy_timing_context& context) = 0;

  /// \brief Notifies that an uplink full slot has been received and processed by the lower PHY.
  ///
  /// After this notification, the lower PHY does not report more uplink receive symbols events for the given context.
  ///
  /// \param[in] context Notification context.
  virtual void on_ul_full_slot_boundary(const lower_phy_timing_context& context) = 0;
};

} // namespace srsran
