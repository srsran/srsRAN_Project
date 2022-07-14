/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

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
};

} // namespace srsgnb
