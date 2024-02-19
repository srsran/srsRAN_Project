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

#include "srsran/phy/upper/upper_phy_error_handler.h"
#include "srsran/phy/upper/upper_phy_error_notifier.h"
#include <functional>

namespace srsran {

/// Upper physical layer error handler implementation.
class upper_phy_error_handler_impl : public upper_phy_error_handler
{
public:
  upper_phy_error_handler_impl();

  // See interface for documentation.
  void handle_late_downlink_message(slot_point slot) override;

  /// Sets the given notifier as the error notifier of this handler.
  void set_error_notifier(upper_phy_error_notifier& notifier) { error_notifier = std::ref(notifier); }

private:
  std::reference_wrapper<upper_phy_error_notifier> error_notifier;
};

} // namespace srsran
