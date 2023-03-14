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

struct prach_buffer_context;
class prach_buffer;

/// \brief Lower physical layer PRACH processor - Request handler interface.
///
/// This class provides an interface for requesting the capture and demodulation of PRACH occasions.
class prach_processor_request_handler
{
public:
  /// Default destructor.
  virtual ~prach_processor_request_handler() = default;

  /// \brief Handles a new PRACH request.
  ///
  /// The PRACH buffer and context provided to this method are forwarded to a dedicated block that takes care of the
  /// request asynchronously. A notification through the \ref prach_processor_notifier interface will inform the client
  /// that the request has been processed and that the PRACH demodulated sequence is available in \c buffer.
  ///
  /// \param[in] buffer Destination PRACH buffer.
  /// \param[in] context PRACH context.
  virtual void handle_request(prach_buffer& buffer, const prach_buffer_context& context) = 0;
};

} // namespace srsran