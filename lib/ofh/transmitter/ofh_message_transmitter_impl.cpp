/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

void message_transmitter_impl::transmit_enqueued_messages(slot_symbol_point symbol_point,
                                                          message_type      type,
                                                          data_direction    direction)
{
  slot_point slot         = symbol_point.get_slot();
  unsigned   symbol_index = symbol_point.get_symbol_index();

  const ether::frame_pool_context context{slot, symbol_index, type, direction};

  auto frame_buffers = pool.read_frame_buffers(context);
  for (const auto* eth_buffer : frame_buffers) {
    // Send buffer.
    gateway->send(eth_buffer->data());

    logger.debug("Sending Ethernet frame through gateway of size={} in slot={}, symbol={}, type={}",
                 eth_buffer->size(),
                 slot,
                 symbol_index,
                 (type == message_type::control_plane) ? "control" : "user");
  }

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
}
