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

#include "srsran/ru/ru_uplink_plane.h"
#include <vector>

namespace srsran {

class lower_phy_sector;
class lower_phy_request_handler;
class shared_resource_grid;

/// Radio Unit uplink request handler generic implementation.
class ru_uplink_request_handler_generic_impl : public ru_uplink_plane_handler
{
public:
  explicit ru_uplink_request_handler_generic_impl(std::vector<lower_phy_sector*> low_phy_handler_) :
    low_phy_handler(std::move(low_phy_handler_))
  {
  }

  // See interface for documentation.
  void handle_prach_occasion(const prach_buffer_context& context, prach_buffer& buffer) override;

  // See interface for documentation.
  void handle_new_uplink_slot(const resource_grid_context& context, const shared_resource_grid& grid) override;

private:
  std::vector<lower_phy_sector*> low_phy_handler;
};

} // namespace srsran
