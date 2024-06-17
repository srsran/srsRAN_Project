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

#include "srsran/adt/byte_buffer.h"

namespace srsran {

/// Unidirectional notifier of SDUs for a given SCTP association.
class sctp_association_sdu_notifier
{
public:
  virtual ~sctp_association_sdu_notifier() = default;

  /// Forward a SDU.
  /// \return True if the SDU was successfully forwarded, false otherwise.
  virtual bool on_new_sdu(byte_buffer pdu) = 0;
};

class sctp_network_gateway_info
{
public:
  virtual ~sctp_network_gateway_info() = default;

  /// \brief Retrieve the SCTP network node opened socket file descriptor.
  virtual int get_socket_fd() const = 0;
};

/// SCTP network client interface, which can be used to initiate new SCTP associations to a remote SCTP server.
class sctp_network_client : public sctp_network_gateway_info
{
public:
  virtual ~sctp_network_client() = default;

  /// \brief Connect to remote SCTP server.
  [[nodiscard]] virtual std::unique_ptr<sctp_association_sdu_notifier>
  connect_to(const std::string&                             dest_name,
             const std::string&                             dest_addr,
             int                                            dest_port,
             std::unique_ptr<sctp_association_sdu_notifier> recv_handler) = 0;
};

} // namespace srsran