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

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  ru_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  ru_gain_controller* get_gain_controller() override { return nullptr; }

  // See interface for documentation.
  ru_cfo_controller* get_cfo_controller() override { return nullptr; }

  /// Sets the sectors controllers.
  void set_sector_controllers(std::vector<ofh::controller*> controllers);

private:
  srslog::basic_logger&         logger;
  std::vector<ofh::controller*> sector_controllers;
};

} // namespace srsran
