
#ifndef SAMPLE_APPS_PHY_LOWER_PHY_TIMING_HANDLER_SAMPLE_H
#define SAMPLE_APPS_PHY_LOWER_PHY_TIMING_HANDLER_SAMPLE_H

#include "srsgnb/phy/lower/lower_phy.h"
#include <condition_variable>
#include <mutex>

namespace srsgnb {

class lower_phy_timing_handler_sample : public lower_phy_timing_handler
{
private:
  srslog::basic_logger&   logger;
  std::mutex              mutex;
  std::condition_variable cvar_tti_boundary;
  bool                    tti_boundary = false;
  slot_point              tti_boundary_slot;
  bool                    quit = false;

public:
  lower_phy_timing_handler_sample(std::string log_level = "none") : logger(srslog::fetch_basic_logger("TimeHan"))
  {
    logger.set_level(srslog::str_to_basic_level(log_level));
  }

  void notify_tti_boundary(const lower_phy_timing_context_t& context) override
  {
    std::unique_lock<std::mutex> lock(mutex);

    // Set logger context.
    logger.set_context(context.slot.system_slot());
    logger.debug("New TTI boundary.");

    // Wait for TTI boundary to be cleared.
    while (tti_boundary && !quit) {
      cvar_tti_boundary.wait(lock);
    }

    // Raise TTI boundary, save slot point and notify.
    tti_boundary      = true;
    tti_boundary_slot = context.slot;
    cvar_tti_boundary.notify_all();
  }
  void notify_ul_half_slot_boundary(const lower_phy_timing_context_t& context) override
  {
    logger.debug("UL half slot boundary.");
  }
  void notify_ul_full_slot_boundary(const lower_phy_timing_context_t& context) override
  {
    logger.debug("UL full slot boundary.");
  }

  slot_point wait_tti_boundary()
  {
    std::unique_lock<std::mutex> lock(mutex);

    // Wait for TTI boundary to be raised.
    while (!tti_boundary && !quit) {
      cvar_tti_boundary.wait(lock);
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

#endif // SAMPLE_APPS_PHY_LOWER_PHY_TIMING_HANDLER_SAMPLE_H
