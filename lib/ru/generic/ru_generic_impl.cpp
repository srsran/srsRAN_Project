/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_generic_impl.h"

using namespace srsran;

ru_generic_impl::ru_generic_impl(const ru_generic_impl_config& config, ru_generic_impl_dependencies&& dependencies) :
  are_metrics_enabled(config.are_metrics_enabled),
  error_adapter(dependencies.logger, dependencies.error_notifier),
  rx_adapter(dependencies.rx_symbol_handler),
  timing_adapter(dependencies.timing_handler, std::move(dependencies.radio_event_counter)),
  radio(std::move(dependencies.radio)),
  metrics_collector({}),
  ru_ctrl(*radio, config.srate_MHz),
  ru_downlink_hdlr({}),
  ru_uplink_request_hdlr({})
{
}

ru_controller& ru_generic_impl::get_controller()
{
  return ru_ctrl;
}

ru_downlink_plane_handler& ru_generic_impl::get_downlink_plane_handler()
{
  return ru_downlink_hdlr;
}

ru_uplink_plane_handler& ru_generic_impl::get_uplink_plane_handler()
{
  return ru_uplink_request_hdlr;
}

void ru_generic_impl::set_lower_phy_sectors(std::vector<std::unique_ptr<lower_phy_sector>> sectors)
{
  phy_sectors = std::move(sectors);

  srsran_assert(!phy_sectors.empty(), "Invalid Radio Unit sectors");

  std::vector<lower_phy_sector*> sector_ptrs;
  for (auto& phy_sector : phy_sectors) {
    sector_ptrs.push_back(phy_sector.get());
  }

  ru_ctrl.set_lower_phy_sectors(sector_ptrs);

  metrics_collector      = ru_metrics_collector_generic_impl(sector_ptrs);
  ru_downlink_hdlr       = ru_downlink_handler_generic_impl(sector_ptrs);
  ru_uplink_request_hdlr = ru_uplink_request_handler_generic_impl(sector_ptrs);
}
