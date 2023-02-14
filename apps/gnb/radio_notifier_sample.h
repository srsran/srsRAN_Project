/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/radio/radio_notification_handler.h"
#include "fmt/format.h"

namespace srsgnb {

class radio_notification_handler_printer : public radio_notification_handler
{
public:
  radio_notification_handler_printer(srslog::basic_logger& logger_) : logger(logger_) {}

  void on_radio_rt_event(const event_description& description) override
  {
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

    static const auto& log_format = "Radio realtime event: Type={} Source={} Timestamp={}";

    if (description.timestamp.has_value()) {
      logger.warning(
          log_format, description.type.to_string(), description.source.to_string(), description.timestamp.value());
    } else {
      logger.warning(log_format, description.type.to_string(), description.source.to_string(), "na");
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
  std::atomic<unsigned> late_count      = {};
  std::atomic<unsigned> underflow_count = {};
  std::atomic<unsigned> overflow_count  = {};
  srslog::basic_logger& logger;
};

} // namespace srsgnb
