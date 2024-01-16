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
namespace fapi {

/// \brief Slot-based, last message notifier interface.
///
/// This interface notifies, for each slot, the transmission of the last FAPI message that has been sent through the
/// FAPI message gateway.
class slot_last_message_notifier
{
public:
  virtual ~slot_last_message_notifier() = default;

  /// \brief Notifies the transmission of the last message corresponding to the given slot.
  ///
  /// \param[in] slot Current slot.
  virtual void on_last_message(slot_point slot) = 0;
};

} // namespace fapi
} // namespace srsran
