/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/radio/radio_notification_handler.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

/// Radio Unit radio event logger.
class ru_radio_logger_event_handler : public radio_notification_handler
{
public:
  explicit ru_radio_logger_event_handler(srslog::basic_logger& logger_) : logger(logger_) {}

  // See interface for documentation.
  void on_radio_rt_event(const event_description& description) override
  {
    if (description.type == event_type::LATE or description.type == event_type::UNDERFLOW or
        description.type == event_type::OVERFLOW) {
      logger.warning("Real-time failure in RF: {}", description.type.to_string());
    }

    static const auto& log_format_debug = "Real-time failure in RF: Type={} Source={} Timestamp={}";

    if (description.timestamp.has_value()) {
      logger.debug(
          log_format_debug, description.type.to_string(), description.source.to_string(), *description.timestamp);
    } else {
      logger.debug(log_format_debug, description.type.to_string(), description.source.to_string(), "na");
    }
  }

private:
  srslog::basic_logger& logger;
};

/// Radio event dispatcher.
class ru_radio_event_dispatcher : public radio_notification_handler
{
  std::vector<radio_notification_handler*> handlers;

public:
  explicit ru_radio_event_dispatcher(std::vector<radio_notification_handler*> handlers_) :
    handlers(std::move(handlers_))
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
