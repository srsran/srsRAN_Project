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

/// Lower physical layer PRACH processor - Request handler interface.
class prach_processor_request_handler
{
public:
  /// Default destructor.
  virtual ~prach_processor_request_handler() = default;

  /// \brief Handles a new PRACH request.
  /// \param[in] buffer  PRACH Buffer.
  /// \param[in] context PRACH context.
  virtual void handle_request(prach_buffer& buffer, const prach_buffer_context& context) = 0;
};

} // namespace srsgnb