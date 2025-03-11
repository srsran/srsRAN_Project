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
  virtual void handle_request(const shared_resource_grid& grid, const resource_grid_context& context) = 0;
};

} // namespace srsran
