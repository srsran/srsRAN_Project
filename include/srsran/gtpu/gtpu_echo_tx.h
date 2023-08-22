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

struct sockaddr_storage;

namespace srsran {

/// This interface represents the data entry point of the transmitting side of a GTP-U echo.
/// The other-layers will use this interface to trigger echo requests or to reply with echo responses.
class gtpu_echo_tx_interface
{
public:
  gtpu_echo_tx_interface()                                          = default;
  virtual ~gtpu_echo_tx_interface()                                 = default;
  gtpu_echo_tx_interface(const gtpu_echo_tx_interface&)             = delete;
  gtpu_echo_tx_interface& operator=(const gtpu_echo_tx_interface&)  = delete;
  gtpu_echo_tx_interface(const gtpu_echo_tx_interface&&)            = delete;
  gtpu_echo_tx_interface& operator=(const gtpu_echo_tx_interface&&) = delete;

  virtual void send_echo_request(const sockaddr_storage& dst_addr)    = 0;
  virtual void send_echo_response(const sockaddr_storage& dst_addr)   = 0;
  virtual void handle_echo_response(const sockaddr_storage& src_addr) = 0;
};

} // namespace srsran
