/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "low_phy_sector_timing_handler.h"

using namespace srsran;

namespace {

/// Low PHY sector timing notifier dummy implementation.
class low_phy_sector_timing_notifier_dummy : public lower_phy_timing_notifier
{
public:
  // See interface for documentation.
  void on_tti_boundary(const lower_phy_timing_context& context) override {}

  // See interface for documentation.
  void on_ul_half_slot_boundary(const lower_phy_timing_context& context) override {}

  // See interface for documentation.
  void on_ul_full_slot_boundary(const lower_phy_timing_context& context) override {}
};

} // namespace

static low_phy_sector_timing_notifier_dummy dummy_notifier;

low_phy_sector_timing_handler::low_phy_sector_timing_handler(lower_phy_timing_notifier* notifier_) :
  notifier(notifier_ ? notifier_ : &dummy_notifier)
{
}

void low_phy_sector_timing_handler::on_tti_boundary(const lower_phy_timing_context& context)
{
  notifier->on_tti_boundary(context);
}

void low_phy_sector_timing_handler::on_ul_half_slot_boundary(const lower_phy_timing_context& context)
{
  notifier->on_ul_half_slot_boundary(context);
}

void low_phy_sector_timing_handler::on_ul_full_slot_boundary(const lower_phy_timing_context& context)
{
  notifier->on_ul_full_slot_boundary(context);
}
