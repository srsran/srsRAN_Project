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

#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier.h"
#include "srsran/ofh/transmitter/ofh_downlink_handler.h"

namespace srsran {
namespace ofh {

class error_notifier;

/// Open Fronthaul downlink manager.
class downlink_manager
{
public:
  /// Default destructor.
  virtual ~downlink_manager() = default;

  /// Returns the downlink handler of this downlink manager.
  virtual downlink_handler& get_downlink_handler() = 0;

  /// Returns the OTA symbol boundary notifier of this downlink manager.
  virtual ota_symbol_boundary_notifier& get_ota_symbol_boundary_notifier() = 0;

  /// Sets the error notifier of this sector to the given one.
  virtual void set_error_notifier(error_notifier& notifier) = 0;
};

} // namespace ofh
} // namespace srsran
