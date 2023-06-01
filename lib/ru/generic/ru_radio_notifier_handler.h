/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/support/executors/task_executor.h"
#include "fmt/format.h"

namespace srsran {

/// Radio Unit radio notification logger.
class ru_radio_notification_handler_logger : public radio_notification_handler
{
public:
  ru_radio_notification_handler_logger(std::unique_ptr<radio_notification_handler> handler_,
                                       srslog::basic_logger&                       logger_) :
    handler(std::move(handler_)), logger(logger_)
  {
  }

  // See interface for documentation.
  void on_radio_rt_event(const event_description& description) override
  {
    // Forward event.
    if (handler) {
      handler->on_radio_rt_event(description);
    }

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
  std::unique_ptr<radio_notification_handler> handler;
  srslog::basic_logger&                       logger;
};

/// Radio Unit radio notification counter.
class ru_radio_notification_handler_counter : public radio_notification_handler
{
public:
  explicit ru_radio_notification_handler_counter(std::unique_ptr<radio_notification_handler> handler_,
                                                 task_executor&                              executor_,
                                                 unsigned                                    print_interval_in_slots_) :
    handler(std::move(handler_)),
    executor(executor_),
    print_interval_in_slots(print_interval_in_slots_),
    slot_counter(0)
  {
  }

  // See interface for documentation.
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

  /// Enqueue the task of printing statistics.
  void print()
  {
    ++slot_counter;

    if (slot_counter == print_interval_in_slots) {
      slot_counter = 0;
      executor.defer([this]() { print_statistics(); });
    }
  }

private:
  void print_statistics()
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
  task_executor&                              executor;
  const unsigned                              print_interval_in_slots;
  unsigned                                    slot_counter;
};

} // namespace srsran
