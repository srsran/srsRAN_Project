/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

static message_receiver_dependencies get_message_receiver_dependencies(receiver_impl_dependencies&& depen)
{
  message_receiver_dependencies dependencies;

  dependencies.logger            = depen.logger;
  dependencies.ecpri_decoder     = std::move(depen.ecpri_decoder);
  dependencies.eth_frame_decoder = std::move(depen.eth_frame_decoder);
  dependencies.uplane_decoder    = std::move(depen.uplane_decoder);
  dependencies.data_flow_uplink  = std::move(depen.data_flow_uplink);
  dependencies.data_flow_prach   = std::move(depen.data_flow_prach);

  return dependencies;
}

receiver_impl::receiver_impl(const receiver_config& config, receiver_impl_dependencies&& dependencies) :
  window_checker(*dependencies.logger,
                 config.rx_timing_params,
                 std::chrono::duration<double, std::nano>(
                     1e6 / (get_nsymb_per_slot(config.cp) * get_nof_slots_per_subframe(config.scs)))),
  msg_receiver(get_message_receiver_configuration(config), get_message_receiver_dependencies(std::move(dependencies)))
{
}

ether::frame_notifier& receiver_impl::get_ethernet_frame_notifier()
{
  return msg_receiver;
}

ota_symbol_handler& receiver_impl::get_ota_symbol_handler()
{
  return window_checker;
}
