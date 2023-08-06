/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

ota_symbol_handler& transmitter_impl::get_ota_symbol_handler()
{
  return ota_dispatcher;
}
