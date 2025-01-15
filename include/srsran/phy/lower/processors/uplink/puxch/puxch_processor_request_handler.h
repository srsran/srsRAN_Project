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
  virtual void handle_request(const shared_resource_grid& grid, const resource_grid_context& context) = 0;
};

} // namespace srsran