/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#pragma once

#include "srsran/ofh/ethernet/ethernet_frame_pool.h"
#include "srsran/ofh/ethernet/ethernet_gateway.h"
#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier.h"
#include "srsran/ofh/transmitter/ofh_transmitter_timing_parameters.h"

namespace srsran {
namespace ofh {

/// \brief Transmits enqueued Open Fronthaul messages through an Ethernet gateway.
///
/// Message transmission is managed according the given transmission window.
class message_transmitter_impl : public ota_symbol_boundary_notifier
{
  /// Maximum number of frames allowed to be transmitted in a single burst.
  static constexpr unsigned MAX_BURST_SIZE = 64;

  /// Logger.
  srslog::basic_logger& logger;
  /// Ethernet frame pool.
  std::shared_ptr<ether::eth_frame_pool> pool_ptr;
  ether::eth_frame_pool&                 pool;
  /// Gateway handling message transmission.
  std::unique_ptr<ether::gateway> gateway;
  /// Internal representation of timing parameters.
  const tx_window_timing_parameters timing_params;

public:
  message_transmitter_impl(srslog::basic_logger&                  logger_,
                           const tx_window_timing_parameters&     timing_params_,
                           std::unique_ptr<ether::gateway>        gw,
                           std::shared_ptr<ether::eth_frame_pool> frame_pool);

  // See interface for documentation.
  void on_new_symbol(slot_symbol_point symbol_point) override;

private:
  /// Transmits the given frame burst.
  void transmit_frame_burst(span<span<const uint8_t>> frame_burst);

  /// Enqueues pending frames that match the given interval into the output buffer.
  void enqueue_messages_into_burst(const ether::frame_pool_interval&                   interval,
                                   static_vector<span<const uint8_t>, MAX_BURST_SIZE>& frame_burst);
};

} // namespace ofh
} // namespace srsran
