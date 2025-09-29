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

#include "apps/units/flexible_o_du/o_du_low/o_du_low_unit_factory.h"
#include "split6_flexible_o_du_low_session.h"
#include "split6_o_du_low_unit_config.h"
#include "srsran/fapi_adaptor/fapi_slot_messages_adaptor_factory.h"

namespace srsran {

namespace fapi {
class slot_message_gateway;
class slot_last_message_notifier;
struct fapi_cell_config;
class config_message_gateway;
} // namespace fapi

struct worker_manager;

/// Split 6 flexible O-DU low session factory.
class split6_flexible_o_du_low_session_factory
{
  /// Class to calculate the time start of the session.
  class start_time_calculator
  {
    /// Minimum value of the window in milliseconds.
    static constexpr std::chrono::milliseconds MINIMUM_WINDOW_SIZE_MS{5000};

    const std::chrono::milliseconds jitter;
    const std::chrono::milliseconds window_size;
    const std::chrono::milliseconds jitter_upper_border;
    srslog::basic_logger&           logger;

  public:
    start_time_calculator(std::chrono::milliseconds jitter_, srslog::basic_logger& logger_) :
      jitter(jitter_),
      window_size{std::max((jitter * 100U), MINIMUM_WINDOW_SIZE_MS)},
      jitter_upper_border(window_size - jitter),
      logger(logger_)
    {
    }

    /// \brief Returns the start time based on the class configuration.
    ///
    /// This algorithm calculates the start time based on the current time and a configured jitter. It consists on
    /// splitting the time in window_size and calculating the window for the current time. Then \c start_time is
    /// calculated at the end of the next window.
    ///
    ///                                                 border (will log a warning)
    ///           window     window    window             |
    ///    ...X|X........X|X........X|X.......X|X.......X|X...   time
    ///             |                |
    ///             |--------------->|
    ///        current_time       start_time
    ///
    /// Each window defines a lower and upper border, if the current time matches one of these borders, a warning
    /// message that the start might not be synchronized will be logged.
    /// In case that the configured jitter is 0, \c nullopt is returned.
    std::optional<std::chrono::system_clock::time_point> calculate_start_time() const;
  };

  const split6_o_du_low_unit_config                    unit_config;
  worker_manager&                                      workers;
  timer_manager&                                       timers;
  split6_flexible_o_du_low_metrics_notifier*           notifier;
  std::unique_ptr<fapi::slot_messages_adaptor_factory> slot_messages_adaptor_factory;
  start_time_calculator                                start_time_calc;

public:
  split6_flexible_o_du_low_session_factory(
      split6_o_du_low_unit_config                          unit_config_,
      worker_manager&                                      workers_,
      timer_manager&                                       timers_,
      split6_flexible_o_du_low_metrics_notifier*           notifier_,
      std::unique_ptr<fapi::slot_messages_adaptor_factory> slot_messages_adaptor_factory_) :
    unit_config(std::move(unit_config_)),
    workers(workers_),
    timers(timers_),
    notifier(notifier_),
    slot_messages_adaptor_factory(std::move(slot_messages_adaptor_factory_)),
    start_time_calc(std::chrono::milliseconds{unit_config.start_time_jitter_ms}, srslog::fetch_basic_logger("APP"))
  {
    report_error_if_not(slot_messages_adaptor_factory, "Invalid FAPI slot messages adaptor factory");
  }

  /// Creates the split 6 flexible O-DU low session.
  std::unique_ptr<split6_flexible_o_du_low_session> create_o_du_low_session(const fapi::fapi_cell_config& config);

private:
  /// Creates Radio Unit.
  std::unique_ptr<radio_unit> create_radio_unit(split6_flexible_o_du_low_session& odu_low,
                                                const fapi::fapi_cell_config&     config);

  /// Creates O-DU low.
  o_du_low_unit create_o_du_low(const fapi::fapi_cell_config& config,
                                o_du_low_unit_dependencies&&  odu_low_dependencies);
};

} // namespace srsran
