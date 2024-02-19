/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "ofh_downlink_handler_broadcast_impl.h"
#include "ofh_downlink_manager.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul downlink manager broadcast implementation.
class downlink_manager_broadcast_impl : public downlink_manager
{
  downlink_handler_broadcast_impl handler;

public:
  downlink_manager_broadcast_impl(const downlink_handler_broadcast_impl_config&  config,
                                  downlink_handler_broadcast_impl_dependencies&& dependencies) :
    handler(config, std::move(dependencies))
  {
  }

  // See interface for documentation.
  downlink_handler& get_downlink_handler() override;

  // See interface for documentation.
  ota_symbol_boundary_notifier& get_ota_symbol_boundary_notifier() override;

  // See interface for documentation.
  void set_error_notifier(error_notifier& notifier) override;
};

} // namespace ofh
} // namespace srsran
