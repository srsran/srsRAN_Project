/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "../support/uplink_notified_grid_symbol_repository.h"
#include "srsran/ofh/ofh_uplane_rx_symbol_notifier.h"
#include <memory>

namespace srsran {
namespace ofh {

/// \brief Received symbol reorderer.
///
/// This object is a decorator that receives symbols out of order and notifies them to the configured notifier in
/// ascending order.
class rx_symbol_reorderer : public uplane_rx_symbol_notifier
{
  uplane_rx_symbol_notifier&                              notifier;
  std::shared_ptr<uplink_notified_grid_symbol_repository> uplink_symbol_repo;

public:
  rx_symbol_reorderer(uplane_rx_symbol_notifier&                              notifier_,
                      std::shared_ptr<uplink_notified_grid_symbol_repository> uplink_symbol_repo_) :
    notifier(notifier_), uplink_symbol_repo(uplink_symbol_repo_)
  {
    srsran_assert(uplink_symbol_repo, "Invalid notified uplink grid symbol repository");
  }

  // See interface for documentation.
  void on_new_uplink_symbol(const uplane_rx_symbol_context& context, shared_resource_grid grid, bool is_valid) override;

  // See interface for documentation.
  void on_new_prach_window_data(const prach_buffer_context& context, const prach_buffer& buffer) override
  {
    // PRACH notifies all symbols at once, no need to reorder.
    notifier.on_new_prach_window_data(context, buffer);
  }
};

} // namespace ofh
} // namespace srsran
