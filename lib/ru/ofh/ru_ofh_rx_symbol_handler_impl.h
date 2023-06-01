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

#include "srsran/ofh/ofh_uplane_rx_symbol_notifier.h"
#include "srsran/ru/ru_uplink_plane.h"

namespace srsran {

/// RU received symbol handler for the Open Fronthaul interface.
class ru_ofh_rx_symbol_handler_impl : public ofh::uplane_rx_symbol_notifier
{
public:
  explicit ru_ofh_rx_symbol_handler_impl(ru_uplink_plane_rx_symbol_notifier& notifier_) : notifier(notifier_) {}

  // See interface for documentation.
  void on_new_uplink_symbol(const ofh::uplane_rx_symbol_context& context, const resource_grid_reader& grid) override;

  // See interface for documentation.
  void on_new_prach_window_data(const prach_buffer_context& context, const prach_buffer& buffer) override;

private:
  ru_uplink_plane_rx_symbol_notifier& notifier;
};

} // namespace srsran
