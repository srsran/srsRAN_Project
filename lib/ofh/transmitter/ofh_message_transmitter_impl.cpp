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

#include "ofh_message_transmitter_impl.h"
#include "srsran/adt/static_vector.h"
#include "srsran/instrumentation/traces/ofh_traces.h"

using namespace srsran;
using namespace ofh;

message_transmitter_impl::message_transmitter_impl(srslog::basic_logger&                  logger_,
                                                   const tx_window_timing_parameters&     timing_params_,
                                                   bool                                   are_metrics_enabled,
                                                   std::unique_ptr<ether::transmitter>    transmitter,
                                                   std::shared_ptr<ether::eth_frame_pool> pool_dl_cp_,
                                                   std::shared_ptr<ether::eth_frame_pool> pool_ul_cp_,
                                                   std::shared_ptr<ether::eth_frame_pool> pool_dl_up_) :
  logger(logger_),
  pool_dl_cp(std::move(pool_dl_cp_)),
  pool_ul_cp(std::move(pool_ul_cp_)),
  pool_dl_up(std::move(pool_dl_up_)),
  eth_transmitter(std::move(transmitter)),
  metrics_collector(are_metrics_enabled),
  timing_params(timing_params_)
{
  srsran_assert(eth_transmitter, "Invalid Ethernet transmitter");
  srsran_assert(pool_dl_cp, "Invalid Control-Plane downlink frame pool");
  srsran_assert(pool_ul_cp, "Invalid Control-Plane uplink frame pool");
  srsran_assert(pool_dl_up, "Invalid User-Plane downlink frame pool");
}

void message_transmitter_impl::transmit_frame_burst(span<span<const uint8_t>> frame_burst)
{
  if (frame_burst.empty()) {
    return;
  }

  eth_transmitter->send(frame_burst);

  if (SRSRAN_UNLIKELY(logger.debug.enabled())) {
    logger.debug("Sending an Ethernet frame burst of size '{}'", frame_burst.size());
  }
}

void message_transmitter_impl::enqueue_messages_into_burst(
    const ether::frame_pool_interval&                                    interval,
    ofh::message_type                                                    type,
    ofh::data_direction                                                  direction,
    static_vector<ether::scoped_frame_buffer, ether::MAX_TX_BURST_SIZE>& read_frames,
    std::shared_ptr<ether::eth_frame_pool>&                              pool)
{
  trace_point pool_access_tp = ofh_tracer.now();

  unsigned prev_size = read_frames.size();
  pool->enqueue_pending_into_burst(interval, read_frames);

  ofh_tracer << trace_event("ofh_tx_pool_access", pool_access_tp);

  if ((read_frames.size() - prev_size) == 0) {
    return;
  }

  if (SRSRAN_UNLIKELY(logger.debug.enabled())) {
    logger.debug("Enqueueing '{}' frame(s) of type '{}-{}' in interval '{}_{}':{}_{} for tx burst",
                 read_frames.size() - prev_size,
                 (type == message_type::control_plane) ? "control-plane" : "user-plane",
                 (direction == data_direction::downlink) ? "downlink" : "uplink",
                 interval.start.get_slot(),
                 interval.start.get_symbol_index(),
                 interval.end.get_slot(),
                 interval.end.get_symbol_index());
  }
}

void message_transmitter_impl::on_new_symbol(const slot_symbol_point_context& symbol_point_context)
{
  // Creates and starts the execution time measurer.
  time_execution_measurer meas(metrics_collector.enabled());

  trace_point                                                         tp = ofh_tracer.now();
  static_vector<ether::scoped_frame_buffer, ether::MAX_TX_BURST_SIZE> read_frames;

  // Enqueue pending DL Control-Plane messages.
  ether::frame_pool_interval interval_cp_dl{symbol_point_context.symbol_point + timing_params.sym_cp_dl_end,
                                            symbol_point_context.symbol_point + timing_params.sym_cp_dl_start};
  enqueue_messages_into_burst(
      interval_cp_dl, message_type::control_plane, data_direction::downlink, read_frames, pool_dl_cp);

  // Enqueue pending UL Control-Plane messages.
  ether::frame_pool_interval interval_cp_ul{symbol_point_context.symbol_point + timing_params.sym_cp_ul_end,
                                            symbol_point_context.symbol_point + timing_params.sym_cp_ul_start};
  enqueue_messages_into_burst(
      interval_cp_ul, message_type::control_plane, data_direction::uplink, read_frames, pool_ul_cp);

  // Enqueue pending User-Plane messages.
  ether::frame_pool_interval interval_up{symbol_point_context.symbol_point + timing_params.sym_up_dl_end,
                                         symbol_point_context.symbol_point + timing_params.sym_up_dl_start};
  enqueue_messages_into_burst(interval_up, message_type::user_plane, data_direction::downlink, read_frames, pool_dl_up);

  // Construct burst of byte buffers ready to be transmitted.
  static_vector<span<const uint8_t>, ether::MAX_TX_BURST_SIZE> frame_burst;

  for (const auto& frame : read_frames) {
    frame_burst.emplace_back(frame->data());
  }

  // Transmit the data.
  trace_point tp_ether = ofh_tracer.now();
  transmit_frame_burst(frame_burst);

  ofh_tracer << trace_event("ofh_ether_tx", tp_ether);
  ofh_tracer << trace_event("ofh_message_transmitter", tp);

  metrics_collector.update_stats(meas.stop());
}

ether::transmitter& message_transmitter_impl::get_ethernet_transmitter()
{
  return *eth_transmitter;
}

message_transmitter_metrics_collector& message_transmitter_impl::get_metrics_collector()
{
  return metrics_collector;
}
