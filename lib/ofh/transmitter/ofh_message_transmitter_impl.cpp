/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
    const ether::frame_pool_interval&                   interval,
    static_vector<span<const uint8_t>, MAX_BURST_SIZE>& frame_burst,
    std::shared_ptr<ether::eth_frame_pool>&             pool)
{
  trace_point pool_access_tp = ofh_tracer.now();
  auto        frame_buffers  = pool->read_frame_buffers(interval);
  ofh_tracer << trace_event("ofh_tx_pool_access", pool_access_tp);

  if (frame_buffers.empty()) {
    return;
  }

  srsran_assert(frame_burst.size() + frame_buffers.size() <= frame_burst.capacity(), "Frame burst vector is too small");

  for (const auto& frame : frame_buffers) {
    frame_burst.emplace_back(frame->data());
  }

  if (SRSRAN_UNLIKELY(logger.debug.enabled())) {
    logger.debug("Enqueueing '{}' frame(s) of type '{}-{}' in interval '{}_{}':{}_{} for tx burst",
                 frame_buffers.size(),
                 (interval.type.type == message_type::control_plane) ? "control-plane" : "user-plane",
                 (interval.type.direction == data_direction::downlink) ? "downlink" : "uplink",
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

  trace_point tp = ofh_tracer.now();

  static_vector<span<const uint8_t>, MAX_BURST_SIZE> frame_burst;

  // Enqueue pending DL Control-Plane messages.
  ether::frame_pool_interval interval_cp_dl{{message_type::control_plane, data_direction::downlink},
                                            symbol_point_context.symbol_point + timing_params.sym_cp_dl_end,
                                            symbol_point_context.symbol_point + timing_params.sym_cp_dl_start};
  enqueue_messages_into_burst(interval_cp_dl, frame_burst, pool_dl_cp);

  // Enqueue pending UL Control-Plane messages.
  ether::frame_pool_interval interval_cp_ul{{message_type::control_plane, data_direction::uplink},
                                            symbol_point_context.symbol_point + timing_params.sym_cp_ul_end,
                                            symbol_point_context.symbol_point + timing_params.sym_cp_ul_start};
  enqueue_messages_into_burst(interval_cp_ul, frame_burst, pool_ul_cp);

  // Enqueue pending User-Plane messages.
  ether::frame_pool_interval interval_up{{message_type::user_plane, data_direction::downlink},
                                         symbol_point_context.symbol_point + timing_params.sym_up_dl_end,
                                         symbol_point_context.symbol_point + timing_params.sym_up_dl_start};
  enqueue_messages_into_burst(interval_up, frame_burst, pool_dl_up);

  // Transmit the data.
  trace_point tp_ether = ofh_tracer.now();
  transmit_frame_burst(frame_burst);
  ofh_tracer << trace_event("ofh_ether_tx", tp_ether);

  // Clear sent buffers.
  pool_dl_cp->clear_sent_frame_buffers(interval_cp_dl);
  pool_ul_cp->clear_sent_frame_buffers(interval_cp_ul);
  pool_dl_up->clear_sent_frame_buffers(interval_up);

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
