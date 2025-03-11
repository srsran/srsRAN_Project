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

#include "srsran/phy/lower/lower_phy_timing_context.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_notifier.h"

namespace srsran {

class downlink_processor_notifier_spy : public downlink_processor_notifier
{
public:
  void on_tti_boundary(const lower_phy_timing_context& context) override { tti_boundaries.emplace_back(context); }

  void on_new_metrics(const lower_phy_baseband_metrics& metrics) override {}

  const std::vector<lower_phy_timing_context>& get_tti_boundaries() const { return tti_boundaries; }

  unsigned get_total_count() const { return tti_boundaries.size(); }

  void clear_notifications() { tti_boundaries.clear(); }

private:
  std::vector<lower_phy_timing_context> tti_boundaries;
};

} // namespace srsran
