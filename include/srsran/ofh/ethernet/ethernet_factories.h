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

#include "srsran/ofh/ethernet/ethernet_gateway.h"
#include "srsran/ofh/ethernet/ethernet_gw_config.h"
#include "srsran/ofh/ethernet/ethernet_receiver.h"
#include "srsran/ofh/ethernet/vlan_ethernet_frame_builder.h"
#include "srsran/ofh/ethernet/vlan_ethernet_frame_decoder.h"
#include "srsran/srslog/logger.h"
#include <memory>

namespace srsran {

class task_executor;

namespace ether {

class frame_notifier;

/// Creates an Ethernet gateway.
std::unique_ptr<gateway> create_gateway(const gw_config& config, srslog::basic_logger& logger);

/// Creates an Ethernet receiver.
std::unique_ptr<receiver> create_receiver(const std::string&    interface,
                                          bool                  is_promiscuous_mode_enabled,
                                          task_executor&        executor,
                                          srslog::basic_logger& logger);

/// Creates an Ethernet VLAN frame builder.
std::unique_ptr<vlan_frame_builder> create_vlan_frame_builder();

/// Creates an Ethernet VLAN frame decoder.
std::unique_ptr<vlan_frame_decoder> create_vlan_frame_decoder(srslog::basic_logger& logger);

} // namespace ether
} // namespace srsran
