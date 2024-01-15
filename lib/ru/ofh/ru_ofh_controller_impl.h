/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
class ru_ofh_controller_impl : public ru_controller
{
public:
  ru_ofh_controller_impl(srslog::basic_logger& logger_, std::vector<ofh::controller*> sector_controllers_);

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  srslog::basic_logger&         logger;
  std::vector<ofh::controller*> sector_controllers;
};

} // namespace srsran
