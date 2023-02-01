/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

struct resource_grid_context;
class resource_grid_reader;

/// \brief Radio Unit downlink plane interface.
///
/// Downlink plane interface of a radio unit used to transmit downlink data.
class ru_downlink_plane
{
public:
  virtual ~ru_downlink_plane() = default;

  /// \brief Handles the given downlink data to be transmitted.
  ///
  /// \param[in] context Resource grid context.
  /// \param[in] grid Downlink data to transmit.
  virtual void handle_dl_data(const resource_grid_context& context, const resource_grid_reader& grid) = 0;
};

} // namespace srsgnb