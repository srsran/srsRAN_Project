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

#include "srsran/ofh/timing/slot_symbol_point.h"
#include <chrono>

namespace srsran {
namespace ofh {

/// Slot symbol time point context.
struct slot_symbol_point_context {
  slot_symbol_point                                  symbol_point;
  std::chrono::time_point<std::chrono::system_clock> time_point;
};

/// This interface notifies the timing boundary of an OFDM symbol over the air.
class ota_symbol_boundary_notifier
{
public:
  virtual ~ota_symbol_boundary_notifier() = default;

  /// \brief Notifies a new OTA symbol boundary event.
  ///
  /// Notifies that the beginning of a new OTA symbol has started.
  ///
  /// \param[in] symbol_point_context Current slot and symbol point.
  virtual void on_new_symbol(const slot_symbol_point_context& symbol_point_context) = 0;
};

} // namespace ofh
} // namespace srsran
