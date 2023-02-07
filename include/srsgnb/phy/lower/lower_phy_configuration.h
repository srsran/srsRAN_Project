/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/gateways/baseband/baseband_gateway.h"
#include "srsgnb/phy/lower/amplitude_controller/amplitude_controller_factories.h"
#include "srsgnb/phy/lower/lower_phy_error_notifier.h"
#include "srsgnb/phy/lower/lower_phy_rx_symbol_notifier.h"
#include "srsgnb/phy/lower/lower_phy_timing_notifier.h"
#include "srsgnb/phy/lower/modulation/ofdm_demodulator.h"
#include "srsgnb/phy/lower/modulation/ofdm_modulator.h"
#include "srsgnb/phy/lower/sampling_rate.h"
#include "srsgnb/phy/support/resource_grid_pool.h"
#include "srsgnb/ran/cyclic_prefix.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include "srsgnb/support/executors/task_executor.h"

namespace srsgnb {

/// Describes the lower physical layer RF port mapping.
struct lower_phy_sector_port_mapping {
  /// Indicates the radio stream identifier.
  unsigned stream_id;
  /// Indicates the radio channel identifier within the stream.
  unsigned channel_id;
};

/// Describes a sector configuration.
struct lower_phy_sector_description {
  /// Indicates the sector bandwidth in resource blocks.
  unsigned bandwidth_rb;
  /// Indicates the downlink frequency.
  double dl_freq_hz;
  /// Indicates the uplink frequency.
  double ul_freq_hz;
  /// Provides the sector port mapping.
  std::vector<lower_phy_sector_port_mapping> port_mapping;
};

/// \brief Time advance offset - parameter \f$N_{TA,offset}\f$ in TS38.211 Section 4.3.3.
///
/// This value must be selected from the parameter \e n-TimingAdvanceOffset (TS38.331 Section 6.3.2, Information Element
/// \e ServingCellConfigCommon) if it is present. Otherwise, it is given by TS38.133 Section 7.1.2 depending on the
/// duplex mode of the cell and the frequency range.
///
/// The values are given in units of \f$T_c\f$ (see TS38.211 Section 4.1).
enum class lower_phy_ta_offset {
  /// For FR1 FDD band with LTE-NR coexistence case.
  n0 = 0,
  /// For FR1 FDD band without LTE-NR coexistence case or for FR1 TDD band without LTE-NR coexistence case.
  n25600 = 25600,
  /// For FR2.
  n13792 = 13792,
  /// For FR1 TDD band with LTE-NR coexistence case.
  n39936 = 39936
};

/// Lower physical layer configuration.
struct lower_phy_configuration {
  /// Indicates the log level.
  std::string log_level;
  /// Subcarrier spacing for the overall PHY.
  subcarrier_spacing scs;
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// \brief Number of slots the timing handler is notified in advance of the transmission time.
  ///
  /// Sets the maximum allowed processing delay in slots.
  unsigned max_processing_delay_slots;
  /// \brief Indicates the UL-to-DL slot context offset.
  ///
  /// Determines the time offset between the UL and DL processes in subframes or, equivalently, with a granularity of
  /// one millisecond.
  ///
  /// An assertion is triggered if it is equal to zero.
  unsigned ul_to_dl_subframe_offset;
  /// Sampling rate.
  sampling_rate srate;
  /// Time alignment offset.
  lower_phy_ta_offset ta_offset;
  /// \brief Time alignment calibration in number of samples.
  ///
  /// Models the reception and transmission time misalignment inherent to the RF device. This time adjustment is
  /// subtracted from the UL-to-DL processing time offset for calibrating the baseband device.
  ///
  /// \remark Positive values cause a reduction of the RF transmission delay with respect to the RF reception, while
  /// negative values increase it.
  int time_alignment_calibration;
  /// OFDM modulator scale.
  float tx_scale;
  /// Amplitude control parameters, including baseband gain and clipping.
  amplitude_controller_clipping_config amplitude_config;
  /// Provides the sectors configuration.
  std::vector<lower_phy_sector_description> sectors;
  /// Indicates the numbers of channels for every baseband stream.
  std::vector<unsigned> nof_channels_per_stream;
  /// Provides the baseband gateway.
  baseband_gateway* bb_gateway;
  /// Provides a symbol handler to notify the reception of symbols.
  lower_phy_rx_symbol_notifier* rx_symbol_notifier;
  /// Provides the timing handler to notify the timing boundaries.
  lower_phy_timing_notifier* timing_notifier;
  /// Provides the error handler to notify runtime errors.
  lower_phy_error_notifier* error_notifier;
  /// PRACH asynchronous task executor.
  task_executor* prach_async_executor;
};

/// Returns true if the given lower PHY configuration is valid, otherwise false.
inline bool is_valid_lower_phy_config(const lower_phy_configuration& config)
{
  // :TODO: Implement me!

  return true;
}

} // namespace srsgnb
