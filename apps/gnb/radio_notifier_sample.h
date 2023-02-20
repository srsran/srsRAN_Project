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

#include "srsran/radio/radio_notification_handler.h"
#include "fmt/format.h"

namespace srsran {

class radio_notification_handler_logger : public radio_notification_handler
{
public:
  radio_notification_handler_logger(std::unique_ptr<radio_notification_handler> handler_,
                                    srslog::basic_logger&                       logger_) :
    handler(std::move(handler_)), logger(logger_)
  {
  }

  void on_radio_rt_event(const event_description& description) override
  {
    // Forward event.
    if (handler) {
      handler->on_radio_rt_event(description);
    }

    static const auto& log_format = "Radio realtime event: Type={} Source={} Timestamp={}";

    if (description.timestamp.has_value()) {
      logger.warning(
          log_format, description.type.to_string(), description.source.to_string(), description.timestamp.value());
    } else {
      logger.warning(log_format, description.type.to_string(), description.source.to_string(), "na");
    }
  }

private:
  std::unique_ptr<radio_notification_handler> handler;
  srslog::basic_logger&                       logger;
};

class radio_notification_handler_counter : public radio_notification_handler
{
public:
  radio_notification_handler_counter(std::unique_ptr<radio_notification_handler> handler_) :
    handler(std::move(handler_))
  {
  }

  void on_radio_rt_event(const event_description& description) override
  {
    if (handler) {
      handler->on_radio_rt_event(description);
    }

    // Count events.
    switch (description.type) {
      case event_type::LATE:
        ++late_count;
        break;
      case event_type::UNDERFLOW:
        ++underflow_count;
        break;
      case event_type::OVERFLOW:
        ++overflow_count;
        break;
      case event_type::UNDEFINED:
      case event_type::START_OF_BURST:
      case event_type::END_OF_BURST:
      case event_type::OTHER:
        // Uncounted.
        break;
    }
  }

  void print()
  {
    unsigned temp_late_count      = late_count.exchange(0);
    unsigned temp_underflow_count = underflow_count.exchange(0);
    unsigned temp_overflow_count  = overflow_count.exchange(0);

    // Print late, underflow and overflow counts since last print.
    if ((temp_late_count != 0) || (temp_underflow_count != 0) || (temp_overflow_count != 0)) {
      fmt::print(
          "Late: {}; Underflow: {}; Overflow: {};\n", temp_late_count, temp_underflow_count, temp_overflow_count);
    }
  }

private:
  std::atomic<unsigned>                       late_count      = {};
  std::atomic<unsigned>                       underflow_count = {};
  std::atomic<unsigned>                       overflow_count  = {};
  std::unique_ptr<radio_notification_handler> handler;
};

} // namespace srsran
