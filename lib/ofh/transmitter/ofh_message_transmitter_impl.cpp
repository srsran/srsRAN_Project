/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

void message_transmitter_impl::transmit_enqueued_messages(slot_symbol_point start_symbol_point,
                                                          slot_symbol_point end_symbol_point,
                                                          message_type      type,
                                                          data_direction    direction)
{
  const ether::frame_pool_context context{
      start_symbol_point.get_slot(), start_symbol_point.get_symbol_index(), type, direction};
  const ether::frame_pool_interval interval{context, start_symbol_point, end_symbol_point};
  auto                             frame_buffers = pool.read_frame_buffers(interval);
  if (frame_buffers.empty()) {
    return;
  }

  static_vector<span<const uint8_t>, MAX_FRAMES> frames;
  for (const auto& frame : frame_buffers) {
    frames.push_back(frame->data());
  }

  gateway->send(frames);
  logger.debug(
      "Sending an Ethernet frame burst of size '{}' frames through gateway in interval '{}_{}':{}_{} and type '{}'",
      frames.size(),
      start_symbol_point.get_slot(),
      start_symbol_point.get_symbol_index(),
      end_symbol_point.get_slot(),
      end_symbol_point.get_symbol_index(),
      (type == message_type::control_plane) ? "control" : "user");

  pool.clear_sent_frame_buffers(interval);
}

void message_transmitter_impl::on_new_symbol(slot_symbol_point symbol_point)
{
  // Transmit pending DL Control-Plane messages.
  transmit_enqueued_messages(symbol_point + timing_params.sym_cp_dl_end,
                             symbol_point + timing_params.sym_cp_dl_start,
                             message_type::control_plane,
                             data_direction::downlink);

  // Transmit pending UL Control-Plane messages.
  transmit_enqueued_messages(symbol_point + timing_params.sym_cp_ul_end,
                             symbol_point + timing_params.sym_cp_ul_start,
                             message_type::control_plane,
                             data_direction::uplink);

  // Transmit pending User-Plane messages.
  transmit_enqueued_messages(symbol_point + timing_params.sym_up_dl_end,
                             symbol_point + timing_params.sym_up_dl_start,
                             message_type::user_plane,
                             data_direction::downlink);

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
    logger.warning("Detected '{}' late Ethernet frames in the transmitter queue for slot '{}', symbol '{}', type '{}', "
                   "direction '{}'",
                   frame_buffers.size(),
                   slot,
                   symbol_index,
                   (type == message_type::control_plane) ? "control" : "user",
                   (direction == data_direction::downlink) ? "downlink" : "uplink");
    pool.clear_sent_frame_buffers(context);
  }
}
