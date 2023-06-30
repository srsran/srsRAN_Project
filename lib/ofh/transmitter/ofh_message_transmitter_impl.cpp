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

#include "ofh_message_transmitter_impl.h"

using namespace srsran;
using namespace ofh;

message_transmitter_impl::message_transmitter_impl(srslog::basic_logger&                  logger_,
                                                   const symbol_handler_config&           cfg,
                                                   std::unique_ptr<ether::gateway>        gw,
                                                   std::shared_ptr<ether::eth_frame_pool> frame_pool) :
  logger(logger_),
  pool_ptr(frame_pool),
  pool(*pool_ptr),
  gateway(std::move(gw)),
  timing_params(
      cfg.tx_timing_params,
      std::chrono::duration<double, std::nano>(1e6 / (cfg.symbols_per_slot * get_nof_slots_per_subframe(cfg.scs))))
{
  srsran_assert(gateway, "Invalid Ethernet gateway");
}

void message_transmitter_impl::transmit_enqueued_messages(slot_symbol_point symbol_point, message_type type)
{
  slot_point slot         = symbol_point.get_slot();
  unsigned   symbol_index = symbol_point.get_symbol_index();

  span<const span<const ether::frame_buffer>> frame_buffers = pool.read_frame_buffers(slot, symbol_index, type);

  for (const auto buffers_span : frame_buffers) {
    for (const auto& eth_buffer : buffers_span) {
      if (eth_buffer.empty()) {
        continue;
      }

      // Send buffer.
      gateway->send(eth_buffer.data());

      logger.debug("Sending Ethernet frame through gateway of size={} in slot={}, symbol={}, type={}",
                   eth_buffer.size(),
                   slot,
                   symbol_index,
                   (type == message_type::control_plane) ? "control" : "user");
    }
  }

  pool.eth_frames_sent(slot, symbol_index, type);
}

void message_transmitter_impl::handle_new_ota_symbol(slot_symbol_point symbol_point)
{
  // Transmit pending DL Control-Plane messages.
  transmit_enqueued_messages(symbol_point + timing_params.sym_cp_dl_start, message_type::control_plane);

  // Transmit pending UL Control-Plane messages.
  transmit_enqueued_messages(symbol_point + timing_params.sym_cp_ul_start, message_type::control_plane);

  // Transmit pending User-Plane messages.
  transmit_enqueued_messages(symbol_point + timing_params.sym_up_dl_start, message_type::user_plane);
}
