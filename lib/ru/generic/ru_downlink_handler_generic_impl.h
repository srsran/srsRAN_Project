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

class lower_phy_sector;
class lower_phy_rg_handler;

/// Radio Unit downlink handler generic implementation.
class ru_downlink_handler_generic_impl : public ru_downlink_plane_handler
{
public:
  explicit ru_downlink_handler_generic_impl(std::vector<lower_phy_sector*> handler_) : handler(std::move(handler_)) {}

  // See interface for documentation.
  void handle_dl_data(const resource_grid_context& context, const shared_resource_grid& grid) override;

private:
  std::vector<lower_phy_sector*> handler;
};

} // namespace srsran
