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
#include "srsran/adt/static_vector.h"

using namespace srsran;
using namespace ofh;

static constexpr unsigned MAX_FRAMES = 64;

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
  srsran_assert(pool_ptr, "Invalid frame pool");
}

void message_transmitter_impl::transmit_enqueued_messages(slot_symbol_point symbol_point,
                                                          message_type      type,
                                                          data_direction    direction)
{
  slot_point slot         = symbol_point.get_slot();
  unsigned   symbol_index = symbol_point.get_symbol_index();

  const ether::frame_pool_context context{slot, symbol_index, type, direction};
  auto                            frame_buffers = pool.read_frame_buffers(context);
  if (frame_buffers.empty()) {
    return;
  }

  static_vector<span<const uint8_t>, MAX_FRAMES> frames;
  for (const auto& frame : frame_buffers) {
    frames.push_back(frame->data());
  }

  gateway->send(frames);
  logger.debug("Sending Ethernet frame burst through gateway of size={} frames, in slot={}, symbol={}, type={}",
               frames.size(),
               slot,
               symbol_index,
               (type == message_type::control_plane) ? "control" : "user");

  pool.eth_frames_sent(context);
}

void message_transmitter_impl::handle_new_ota_symbol(slot_symbol_point symbol_point)
{
  // Transmit pending DL Control-Plane messages.
  transmit_enqueued_messages(
      symbol_point + timing_params.sym_cp_dl_start, message_type::control_plane, data_direction::downlink);

  // Transmit pending UL Control-Plane messages.
  transmit_enqueued_messages(
      symbol_point + timing_params.sym_cp_ul_start, message_type::control_plane, data_direction::uplink);

  // Transmit pending User-Plane messages.
  transmit_enqueued_messages(
      symbol_point + timing_params.sym_up_dl_start, message_type::user_plane, data_direction::downlink);

  // Log the late messages when the transmission window closes.
  log_late_messages_on_tx_window_close(symbol_point);
}

void message_transmitter_impl::log_late_messages_on_tx_window_close(slot_symbol_point symbol_point)
{
  log_late_messages(symbol_point + timing_params.sym_cp_dl_end, message_type::control_plane, data_direction::downlink);
  log_late_messages(symbol_point + timing_params.sym_cp_ul_end, message_type::control_plane, data_direction::uplink);
  log_late_messages(symbol_point + timing_params.sym_up_dl_end, message_type::user_plane, data_direction::downlink);
}

void message_transmitter_impl::log_late_messages(slot_symbol_point late_point,
                                                 message_type      type,
                                                 data_direction    direction)
{
  slot_point slot         = late_point.get_slot();
  unsigned   symbol_index = late_point.get_symbol_index();

  const ether::frame_pool_context context{slot, symbol_index, type, direction};
  auto                            frame_buffers = pool.read_frame_buffers(context);

  if (!frame_buffers.empty()) {
    logger.warning("Late Ethernet frames detected in the transmitter queue in slot={}, symbol={}, type={}, "
                   "direction={}, nof_frames={}",
                   slot,
                   symbol_index,
                   (type == message_type::control_plane) ? "control" : "user",
                   (direction == data_direction::downlink) ? "downlink" : "uplink",
                   frame_buffers.size());
  }
}
