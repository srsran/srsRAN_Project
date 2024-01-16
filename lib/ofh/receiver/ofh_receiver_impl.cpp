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

#include "ofh_receiver_impl.h"
#include "srsran/ofh/ethernet/ethernet_properties.h"

using namespace srsran;
using namespace ofh;

static message_receiver_config get_message_receiver_configuration(const receiver_config& rx_config)
{
  message_receiver_config config;

  config.vlan_params.mac_src_address = rx_config.mac_src_address;
  config.vlan_params.mac_dst_address = rx_config.mac_dst_address;
  config.vlan_params.tci             = rx_config.tci;
  config.vlan_params.eth_type        = ether::ECPRI_ETH_TYPE;

  config.prach_eaxc = rx_config.prach_eaxc;
  config.ul_eaxc    = rx_config.ul_eaxc;

  return config;
}

static message_receiver_dependencies get_message_receiver_dependencies(receiver_impl_dependencies&& rx_dependencies,
                                                                       rx_window_checker&           window_checker)
{
  message_receiver_dependencies dependencies;

  dependencies.logger         = rx_dependencies.logger;
  dependencies.window_checker = &window_checker;
  dependencies.ecpri_decoder  = std::move(rx_dependencies.ecpri_decoder);
  srsran_assert(dependencies.ecpri_decoder, "Invalid eCPRI decoder");
  dependencies.eth_frame_decoder = std::move(rx_dependencies.eth_frame_decoder);
  srsran_assert(dependencies.eth_frame_decoder, "Invalid Ethernet frame decoder");
  dependencies.uplane_decoder = std::move(rx_dependencies.uplane_decoder);
  srsran_assert(dependencies.uplane_decoder, "Invalid Open Fronthaul User-Plane decoder");
  dependencies.data_flow_uplink = std::move(rx_dependencies.data_flow_uplink);
  srsran_assert(dependencies.data_flow_uplink, "Invalid uplink data flow decoder");
  dependencies.data_flow_prach = std::move(rx_dependencies.data_flow_prach);
  srsran_assert(dependencies.data_flow_prach, "Invalid PRACH data flow decoder");
  dependencies.seq_id_checker = std::move(rx_dependencies.seq_id_checker);
  srsran_assert(dependencies.seq_id_checker, "Invalid sequence id checker");

  return dependencies;
}

receiver_impl::receiver_impl(const receiver_config& config, receiver_impl_dependencies&& dependencies) :
  window_checker(*dependencies.logger,
                 config.rx_timing_params,
                 std::chrono::duration<double, std::nano>(
                     1e6 / (get_nsymb_per_slot(config.cp) * get_nof_slots_per_subframe(config.scs)))),
  msg_receiver(get_message_receiver_configuration(config),
               get_message_receiver_dependencies(std::move(dependencies), window_checker)),
  ctrl(msg_receiver)
{
}

ether::frame_notifier& receiver_impl::get_ethernet_frame_notifier()
{
  return msg_receiver;
}

ota_symbol_boundary_notifier& receiver_impl::get_ota_symbol_boundary_notifier()
{
  return window_checker;
}

controller& receiver_impl::get_controller()
{
  return ctrl;
}
