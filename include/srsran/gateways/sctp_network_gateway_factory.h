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
