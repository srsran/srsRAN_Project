/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lower_phy_sector_timing_handler.h"

using namespace srsran;

namespace {

/// Lower PHY sector timing notifier dummy implementation.
class lower_phy_sector_timing_notifier_dummy : public lower_phy_timing_notifier
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

static lower_phy_sector_timing_notifier_dummy dummy_notifier;

lower_phy_sector_timing_handler::lower_phy_sector_timing_handler(lower_phy_timing_notifier* notifier_) :
  notifier(notifier_ ? notifier_ : &dummy_notifier)
{
}

void lower_phy_sector_timing_handler::on_tti_boundary(const lower_phy_timing_context& context)
{
  notifier->on_tti_boundary(context);
}

void lower_phy_sector_timing_handler::on_ul_half_slot_boundary(const lower_phy_timing_context& context)
{
  notifier->on_ul_half_slot_boundary(context);
}

void lower_phy_sector_timing_handler::on_ul_full_slot_boundary(const lower_phy_timing_context& context)
{
  notifier->on_ul_full_slot_boundary(context);
}
