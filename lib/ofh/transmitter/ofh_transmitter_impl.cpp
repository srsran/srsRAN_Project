/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
  dl_handler(std::move(dependencies.dl_handler)),
  ul_request_handler(std::move(dependencies.ul_request_handler)),
  msg_transmitter(*dependencies.logger,
                  config.symbol_handler_cfg,
                  std::move(dependencies.eth_gateway),
                  dependencies.frame_pool),
  ota_dispatcher(*dependencies.executor, *dependencies.window_handler, msg_transmitter)
{
  srsran_assert(dl_handler, "Invalid downlink handler");
  srsran_assert(ul_request_handler, "Invalid uplink request handler");
}

uplink_request_handler& transmitter_impl::get_uplink_request_handler()
{
  return *ul_request_handler;
}

downlink_handler& transmitter_impl::get_downlink_handler()
{
  return *dl_handler;
}

ota_symbol_boundary_notifier& transmitter_impl::get_ota_symbol_boundary_notifier()
{
  return ota_dispatcher;
}
