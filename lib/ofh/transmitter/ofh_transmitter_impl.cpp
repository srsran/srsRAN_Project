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
#include "scoped_frame_buffer.h"
#include "srsran/ofh/ethernet/ethernet_properties.h"

using namespace srsran;
using namespace ofh;

transmitter_impl::transmitter_impl(const transmitter_config& config, transmitter_impl_dependencies&& depen) :
  dl_handler(std::move(depen.dl_handler)),
  ul_request_handler(std::move(depen.ul_request_handler)),
  msg_transmitter(*depen.logger, config.symbol_handler_cfg, std::move(depen.eth_gateway), depen.frame_pool)
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

ota_symbol_handler& transmitter_impl::get_ota_symbol_handler()
{
  return msg_transmitter;
}
