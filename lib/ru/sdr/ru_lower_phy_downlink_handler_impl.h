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

#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/ru/ru_downlink_plane.h"

namespace srsran {

class lower_phy_downlink_handler;

/// Radio Unit to lower physical layer downlink handler implementation.
class ru_lower_phy_downlink_handler_impl : public ru_downlink_plane_handler
{
public:
  explicit ru_lower_phy_downlink_handler_impl(std::vector<lower_phy_downlink_handler*> handlers_) :
    handlers(std::move(handlers_))
  {
  }

  // See interface for documentation.
  void handle_dl_data(const resource_grid_context& context, const shared_resource_grid& grid) override;

private:
  std::vector<lower_phy_downlink_handler*> handlers;
};

} // namespace srsran
