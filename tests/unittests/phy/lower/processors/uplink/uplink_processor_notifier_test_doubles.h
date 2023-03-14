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

#include "srsran/phy/lower/lower_phy_timing_context.h"
#include "srsran/phy/lower/processors/uplink/uplink_processor_notifier.h"

namespace srsran {

class uplink_processor_notifier_spy : public uplink_processor_notifier
{
public:
  void on_half_slot(const lower_phy_timing_context& context) override { half_slots.emplace_back(context); }

  void on_full_slot(const lower_phy_timing_context& context) override { full_slots.emplace_back(context); }

  const std::vector<lower_phy_timing_context>& get_half_slots() const { return half_slots; }

  const std::vector<lower_phy_timing_context>& get_full_slots() const { return full_slots; }

  void clear_notifications()
  {
    half_slots.clear();
    full_slots.clear();
  }

private:
  std::vector<lower_phy_timing_context> half_slots;
  std::vector<lower_phy_timing_context> full_slots;
};

} // namespace srsran