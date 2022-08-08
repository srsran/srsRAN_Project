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

upper_phy_impl::upper_phy_impl(unsigned                                 sector_id_,
                               std::unique_ptr<downlink_processor_pool> dl_processor_pool_,
                               std::unique_ptr<resource_grid_pool>      dl_rg_pool_,
                               std::unique_ptr<uplink_processor_pool>   ul_processor_pool_,
                               std::unique_ptr<prach_buffer_pool>       prach_pool_,
                               upper_phy_rx_results_notifier_proxy&&    notifier_proxy,
                               upper_phy_rx_symbol_request_notifier&    symbol_request_notifier) :
  sector_id(sector_id_),
  dl_processor_pool(std::move(dl_processor_pool_)),
  dl_rg_pool(std::move(dl_rg_pool_)),
  ul_processor_pool(std::move(ul_processor_pool_)),
  prach_pool(std::move(prach_pool_)),
  symbol_handler(*ul_processor_pool),
  results_notifier_proxy(std::move(notifier_proxy)),
  ul_request_processor(symbol_request_notifier, *prach_pool),
  timing_handler(notifier_dummy)
{
  srsgnb_assert(dl_processor_pool, "Invalid downlink processor pool");
  srsgnb_assert(dl_rg_pool, "Invalid downlink resource grid pool");
  srsgnb_assert(ul_processor_pool, "Invalid uplink processor pool");
  srsgnb_assert(prach_pool, "Invalid PRACH buffer pool");

  // :TODO: Add a logger here.
  (void)sector_id;
}

upper_phy_rx_symbol_handler& upper_phy_impl::get_rx_symbol_handler()
{
  return symbol_handler;
}

upper_phy_timing_handler& upper_phy_impl::get_timing_handler()
{
  return timing_handler;
}

downlink_processor_pool& upper_phy_impl::get_downlink_processor_pool()
{
  return *dl_processor_pool;
}

resource_grid_pool& upper_phy_impl::get_downlink_resource_grid_pool()
{
  return *dl_rg_pool;
}

uplink_request_processor& upper_phy_impl::get_uplink_request_processor()
{
  return ul_request_processor;
}

void upper_phy_impl::set_timing_notifier(srsgnb::upper_phy_timing_notifier& notifier)
{
  timing_handler.set_upper_phy_notifier(notifier);
}

void upper_phy_impl::set_results_notifier(upper_phy_rx_results_notifier& notifier)
{
  results_notifier_proxy.connect(notifier);
}
