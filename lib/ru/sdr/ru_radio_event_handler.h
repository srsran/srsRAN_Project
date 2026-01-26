/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
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

#include "srsran/radio/radio_event_notifier.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

/// Radio Unit radio event logger.
class ru_radio_logger_event_handler : public radio_event_notifier
{
public:
  explicit ru_radio_logger_event_handler(srslog::basic_logger& logger_) : logger(logger_) {}

  // See interface for documentation.
  void on_radio_rt_event(const event_description& description) override
  {
    if (description.type == radio_event_type::LATE || description.type == radio_event_type::UNDERFLOW ||
        description.type == radio_event_type::OVERFLOW) {
      logger.warning("Real-time failure in RF: {}", to_string(description.type));
    }

    static const auto& log_format_debug = "Real-time failure in RF: Type={} Source={} Timestamp={}";

    if (description.timestamp.has_value()) {
      logger.debug(
          log_format_debug, to_string(description.type), to_string(description.source), *description.timestamp);
    } else {
      logger.debug(log_format_debug, to_string(description.type), to_string(description.source), "na");
    }
  }

private:
  srslog::basic_logger& logger;
};

/// Radio event dispatcher.
class ru_radio_event_dispatcher : public radio_event_notifier
{
  std::vector<radio_event_notifier*> handlers;

public:
  explicit ru_radio_event_dispatcher(std::vector<radio_event_notifier*> handlers_) : handlers(std::move(handlers_))
  {
    srsran_assert(!handlers.empty(), "Empty list of radio event notifiers");
    srsran_assert(std::all_of(handlers.begin(), handlers.end(), [](auto* handler) { return handler != nullptr; }),
                  "Invalid radio event notifier");
  }

  // See interface for documentation.
  void on_radio_rt_event(const event_description& description) override
  {
    for (auto* handler : handlers) {
      handler->on_radio_rt_event(description);
    }
  }
};

} // namespace srsran
