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

#include "srsran/ofh/ethernet/ethernet_factories.h"
#include "ethernet_frame_builder_impl.h"
#include "ethernet_receiver_impl.h"
#include "ethernet_transmitter_impl.h"
#include "vlan_ethernet_frame_builder_impl.h"
#include "vlan_ethernet_frame_decoder_impl.h"

using namespace srsran;
using namespace ether;

std::unique_ptr<transmitter> srsran::ether::create_transmitter(const transmitter_config& config,
                                                               srslog::basic_logger&     logger)
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
