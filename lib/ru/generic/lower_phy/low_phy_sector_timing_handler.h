/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/lower/lower_phy_timing_notifier.h"

namespace srsran {

class low_phy_sector_timing_handler : public lower_phy_timing_notifier
{
  lower_phy_timing_notifier* notifier = nullptr;

public:
  explicit low_phy_sector_timing_handler(lower_phy_timing_notifier* notifier_);

  // See interface for documentation.
  void on_tti_boundary(const lower_phy_timing_context& context) override;

  // See interface for documentation.
  void on_ul_half_slot_boundary(const lower_phy_timing_context& context) override;

  // See interface for documentation.
  void on_ul_full_slot_boundary(const lower_phy_timing_context& context) override;
};

} // namespace srsran
