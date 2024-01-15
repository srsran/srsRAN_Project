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

#pragma once

#include "srsran/phy/lower/lower_phy_timing_context.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_notifier.h"

namespace srsran {

class downlink_processor_notifier_spy : public downlink_processor_notifier
{
public:
  void on_tti_boundary(const lower_phy_timing_context& context) override { tti_boundaries.emplace_back(context); }

  const std::vector<lower_phy_timing_context>& get_tti_boundaries() const { return tti_boundaries; }

  unsigned get_total_count() const { return tti_boundaries.size(); }

  void clear_notifications() { tti_boundaries.clear(); }

private:
  std::vector<lower_phy_timing_context> tti_boundaries;
};

} // namespace srsran