/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_APPS_EXAMPLES_PHY_TIMING_HANDLER_SAMPLE_H
#define SRSGNB_APPS_EXAMPLES_PHY_TIMING_HANDLER_SAMPLE_H

#include "srsgnb/phy/upper/upper_phy_timing_handler.h"
#include <condition_variable>
#include <mutex>

namespace srsgnb {

class timing_handler_sample : public upper_phy_timing_handler
{
private:
  srslog::basic_logger&   logger;
  std::mutex              mutex;
  std::condition_variable cvar_tti_boundary;
  bool                    tti_boundary      = false;
  slot_point              tti_boundary_slot = {};
  bool                    quit              = false;

public:
  timing_handler_sample() : logger(srslog::fetch_basic_logger("TimeHan")) {}

  void set_log_level(std::string log_level) { logger.set_level(srslog::str_to_basic_level(log_level)); }

  void handle_tti_boundary(const upper_phy_timing_context& context) override
  {
    std::unique_lock<std::mutex> lock(mutex);

    // Set logger context.
    logger.set_context(context.slot.system_slot());
    logger.debug("New TTI boundary.");

    // Wait for TTI boundary to be cleared.
    while (tti_boundary && !quit) {
      cvar_tti_boundary.wait_for(lock, std::chrono::milliseconds(1));
    }

    // Raise TTI boundary, save slot point and notify.
    tti_boundary      = true;
    tti_boundary_slot = context.slot;
    cvar_tti_boundary.notify_all();
  }
  void handle_ul_half_slot_boundary(const upper_phy_timing_context& context) override
  {
    logger.debug("UL half slot boundary.");
  }
  void handle_ul_full_slot_boundary(const upper_phy_timing_context& context) override
  {
    logger.debug("UL full slot boundary.");
  }

  slot_point wait_tti_boundary()
  {
    std::unique_lock<std::mutex> lock(mutex);

    // Wait for TTI boundary to be raised.
    while (!tti_boundary && !quit) {
      cvar_tti_boundary.wait_for(lock, std::chrono::milliseconds(1));
    }

    // Clear TTI boundary and notify
    tti_boundary = false;
    cvar_tti_boundary.notify_all();

    // Return the saved slot point.
    return tti_boundary_slot;
  }

  void stop()
  {
    std::unique_lock<std::mutex> lock(mutex);
    quit = true;
    cvar_tti_boundary.notify_all();
  }
};

} // namespace srsgnb

#endif // SRSGNB_APPS_EXAMPLES_PHY_TIMING_HANDLER_SAMPLE_H
