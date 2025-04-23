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

#include "srsran/radio/radio_notification_handler.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

/// Radio Unit radio notification logger.
class ru_radio_notification_handler_logger : public radio_notification_handler
{
public:
  explicit ru_radio_notification_handler_logger(srslog::basic_logger& logger_) : logger(logger_) {}

  // See interface for documentation.
  void on_radio_rt_event(const event_description& description) override
  {
    if (description.type == event_type::LATE or description.type == event_type::UNDERFLOW or
        description.type == event_type::OVERFLOW) {
      logger.warning("Real-time failure in RF: {}", description.type.to_string());
    }

    static const auto& log_format_debug = "Real-time failure in RF: Type={} Source={} Timestamp={}";

    if (description.timestamp.has_value()) {
      logger.debug(log_format_debug,
                   description.type.to_string(),
                   description.source.to_string(),
                   description.timestamp.value());
    } else {
      logger.debug(log_format_debug, description.type.to_string(), description.source.to_string(), "na");
    }
  }

private:
  srslog::basic_logger& logger;
};

/// Radio notification event dispatcher.
class ru_radio_notification_dispatcher : public radio_notification_handler
{
  std::vector<radio_notification_handler*> handlers;

public:
  explicit ru_radio_notification_dispatcher(std::vector<radio_notification_handler*> handlers_) :
    handlers(std::move(handlers_))
  {
    srsran_assert(!handlers.empty(), "Invalid list of radio notification handlers");
    for (const auto& handler : handlers) {
      srsran_assert(handler, "Invalid radio notification handler");
    }
  }

  // See interface for documentation.
  void on_radio_rt_event(const event_description& description) override
  {
    for (auto handler : handlers) {
      handler->on_radio_rt_event(description);
    }
  }
};

} // namespace srsran
