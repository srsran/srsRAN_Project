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

void message_transmitter_impl::transmit_frame_burst(span<span<const uint8_t>> frame_burst)
{
  if (frame_burst.empty()) {
    return;
  }

  gateway->send(frame_burst);
  logger.debug("Sending an Ethernet frame burst of size '{}'", frame_burst.size());
}

void message_transmitter_impl::enqueue_messages_into_burst(
    const ether::frame_pool_interval&                   interval,
    static_vector<span<const uint8_t>, MAX_BURST_SIZE>& frame_burst)
{
  auto frame_buffers = pool.read_frame_buffers(interval);
  if (frame_buffers.empty()) {
    return;
  }

  for (const auto& frame : frame_buffers) {
    frame_burst.push_back(frame->data());
  }

  logger.debug("Enqueueing '{}' frame(s) of type '{}' in interval '{}_{}':{}_{} for tx burst",
               frame_buffers.size(),
               (interval.type.type == message_type::control_plane) ? "control-plane" : "user-plane",
               interval.start.get_slot(),
               interval.start.get_symbol_index(),
               interval.end.get_slot(),
               interval.end.get_symbol_index());
}

void message_transmitter_impl::on_new_symbol(slot_symbol_point symbol_point)
{
  static_vector<span<const uint8_t>, MAX_BURST_SIZE> frame_burst;

  // Enqueue pending DL Control-Plane messages.
  ether::frame_pool_interval interval_cp_dl{{message_type::control_plane, data_direction::downlink},
                                            symbol_point + timing_params.sym_cp_dl_end,
                                            symbol_point + timing_params.sym_cp_dl_start};
  enqueue_messages_into_burst(interval_cp_dl, frame_burst);

  // Enqueue pending UL Control-Plane messages.
  ether::frame_pool_interval interval_cp_ul{{message_type::control_plane, data_direction::uplink},
                                            symbol_point + timing_params.sym_cp_ul_end,
                                            symbol_point + timing_params.sym_cp_ul_start};
  enqueue_messages_into_burst(interval_cp_ul, frame_burst);

  // Enqueue pending User-Plane messages.
  ether::frame_pool_interval interval_up{{message_type::user_plane, data_direction::downlink},
                                         symbol_point + timing_params.sym_up_dl_end,
                                         symbol_point + timing_params.sym_up_dl_start};
  enqueue_messages_into_burst(interval_up, frame_burst);

  // Transmit the data.
  transmit_frame_burst(frame_burst);

  // Clear sent buffers.
  pool.clear_sent_frame_buffers(interval_cp_dl);
  pool.clear_sent_frame_buffers(interval_cp_ul);
  pool.clear_sent_frame_buffers(interval_up);
}
