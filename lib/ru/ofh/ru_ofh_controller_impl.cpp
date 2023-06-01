/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
