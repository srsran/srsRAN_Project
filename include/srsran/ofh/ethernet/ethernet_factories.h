/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ofh/ethernet/ethernet_frame_builder.h"
#include "srsran/ofh/ethernet/ethernet_gateway.h"
#include "srsran/ofh/ethernet/ethernet_gateway_config.h"
#include "srsran/ofh/ethernet/ethernet_receiver.h"
#include "srsran/ofh/ethernet/ethernet_receiver_config.h"
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
