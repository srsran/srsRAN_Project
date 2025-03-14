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

#include "ofh_transmitter_impl.h"
#include "ofh_uplane_fragment_size_calculator.h"

using namespace srsran;
using namespace ofh;

static uplink_request_handler_impl_config generate_uplink_request_handler_config(const transmitter_config& tx_config)
{
  uplink_request_handler_impl_config config;
  config.sector              = tx_config.sector;
  config.is_prach_cp_enabled = tx_config.is_prach_cp_enabled;
  config.prach_eaxc          = tx_config.prach_eaxc;
  config.ul_data_eaxc        = tx_config.ul_eaxc;
  config.tdd_config          = tx_config.tdd_config;
  config.cp                  = tx_config.cp;
  config.scs                 = tx_config.scs;
  config.tx_timing_params    = tx_config.tx_timing_params;
  config.ul_processing_time  = tx_config.ul_processing_time;

  return config;
}

static uplink_request_handler_impl_dependencies
resolve_uplink_request_handler_dependencies(transmitter_impl_dependencies& tx_dependencies)
{
  uplink_request_handler_impl_dependencies dependencies;
  dependencies.logger        = tx_dependencies.logger;
  dependencies.ul_slot_repo  = std::move(tx_dependencies.ul_slot_repo);
  dependencies.ul_prach_repo = std::move(tx_dependencies.ul_prach_repo);
  dependencies.data_flow     = std::move(tx_dependencies.data_flow);
  dependencies.frame_pool    = tx_dependencies.frame_pool;

  return dependencies;
}

transmitter_impl::transmitter_impl(const transmitter_config& config, transmitter_impl_dependencies&& dependencies) :
  dl_manager(std::move(dependencies.dl_manager)),
  ul_request_handler(generate_uplink_request_handler_config(config),
                     resolve_uplink_request_handler_dependencies(dependencies)),
  msg_transmitter(*dependencies.logger,
                  config.tx_timing_params,
                  std::move(dependencies.eth_gateway),
                  std::move(dependencies.frame_pool)),
  ota_dispatcher(*dependencies.executor,
                 dl_manager->get_ota_symbol_boundary_notifier(),
                 ul_request_handler.get_ota_symbol_boundary_notifier(),
                 msg_transmitter)
{
  srsran_assert(dl_manager, "Invalid downlink manager");
}

uplink_request_handler& transmitter_impl::get_uplink_request_handler()
{
  return ul_request_handler;
}

downlink_handler& transmitter_impl::get_downlink_handler()
{
  return dl_manager->get_downlink_handler();
}

ota_symbol_boundary_notifier& transmitter_impl::get_ota_symbol_boundary_notifier()
{
  return ota_dispatcher;
}

void transmitter_impl::set_error_notifier(error_notifier& notifier)
{
  dl_manager->set_error_notifier(notifier);
  ul_request_handler.set_error_notifier(notifier);
}
