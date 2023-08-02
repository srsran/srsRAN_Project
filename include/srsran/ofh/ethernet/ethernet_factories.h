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
                                          task_executor&        executor,
                                          frame_notifier&       notifier,
                                          srslog::basic_logger& logger);

/// Creates an Ethernet VLAN frame builder.
std::unique_ptr<vlan_frame_builder> create_vlan_frame_builder();

/// Creates an Ethernet VLAN frame decoder.
std::unique_ptr<vlan_frame_decoder> create_vlan_frame_decoder(srslog::basic_logger& logger);

} // namespace ether
} // namespace srsran
