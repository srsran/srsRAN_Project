/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ru/ru_uplink_plane.h"
#include <algorithm>
#include <vector>

namespace srsran {

class task_executor;
class shared_resource_grid;

namespace ofh {
class uplink_request_handler;
} // namespace ofh

/// This proxy implementation dispatches the requests to the corresponding OFH sector.
class ru_uplink_plane_handler_proxy : public ru_uplink_plane_handler
{
public:
  explicit ru_uplink_plane_handler_proxy(std::vector<ofh::uplink_request_handler*> sectors_) :
    sectors(std::move(sectors_))
  {
    srsran_assert(std::all_of(sectors.begin(), sectors.end(), [](const auto& elem) { return elem != nullptr; }),
                  "Invalid sector");
  }

  // See interface for documentation.
  void handle_prach_occasion(const prach_buffer_context& context, prach_buffer& buffer) override;

  // See interface for documentation.
  void handle_new_uplink_slot(const resource_grid_context& context, const shared_resource_grid& grid) override;

private:
  std::vector<ofh::uplink_request_handler*> sectors;
};

} // namespace srsran
