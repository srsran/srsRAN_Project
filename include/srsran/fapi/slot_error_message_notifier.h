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

namespace srsran {
namespace fapi {

struct error_indication_message;

/// \brief Slot-based, error-specific message notifier interface.
///
/// This interface notifies the reception of slot-based, error-specific FAPI messages from the underlying PHY.
class slot_error_message_notifier
{
public:
  virtual ~slot_error_message_notifier() = default;

  /// \brief Notifies the reception of an error indication message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_error_indication(const error_indication_message& msg) = 0;
};

} // namespace fapi
} // namespace srsran
