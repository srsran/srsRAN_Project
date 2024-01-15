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

#include "srsran/radio/radio_notification_handler.h"
#include "fmt/format.h"

namespace srsran {

class radio_notification_handler_printer : public radio_notification_handler
{
public:
  void on_radio_rt_event(const event_description& description) override
  {
    fmt::print("Radio Realtime Error Event: Type: {} Source: {}\n",
               description.type.to_string(),
               description.source.to_string());
  }
};

} // namespace srsran
