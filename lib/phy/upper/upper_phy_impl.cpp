/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "upper_phy_impl.h"
#include "srsgnb/phy/upper/upper_phy_timing_notifier.h"

using namespace srsgnb;

namespace {

/// Dummy implementation of an upper PHY timing notifier.
class upper_phy_timing_notifier_dummy : public upper_phy_timing_notifier
{
public:
  void on_tti_boundary(slot_point slot) override {}
};

} // namespace

static upper_phy_timing_notifier_dummy notifier_dummy;

upper_phy_impl::upper_phy_impl(unsigned int                             sector_id_,
                               std::unique_ptr<downlink_processor_pool> dl_processor_pool_,
                               std::unique_ptr<resource_grid_pool>      rg_pool_) :
  sector_id(sector_id_),
  dl_processor_pool(std::move(dl_processor_pool_)),
  rg_pool(std::move(rg_pool_)),
  timing_handler((notifier_dummy))
{
  srsran_assert(dl_processor_pool, "Invalid downlink processor pool");
  srsran_assert(rg_pool, "Invalid downlink resource grid pool");

  // :TODO: Add a logger here.
  (void)sector_id;
}

upper_phy_rx_symbol_handler& upper_phy_impl::get_upper_phy_rx_symbol_handler()
{
  return symbol_handler;
}

upper_phy_timing_handler& upper_phy_impl::get_upper_phy_timing_handler()
{
  return timing_handler;
}

downlink_processor_pool& upper_phy_impl::get_downlink_processor_pool()
{
  return *dl_processor_pool;
}

resource_grid_pool& upper_phy_impl::get_downlink_resource_grid_pool()
{
  return *rg_pool;
}

void upper_phy_impl::set_upper_phy_notifier(upper_phy_timing_notifier& notifier)
{
  timing_handler.set_upper_phy_notifier(notifier);
}
