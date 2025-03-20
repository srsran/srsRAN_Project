/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_ofh_controller_impl.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

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

void ru_ofh_controller_impl::set_sector_controllers(std::vector<ofh::operation_controller*> controllers)
{
  srsran_assert(!controllers.empty(), "Invalid sector controllers");

  sector_controllers = std::move(controllers);

  srsran_assert(std::all_of(sector_controllers.begin(),
                            sector_controllers.end(),
                            [](const auto& elem) { return elem != nullptr; }),
                "Invalid sector controller");
}
