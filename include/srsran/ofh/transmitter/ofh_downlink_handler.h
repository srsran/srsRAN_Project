/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {
struct resource_grid_context;
class resource_grid_reader;

namespace ofh {

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
  virtual void handle_dl_data(const resource_grid_context& context, const resource_grid_reader& grid) = 0;
};

} // namespace ofh
} // namespace srsran