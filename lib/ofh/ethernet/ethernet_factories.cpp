/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ofh/ethernet/ethernet_factories.h"
#include "ethernet_frame_builder_impl.h"
#include "ethernet_receiver_impl.h"
#include "ethernet_transmitter_impl.h"
#include "vlan_ethernet_frame_builder_impl.h"
#include "vlan_ethernet_frame_decoder_impl.h"

using namespace srsran;
using namespace ether;

std::unique_ptr<gateway> srsran::ether::create_gateway(const gw_config& config, srslog::basic_logger& logger)
{
  return std::make_unique<transmitter_impl>(config, logger);
}

std::unique_ptr<receiver>
srsran::ether::create_receiver(const receiver_config& config, task_executor& executor, srslog::basic_logger& logger)
{
  return std::make_unique<receiver_impl>(config, executor, logger);
}

std::unique_ptr<frame_builder> srsran::ether::create_vlan_frame_builder(const vlan_frame_params& eth_params)
{
  return std::make_unique<vlan_frame_builder_impl>(eth_params);
}

std::unique_ptr<frame_builder> srsran::ether::create_frame_builder(const vlan_frame_params& eth_params)
{
  return std::make_unique<frame_builder_impl>(eth_params);
}

std::unique_ptr<vlan_frame_decoder> srsran::ether::create_vlan_frame_decoder(srslog::basic_logger& logger,
                                                                             unsigned              sector_id)
{
  return std::make_unique<vlan_frame_decoder_impl>(logger, sector_id);
}
