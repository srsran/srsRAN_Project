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

#include "srsran/ran/slot_point.h"

namespace srsran {

struct resource_grid_context;
class shared_resource_grid;

/// \brief Radio Unit resource grid handler interface.
///
/// The resource grid handler modulates and transmits the given downlink resource grid through the underlying radio
/// device.
class ru_downlink_plane_handler
{
public:
  /// Default destructor.
  virtual ~ru_downlink_plane_handler() = default;

  /// \brief Handles the given downlink data to be transmitted.
  ///
  /// \param[in] context Resource grid context.
  /// \param[in] grid Downlink data to transmit.
  virtual void handle_dl_data(const resource_grid_context& context, const shared_resource_grid& grid) = 0;
};

} // namespace srsran
