/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/ofh/ethernet/ethernet_frame_builder.h"
#include "srsran/ofh/ethernet/ethernet_receiver.h"
#include "srsran/ofh/ethernet/ethernet_receiver_config.h"
#include "srsran/ofh/ethernet/ethernet_transmitter.h"
#include "srsran/ofh/ethernet/ethernet_transmitter_config.h"
#include "srsran/ofh/ethernet/vlan_ethernet_frame_decoder.h"
#include "srsran/srslog/logger.h"
#include <memory>

namespace srsran {

class task_executor;

namespace ether {

class frame_notifier;

/// Creates an Ethernet transmitter.
std::unique_ptr<transmitter> create_transmitter(const transmitter_config& config, srslog::basic_logger& logger);

/// Creates an Ethernet receiver.
std::unique_ptr<receiver>
create_receiver(const receiver_config& config, task_executor& executor, srslog::basic_logger& logger);

/// Creates an Ethernet frame builder with VLAN tag insertion.
std::unique_ptr<frame_builder> create_vlan_frame_builder(const vlan_frame_params& eth_params);

/// Creates an Ethernet frame builder without VLAN tag insertion.
std::unique_ptr<frame_builder> create_frame_builder(const vlan_frame_params& eth_params);

/// Creates an Ethernet VLAN frame decoder.
std::unique_ptr<vlan_frame_decoder> create_vlan_frame_decoder(srslog::basic_logger& logger, unsigned sector_id);

} // namespace ether
} // namespace srsran
