/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_transmitter_impl.h"
#include "ofh_uplane_fragment_size_calculator.h"

using namespace srsran;
using namespace ofh;

transmitter_impl::transmitter_impl(const transmitter_config& config, transmitter_impl_dependencies&& dependencies) :
  dl_manager(std::move(dependencies.dl_manager)),
  ul_request_handler(std::move(dependencies.ul_request_handler)),
  msg_transmitter(*dependencies.logger,
                  config.tx_timing_params,
                  std::move(dependencies.eth_gateway),
                  std::move(dependencies.frame_pool)),
  ota_dispatcher(*dependencies.executor, dl_manager->get_ota_symbol_boundary_notifier(), msg_transmitter)
{
  srsran_assert(dl_manager, "Invalid downlink manager");
  srsran_assert(ul_request_handler, "Invalid uplink request handler");
}

uplink_request_handler& transmitter_impl::get_uplink_request_handler()
{
  return *ul_request_handler;
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
}
