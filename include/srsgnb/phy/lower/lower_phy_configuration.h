
#ifndef SRSGNB_PHY_LOWER_LOWER_PHY_CONFIGURATION_H_
#define SRSGNB_PHY_LOWER_LOWER_PHY_CONFIGURATION_H_

#include "srsgnb/gateways/baseband/baseband_gateway.h"
#include "srsgnb/phy/cyclic_prefix.h"
#include "srsgnb/phy/lower/lower_phy_rx_symbol_notifier.h"
#include "srsgnb/phy/lower/lower_phy_timing_notifier.h"
#include "srsgnb/phy/lower/modulation/ofdm_demodulator.h"
#include "srsgnb/phy/lower/modulation/ofdm_modulator.h"
#include "srsgnb/phy/resource_grid_pool.h"

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

/// Describes the lower PHY configuration.
struct lower_phy_configuration {
  /// Indicates the DFT size for a 15kHz SCS.
  unsigned dft_size_15kHz;
  /// Indicates the numerology.
  unsigned numerology;
  /// Indicates the number of slots that the timing handler is notified in advance. This sets the maximum allowed
  /// processing delay in slots.
  unsigned max_processing_delay_slots;
  /// Indicates the UL/DL slot context offset.
  unsigned ul_to_dl_slot_offset;
  /// Indicates the size of the circular transmit resource grid buffer in slots. It must be greater than \c
  /// max_processing_delay_slots and divisor of the number of slots per system frame number.
  unsigned nof_dl_rg_buffers;
  /// Indicates the Rx to Tx delay in seconds.
  double rx_to_tx_delay;
  /// OFDM modulator scale.
  float tx_scale;
  /// Indicates the cyclic prefix.
  cyclic_prefix cp;
  /// Provides the radio data plane.
  baseband_gateway* bb_gateway;
  /// Provides a symbol handler to notify the reception of symbols.
  lower_phy_rx_symbol_notifier* rx_symbol_notifier;
  /// Provides the timing handler to notify the timing boundaries.
  lower_phy_timing_notifier* timing_notifier;
  /// Provides the OFDM modulator factory.
  ofdm_modulator_factory* modulator_factory;
  /// Provides the OFDM demodulator factory.
  ofdm_demodulator_factory* demodulator_factory;
  /// Provides the uplink resource grid pool.
  resource_grid_pool* ul_resource_grid_pool;
  /// Provides the sectors configuration.
  std::vector<lower_phy_sector_description> sectors;
  /// Indicates the numbers of channels for every stream.
  std::vector<unsigned> nof_channels_per_stream;
  /// Indicates the log level.
  std::string log_level;
};

} // namespace srsgnb

#endif // SRSGNB_INCLUDE_SRSGNB_PHY_LOWER_LOWER_PHY_CONFIGURATION_H_
