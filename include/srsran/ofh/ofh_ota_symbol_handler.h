/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/ofh/slot_symbol_point.h"

namespace srsran {
namespace ofh {

/// \brief Open Fronthaul interface to handle OTA symbol-based timing events.
class ota_symbol_handler
{
public:
  /// Default destructor.
  virtual ~ota_symbol_handler() = default;

  /// \brief Callback function called on a new OFDM symbol timing event.
  /// \param[in] symbol_point Current slot and symbol point.
  virtual void handle_new_ota_symbol(slot_symbol_point symbol_point) = 0;
};

} // namespace ofh
} // namespace srsran
