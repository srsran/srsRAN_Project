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

#include "sctp_network_gateway.h"
#include <memory>
#include <utility>

namespace srsran {

struct sctp_network_gateway_creation_message {
  sctp_network_gateway_creation_message(sctp_network_gateway_config            config_,
                                        sctp_network_gateway_control_notifier& ctrl_notifier_,
                                        network_gateway_data_notifier&         data_notifier_) :
    config(std::move(config_)), ctrl_notifier(ctrl_notifier_), data_notifier(data_notifier_)
  {
  }
  sctp_network_gateway_config            config;
  sctp_network_gateway_control_notifier& ctrl_notifier;
  network_gateway_data_notifier&         data_notifier;
};

/// Creates an instance of an network gateway
std::unique_ptr<sctp_network_gateway> create_sctp_network_gateway(sctp_network_gateway_creation_message msg);

} // namespace srsran
