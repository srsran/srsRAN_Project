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

namespace srsran {

struct resource_grid_context;
class shared_resource_grid;

namespace ofh {

class error_notifier;

/// Open Fronthaul downlink handler.
class downlink_handler
{
public:
  /// Default destructor.
  virtual ~downlink_handler() = default;

  /// \brief Handles the given downlink data to be transmitted.
  ///
  /// \param[in] context Resource grid context.
  /// \param[in] grid Downlink data to transmit.
  virtual void handle_dl_data(const resource_grid_context& context, const shared_resource_grid& grid) = 0;
};

} // namespace ofh
} // namespace srsran
