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
class resource_grid_reader;

/// \brief Lower physical layer downlink processor - Request handler interface.
///
/// This class provides an interface for requesting the modulation of a downlink resource grid.
class pdxch_processor_request_handler
{
public:
  /// Default destructor.
  virtual ~pdxch_processor_request_handler() = default;

  /// \brief Handles a new downlink resource grid transmission request.
  ///
  /// The downlink resource grid transmission and context provided to this method are forwarded to a dedicated block
  /// that takes care of the request asynchronously. A notification through the \ref downlink_processor_notifier
  /// interface will inform the client if the request has not been processed.
  ///
  /// \param[in] grid    Resource grid to transmit.
  /// \param[in] context Resource grid transmission context.
  virtual void handle_request(const resource_grid_reader& grid, const resource_grid_context& context) = 0;
};

} // namespace srsran