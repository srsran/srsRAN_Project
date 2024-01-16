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
#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier.h"

namespace srsran {
namespace ofh {

/// \brief OTA symbol boundary notifier manager.
///
/// Interface to subscribe OTA symbol boundary notifiers.
class ota_symbol_boundary_notifier_manager
{
public:
  /// Default destructor.
  virtual ~ota_symbol_boundary_notifier_manager() = default;

  /// Subscribes the given notifiers to listen to OTA symbol boundary events.
  virtual void subscribe(span<ota_symbol_boundary_notifier*> notifiers) = 0;
};

} // namespace ofh
} // namespace srsran
