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

#pragma once

#include "srsran/ofh/ethernet/ethernet_frame_pool.h"
#include "srsran/ofh/ethernet/ethernet_gateway.h"
#include "srsran/ofh/ofh_ota_symbol_handler.h"
#include "srsran/ofh/transmitter/ofh_transmitter_configuration.h"

namespace srsran {
namespace ofh {

/// \brief Transmits enqueued Open Fronthaul messages through an Ethernet gateway.
///
/// Message transmission is managed according the given transmission window.
class message_transmitter_impl : public ota_symbol_handler
{
  /// Internal structure used to store transmission window timing parameters expressed in a number of symbols.
  struct tx_timing_parameters {
    /// Offset from the current OTA symbol to the first symbol at which DL Control-Plane message can be sent, or in
    /// other words it is the offset to the start of DL Control-Plane transmission window. Must be calculated based on
    /// \c T1a_max_cp_dl parameter.
    unsigned sym_cp_dl_start;
    /// Offset from the current OTA symbol to the last symbol at which DL Control-Plane message can be sent within its
    /// transmission window. Must be calculated based on \c T1a_min_cp_dl parameter.
    unsigned sym_cp_dl_end;
    /// Offset from the current OTA symbol to the first symbol at which UL Control-Plane message can be sent within its
    /// transmission window. Must be calculated based on \c T1a_max_cp_ul parameter.
    unsigned sym_cp_ul_start;
    /// Offset from the current OTA symbol to the last symbol at which UL Control-Plane message can be sent within its
    /// transmission window. Must be calculated based on \c T1a_min_cp_ul parameter.
    unsigned sym_cp_ul_end;
    /// Offset from the current OTA symbol to the first symbol at which DL User-Plane message can be sent within its
    /// transmission window. Must be calculated based on \c T1a_max_up parameter.
    unsigned sym_up_dl_start;
    /// Offset from the current OTA symbol to the last symbol at which DL User-Plane message can be sent within its
    /// transmission window. Must be calculated based on \c T1a_min_up parameter.
    unsigned sym_up_dl_end;

    tx_timing_parameters(const du_tx_window_timing_parameters&    params,
                         std::chrono::duration<double, std::nano> symbol_duration) :
      sym_cp_dl_start(std::floor(params.T1a_max_cp_dl / symbol_duration)),
      sym_cp_dl_end(std::ceil(params.T1a_min_cp_dl / symbol_duration)),
      sym_cp_ul_start(std::floor(params.T1a_max_cp_ul / symbol_duration)),
      sym_cp_ul_end(std::ceil(params.T1a_min_cp_ul / symbol_duration)),
      sym_up_dl_start(std::floor(params.T1a_max_up / symbol_duration)),
      sym_up_dl_end(std::ceil(params.T1a_min_up / symbol_duration))
    {
    }
  };

  /// Logger.
  srslog::basic_logger& logger;
  /// Ethernet frame pool.
  std::shared_ptr<ether::eth_frame_pool> pool_ptr;
  ether::eth_frame_pool&                 pool;
  /// Gateway handling message transmission.
  std::unique_ptr<ether::gateway> gateway;
  /// Internal representation of timing parameters.
  const tx_timing_parameters timing_params;

public:
  message_transmitter_impl(srslog::basic_logger&                  logger_,
                           const symbol_handler_config&           cfg,
                           std::unique_ptr<ether::gateway>        gw,
                           std::shared_ptr<ether::eth_frame_pool> frame_pool);

  // See interface for documentation.
  void handle_new_ota_symbol(slot_symbol_point symbol_point) override;

private:
  /// Transmits enqueued messages for the given slot symbol point, message type and direction.
  void transmit_enqueued_messages(slot_symbol_point symbol_point, message_type type, data_direction direction);

  /// Logs the messages that could not be sent due the transmission window closed.
  void log_late_messages_on_tx_window_close(slot_symbol_point symbol_point);

  /// Logs the late messages for the given late slot symbol point, message type and direction.
  void log_late_messages(slot_symbol_point late_point, message_type type, data_direction direction);
};

} // namespace ofh
} // namespace srsran
