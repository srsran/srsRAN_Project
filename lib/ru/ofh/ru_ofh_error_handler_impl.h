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

#include "srsran/ofh/ofh_error_notifier.h"
#include "srsran/ru/ru_error_notifier.h"

namespace srsran {

/// Radio Unit error handler for the Open Fronthaul interface.
class ru_ofh_error_handler_impl : public ofh::error_notifier
{
public:
  explicit ru_ofh_error_handler_impl(ru_error_notifier& notifier_) : error_notifier(notifier_) {}

  // See interface for documentation.
  void on_late_downlink_message(const ofh::error_context& context) override;

private:
  ru_error_notifier& error_notifier;
};

} // namespace srsran
