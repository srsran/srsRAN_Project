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

#include "ru_ofh_controller_impl.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

ru_ofh_controller_impl::ru_ofh_controller_impl(srslog::basic_logger&            logger_,
                                               std::unique_ptr<ofh::controller> timing_controller_,
                                               std::vector<ofh::controller*>    sector_controllers_) :
  logger(logger_), timing_controller(std::move(timing_controller_)), sector_controllers(std::move(sector_controllers_))
{
  srsran_assert(timing_controller, "Invalid timing controller");
  srsran_assert(std::all_of(sector_controllers.begin(),
                            sector_controllers.end(),
                            [](const auto& elem) { return elem != nullptr; }),
                "Invalid sector controller");
}

void ru_ofh_controller_impl::start()
{
  logger.info("Starting the Open Fronthaul interface");

  // Start timing controller which is common to all sectors.
  timing_controller->start();

  for (auto* sector : sector_controllers) {
    sector->start();
  }
}

void ru_ofh_controller_impl::stop()
{
  logger.info("Stopping the Open Fronthaul interface");

  // Stop timing controller which is common to all sectors.
  timing_controller->stop();

  for (auto* sector : sector_controllers) {
    sector->stop();
  }
}
