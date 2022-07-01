
#pragma once

#include "srsgnb/phy/lower/lower_phy_timing_notifier.h"
#include "srsgnb/phy/upper/upper_phy_timing_handler.h"

namespace srsgnb {

/// Implements a generic physical layer timing adapter.
class phy_timing_adapter : public lower_phy_timing_notifier
{
private:
  upper_phy_timing_handler* timing_handler;

public:
  /// Connects the adaptor to the upper physical layer handler.
  void connect(upper_phy_timing_handler* upper_handler) { timing_handler = upper_handler; }

  // See interface for documentation.
  void on_tti_boundary(const lower_phy_timing_context& context) override
  {
    srsran_always_assert(timing_handler, "Adapter is not connected.");
    upper_phy_timing_context upper_context;
    upper_context.slot = context.slot;
    timing_handler->handle_tti_boundary(upper_context);
  }

  // See interface for documentation.
  void on_ul_half_slot_boundary(const lower_phy_timing_context& context) override
  {
    srsran_always_assert(timing_handler, "Adapter is not connected.");
    upper_phy_timing_context upper_context;
    upper_context.slot = context.slot;
    timing_handler->handle_ul_half_slot_boundary(upper_context);
  }

  // See interface for documentation.
  void on_ul_full_slot_boundary(const lower_phy_timing_context& context) override
  {
    srsran_always_assert(timing_handler, "Adapter is not connected.");
    upper_phy_timing_context upper_context;
    upper_context.slot = context.slot;
    timing_handler->handle_ul_full_slot_boundary(upper_context);
  }

  // See interface for documentation.
  void on_late_resource_grid(const late_resource_grid_context& context) override
  {
    fmt::print("Unavailable data to transmit for sector {}, slot {} and symbol {}.",
               context.sector,
               context.slot,
               context.symbol);
  }
};

} // namespace srsgnb
