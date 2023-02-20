/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/radio/radio_notification_handler.h"
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
