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

namespace srsgnb {

class prach_buffer_context;
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
  /// The request is not processed immediately. The results of the method are asynchronously notified through \ref
  /// prach_processor_notifier interface.
  ///
  /// \param[in] buffer Destination PRACH buffer. It is forwarded to the processor notifier after writing the PRACH
  /// demodulated signal.
  /// \param[in] context PRACH context.
  virtual void handle_request(prach_buffer& buffer, const prach_buffer_context& context) = 0;
};

} // namespace srsgnb