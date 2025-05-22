/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ofh_message_transmitter_metrics_collector.h"
#include "srsran/ofh/ethernet/ethernet_frame_pool.h"
#include "srsran/ofh/ethernet/ethernet_transmitter.h"
#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier.h"
#include "srsran/ofh/transmitter/ofh_transmitter_timing_parameters.h"

namespace srsran {
namespace ofh {

/// \brief Transmits enqueued Open Fronthaul messages through an Ethernet transmitter.
///
/// Message transmission is managed according the given transmission window.
class message_transmitter_impl : public ota_symbol_boundary_notifier
{
  /// Logger.
  srslog::basic_logger& logger;
  /// Ethernet frame pool.
  std::shared_ptr<ether::eth_frame_pool> pool_dl_cp;
  /// Ethernet frame pool.
  std::shared_ptr<ether::eth_frame_pool> pool_ul_cp;
  /// Ethernet frame pool.
  std::shared_ptr<ether::eth_frame_pool> pool_dl_up;
  /// Ethernet transmitter.
  std::unique_ptr<ether::transmitter> eth_transmitter;
  /// Metrics collector.
  message_transmitter_metrics_collector metrics_collector;
  /// Internal representation of timing parameters.
  const tx_window_timing_parameters timing_params;

public:
  message_transmitter_impl(srslog::basic_logger&                  logger_,
                           const tx_window_timing_parameters&     timing_params_,
                           bool                                   are_metrics_enabled,
                           std::unique_ptr<ether::transmitter>    eth_transmitter,
                           std::shared_ptr<ether::eth_frame_pool> pool_dl_cp_,
                           std::shared_ptr<ether::eth_frame_pool> pool_ul_cp_,
                           std::shared_ptr<ether::eth_frame_pool> pool_dl_up_);

  // See interface for documentation.
  void on_new_symbol(const slot_symbol_point_context& symbol_point_context) override;

  /// Returns the Ethernet transmitter of this Open Fronthaul message transmitter.
  ether::transmitter& get_ethernet_transmitter();

  /// Returns the metrics collector of this message transmitter implementation.
  message_transmitter_metrics_collector& get_metrics_collector();

private:
  /// Transmits the given frame burst.
  void transmit_frame_burst(span<span<const uint8_t>> frame_burst);

  /// Enqueues pending frames that match the given interval into the output buffer.
  void enqueue_messages_into_burst(const ether::frame_pool_interval&                                    interval,
                                   ofh::message_type                                                    type,
                                   ofh::data_direction                                                  direction,
                                   static_vector<ether::scoped_frame_buffer, ether::MAX_TX_BURST_SIZE>& frame_burst,
                                   std::shared_ptr<ether::eth_frame_pool>&                              pool);
};

} // namespace ofh
} // namespace srsran
