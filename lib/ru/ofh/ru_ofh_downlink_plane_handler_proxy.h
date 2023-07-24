/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/ru/ru_downlink_plane.h"

namespace srsran {

class task_executor;

namespace ofh {
class downlink_handler;
}

/// This proxy implementation dispatches the requests to the corresponding OFH sector.
class ru_downlink_plane_handler_proxy : public ru_downlink_plane_handler
{
public:
  explicit ru_downlink_plane_handler_proxy(std::vector<ofh::downlink_handler*> sectors_) : sectors(std::move(sectors_))
  {
    srsran_assert(std::all_of(sectors.begin(), sectors.end(), [](const auto& elem) { return elem != nullptr; }),
                  "Invalid sector");
  }

  // See interface for documentation.
  void handle_dl_data(const resource_grid_context& context, const resource_grid_reader& grid) override;

private:
  std::vector<ofh::downlink_handler*> sectors;
};

} // namespace srsran
