/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ofh/ethernet/ethernet_frame_pool.h"
#include "srsran/ofh/ethernet/ethernet_gateway.h"
#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier.h"
#include "srsran/ofh/transmitter/ofh_transmitter_configuration.h"

namespace srsran {
namespace ofh {

/// \brief Transmits enqueued Open Fronthaul messages through an Ethernet gateway.
///
/// Message transmission is managed according the given transmission window.
class message_transmitter_impl : public ota_symbol_boundary_notifier
{
  /// Maximum number of frames allowed to be transmitted in a single burst.
  static constexpr unsigned MAX_BURST_SIZE = 64;

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
