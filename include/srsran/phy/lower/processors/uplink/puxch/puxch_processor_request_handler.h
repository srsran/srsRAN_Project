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

namespace srsran {

struct resource_grid_context;
class resource_grid;

/// \brief Lower physical layer PUxCH processor - Request handler interface.
///
/// Provides an interface for requesting the capture and demodulation of PUxCH occasions.
class puxch_processor_request_handler
{
public:
  /// Default destructor.
  virtual ~puxch_processor_request_handler() = default;

  /// \brief Handles a new PUxCH request.
  ///
  /// The resource grid and context provided to this method are forwarded to a dedicated block that takes care of the
  /// request asynchronously. A notification through the \ref puxch_processor_notifier interface will inform the client
  /// that the request has been processed and that the PUxCH demodulated sequence is available in \c grid.
  ///
  /// \param[in] grid Destination uplink resource grid.
  /// \param[in] context Resource grid context.
  virtual void handle_request(resource_grid& grid, const resource_grid_context& context) = 0;
};

} // namespace srsran