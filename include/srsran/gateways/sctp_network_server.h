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

#include "srsran/gateways/sctp_network_client.h"
#include "srsran/support/io/transport_layer_address.h"

namespace srsran {

/// Factory of new SCTP association handlers.
class sctp_network_association_factory
{
public:
  virtual ~sctp_network_association_factory() = default;

  /// Called on every SCTP association notification, to create a new SCTP association handler.
  virtual std::unique_ptr<sctp_association_sdu_notifier>
  create(std::unique_ptr<sctp_association_sdu_notifier> sctp_send_notifier) = 0;
};

/// SCTP network server interface, which will handle requests to start new SCTP associations.
class sctp_network_server : public sctp_network_gateway_info
{
public:
  virtual ~sctp_network_server() = default;

  /// \brief Start listening for new SCTP associations.
  virtual bool listen() = 0;

  /// \brief Get port to which server binded and is listening for connections.
  virtual std::optional<uint16_t> get_listen_port() = 0;
};

} // namespace srsran
