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

#include "srsran/ran/slot_point.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul error context.
struct error_context {
  /// Slot context.
  slot_point slot;
  /// Radio sector identifier.
  unsigned sector;
};

/// Open Fronthaul error notifier.
class error_notifier
{
public:
  virtual ~error_notifier() = default;

  /// \brief Notifies a late downlink message.
  ///
  /// \param[in] context Context of the error.
  virtual void on_late_downlink_message(const error_context& context) = 0;

  /// \brief Notifies a late uplink request message.
  ///
  /// \param[in] context Context of the error.
  virtual void on_late_uplink_message(const error_context& context) = 0;

  /// \brief Notifies a late PRACH request message.
  ///
  /// \param[in] context Context of the error.
  virtual void on_late_prach_message(const error_context& context) = 0;
};

} // namespace ofh
} // namespace srsran
