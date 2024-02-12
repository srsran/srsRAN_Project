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

#include "ru_ofh_controller_impl.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

ru_ofh_controller_impl::ru_ofh_controller_impl(srslog::basic_logger&         logger_,
                                               std::vector<ofh::controller*> sector_controllers_) :
  logger(logger_), sector_controllers(std::move(sector_controllers_))
{
  srsran_assert(std::all_of(sector_controllers.begin(),
                            sector_controllers.end(),
                            [](const auto& elem) { return elem != nullptr; }),
                "Invalid sector controller");
}

void ru_ofh_controller_impl::start()
{
  logger.info("Starting the operation of the Open Fronthaul interface");
  for (auto* sector : sector_controllers) {
    sector->start();
  }
  logger.info("Started the operation of the Open Fronthaul interface");
}

void ru_ofh_controller_impl::stop()
{
  logger.info("Stopping the operation of the Open Fronthaul interface");
  for (auto* sector : sector_controllers) {
    sector->stop();
  }
  logger.info("Stopped the operation of the Open Fronthaul interface");
}
