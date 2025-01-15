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
