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

#include "ofh_receiver_impl.h"
#include "ofh_message_receiver_task_dispatcher.h"
#include "srsran/ofh/ethernet/ethernet_properties.h"

using namespace srsran;
using namespace ofh;

static message_receiver_config get_message_receiver_configuration(const receiver_config& rx_config)
{
  message_receiver_config config;

  config.sector                        = rx_config.sector;
  config.nof_symbols                   = get_nsymb_per_slot(rx_config.cp);
  config.scs                           = rx_config.scs;
  config.vlan_params.mac_src_address   = rx_config.mac_src_address;
  config.vlan_params.mac_dst_address   = rx_config.mac_dst_address;
  config.vlan_params.tci               = rx_config.tci;
  config.vlan_params.eth_type          = ether::ECPRI_ETH_TYPE;
  config.warn_unreceived_frames        = rx_config.log_unreceived_ru_frames;
  config.are_metrics_enabled           = rx_config.are_metrics_enabled;
  config.enable_log_warnings_for_lates = rx_config.enable_log_warnings_for_lates;

  config.prach_eaxc = rx_config.prach_eaxc;
  config.ul_eaxc    = rx_config.ul_eaxc;

  return config;
}

static message_receiver_dependencies
get_message_receiver_dependencies(receiver_impl_dependencies::message_rx_dependencies rx_dependencies,
                                  rx_window_checker&                                  window_checker,
                                  closed_rx_window_handler&                           window_handler)
{
  message_receiver_dependencies dependencies;
  dependencies.logger         = rx_dependencies.logger;
  dependencies.window_checker = &window_checker;
  dependencies.window_handler = &window_handler;
  dependencies.ecpri_decoder  = std::move(rx_dependencies.ecpri_decoder);
  srsran_assert(dependencies.ecpri_decoder, "Invalid eCPRI decoder");
  dependencies.eth_frame_decoder = std::move(rx_dependencies.eth_frame_decoder);
  srsran_assert(dependencies.eth_frame_decoder, "Invalid Ethernet frame decoder");
  dependencies.data_flow_uplink = std::move(rx_dependencies.data_flow_uplink);
  srsran_assert(dependencies.data_flow_uplink, "Invalid uplink data flow decoder");
  dependencies.data_flow_prach = std::move(rx_dependencies.data_flow_prach);
  srsran_assert(dependencies.data_flow_prach, "Invalid PRACH data flow decoder");
  dependencies.seq_id_checker = std::move(rx_dependencies.seq_id_checker);
  srsran_assert(dependencies.seq_id_checker, "Invalid sequence id checker");

  return dependencies;
}

static closed_rx_window_handler_config get_closed_rx_window_handler_config(const receiver_config& config)
{
  closed_rx_window_handler_config out_config;
  out_config.sector                 = config.sector;
  out_config.warn_unreceived_frames = config.log_unreceived_ru_frames;
  out_config.rx_timing_params       = config.rx_timing_params;
  // As it runs in the same executor, do not delay the reception window close.
  out_config.nof_symbols_to_process_uplink = 0;

  return out_config;
}

static closed_rx_window_handler_dependencies
generate_closed_rx_window_dependencies(receiver_impl_dependencies::close_rx_window_dependencies&& dependencies,
                                       srslog::basic_logger&                                      logger,
                                       task_executor&                                             executor)
{
  closed_rx_window_handler_dependencies out_dependencies;
  out_dependencies.logger      = &logger;
  out_dependencies.executor    = &executor;
  out_dependencies.prach_repo  = std::move(dependencies.prach_repo);
  out_dependencies.uplink_repo = std::move(dependencies.uplink_repo);
  out_dependencies.notifier    = std::move(dependencies.notifier);

  return out_dependencies;
}

void ota_symbol_boundary_dispatcher::on_new_symbol(const slot_symbol_point_context& symbol_point_context)
{
  for (auto& handler : handlers) {
    handler->on_new_symbol(symbol_point_context);
  }
}

receiver_impl::receiver_impl(const receiver_config& config, receiver_impl_dependencies&& dependencies) :
  symbol_reorderer(std::move(dependencies.symbol_reorderer)),
  closed_window_handler(get_closed_rx_window_handler_config(config),
                        generate_closed_rx_window_dependencies(std::move(dependencies.window_handler_dependencies),
                                                               *dependencies.logger,
                                                               *dependencies.executor)),
  window_checker(config.are_metrics_enabled, config.rx_timing_params),
  symbol_boundary_dispatcher([](closed_rx_window_handler& handler, rx_window_checker& checker) {
    std::vector<ota_symbol_boundary_notifier*> handlers;
    if (!checker.disabled()) {
      handlers.push_back(&checker);
    }
    handlers.push_back(&handler);

    return handlers;
  }(closed_window_handler, window_checker)),
  msg_receiver(get_message_receiver_configuration(config),
               get_message_receiver_dependencies(std::move(dependencies.msg_rx_dependencies),
                                                 window_checker,
                                                 closed_window_handler)),
  metrics_collector(config.are_metrics_enabled,
                    closed_window_handler,
                    window_checker,
                    msg_receiver.get_metrics_collector(),
                    dependencies.eth_receiver->get_metrics_collector()),
  rcv_task_dispatcher(*dependencies.logger,
                      msg_receiver,
                      *dependencies.executor,
                      config.sector,
                      std::move(dependencies.eth_receiver)),
  ctrl(rcv_task_dispatcher, closed_window_handler)
{
}

ota_symbol_boundary_notifier* receiver_impl::get_ota_symbol_boundary_notifier()
{
  return &symbol_boundary_dispatcher;
}

operation_controller& receiver_impl::get_operation_controller()
{
  return ctrl;
}

receiver_metrics_collector* receiver_impl::get_metrics_collector()
{
  return metrics_collector.disabled() ? nullptr : &metrics_collector;
}
