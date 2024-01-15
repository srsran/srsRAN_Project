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

#include "srsran/gateways/baseband/baseband_gateway.h"
#include "srsran/phy/lower/amplitude_controller/amplitude_controller_factories.h"
#include "srsran/phy/lower/lower_phy_error_notifier.h"
#include "srsran/phy/lower/lower_phy_rx_symbol_notifier.h"
#include "srsran/phy/lower/lower_phy_timing_notifier.h"
#include "srsran/phy/lower/modulation/ofdm_demodulator.h"
#include "srsran/phy/lower/modulation/ofdm_modulator.h"
#include "srsran/phy/lower/sampling_rate.h"
#include "srsran/phy/support/resource_grid_pool.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/n_ta_offset.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

/// Describes a sector configuration.
struct lower_phy_sector_description {
  /// Indicates the sector bandwidth in resource blocks.
  unsigned bandwidth_rb;
  /// Indicates the downlink frequency.
  double dl_freq_hz;
  /// Indicates the uplink frequency.
  double ul_freq_hz;
  /// Number of transmit ports.
  unsigned nof_tx_ports;
  /// Number of receive ports.
  unsigned nof_rx_ports;
};

/// \brief Lower physical layer baseband gateway buffer size policy.
///
/// Determines the policy to select the baseband gateway buffer size.
enum class lower_phy_baseband_buffer_size_policy : unsigned {
  /// The buffer size matches the number of samples per slot.
  slot = 0,
  /// The buffer size matches the number of samples per half-slot.
  half_slot,
  /// The buffer size if equal to \ref baseband_gateway::get_transmitter_optimal_buffer_size for the transmitter and
  /// \ref baseband_gateway::get_receiver_optimal_buffer_size for the receiver.
  single_packet,
  /// The buffer size is is equal to the greatest multiple of \ref baseband_gateway::get_transmitter_optimal_buffer_size
  /// for the transmitter and \ref baseband_gateway::get_receiver_optimal_buffer_size for the receiver less than the
  /// number of samples per slot.
  optimal_slot,
};

/// Lower physical layer configuration.
struct lower_phy_configuration {
  /// Sector identifier.
  unsigned sector_id;
  /// Amplitude control logger.
  srslog::basic_logger* logger;
  /// Subcarrier spacing for the overall PHY.
  subcarrier_spacing scs;
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Shifts the DFT window by a fraction of the cyclic prefix [0, 1).
  float dft_window_offset;
  /// \brief Number of slots the timing handler is notified in advance of the transmission time.
  ///
  /// Sets the maximum allowed processing delay in slots.
  unsigned max_processing_delay_slots;
  /// Sampling rate.
  sampling_rate srate;
  /// Time alignment offset.
  n_ta_offset ta_offset;
  /// \brief Time alignment calibration in number of samples.
  ///
  /// Models the reception and transmission time misalignment inherent to the RF device. This time adjustment is
  /// subtracted from the UL-to-DL processing time offset for calibrating the baseband device.
  ///
  /// \remark Positive values cause a reduction of the RF transmission delay with respect to the RF reception, while
  /// negative values increase it.
  int time_alignment_calibration;
  /// \brief System time-based throttling.
  ///
  /// Determines a minimum baseband processor period time between downlink packets. It is expressed as a fraction of the
  /// time equivalent to the number of samples in the baseband buffer. Set to 0.9 to ensure that the downlink packets
  /// are processed with a minimum period of 90% of the buffer duration.
  ///
  /// Set to zero to disable this feature.
  float system_time_throttling;
  /// Baseband transmit buffer size policy.
  lower_phy_baseband_buffer_size_policy baseband_tx_buffer_size_policy;
  /// Baseband receive buffer size policy.
  lower_phy_baseband_buffer_size_policy baseband_rx_buffer_size_policy;
  /// Amplitude control parameters, including baseband gain and clipping.
  amplitude_controller_clipping_config amplitude_config;
  /// Provides the sectors configuration.
  std::vector<lower_phy_sector_description> sectors;
  /// Provides the baseband gateway.
  baseband_gateway* bb_gateway;
  /// Provides a symbol handler to notify the reception of symbols.
  lower_phy_rx_symbol_notifier* rx_symbol_notifier;
  /// Provides the timing handler to notify the timing boundaries.
  lower_phy_timing_notifier* timing_notifier;
  /// Provides the error handler to notify runtime errors.
  lower_phy_error_notifier* error_notifier;
  /// Receive task executor.
  task_executor* rx_task_executor;
  /// Transmit task executor.
  task_executor* tx_task_executor;
  /// Downlink task executor.
  task_executor* dl_task_executor;
  /// Uplink task executor.
  task_executor* ul_task_executor;
  /// PRACH asynchronous task executor.
  task_executor* prach_async_executor;
};

/// Returns true if the given lower PHY configuration is valid, otherwise false.
inline bool is_valid_lower_phy_config(const lower_phy_configuration& config)
{
  // :TODO: Implement me!

  return true;
}

} // namespace srsran
