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

#include "srsran/adt/span.h"
#include "srsran/ofh/ofh_controller.h"
#include "srsran/ru/ru_controller.h"
#include "srsran/srslog/logger.h"
#include <memory>

namespace srsran {

/// \brief RU controller implementation for the Open Fronthaul interface.
///
/// Manages the timing controller that is common to all sectors and the individual controller of each sector.
class ru_ofh_controller_impl : public ru_controller, public ru_operation_controller
{
public:
  ru_ofh_controller_impl(srslog::basic_logger& logger_) : logger(logger_) {}

  // See the ru_operation_controller interface for documentation.
  void start() override;

  // See the ru_operation_controller interface for documentation.
  void stop() override;

  // See the ru_controller interface for documentation.
  ru_operation_controller& get_operation_controller() override { return *this; }

  // See the ru_controller interface for documentation.
  ru_gain_controller* get_gain_controller() override { return nullptr; }

  // See the ru_controller interface for documentation.
  ru_cfo_controller* get_cfo_controller() override { return nullptr; }

  // See the ru_controller interface for documentation.
  ru_center_frequency_controller* get_center_frequency_controller() override { return nullptr; }

  // See the ru_controller interface for documentation.
  ru_tx_time_offset_controller* get_tx_time_offset_controller() override { return nullptr; }

  /// Sets the sectors controllers.
  void set_sector_controllers(std::vector<ofh::operation_controller*> controllers);

private:
  srslog::basic_logger&                   logger;
  std::vector<ofh::operation_controller*> sector_controllers;
};

} // namespace srsran
