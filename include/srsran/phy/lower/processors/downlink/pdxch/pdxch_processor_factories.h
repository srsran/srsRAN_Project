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

#include "srsran/phy/lower/amplitude_controller/amplitude_controller_factories.h"
#include "srsran/phy/lower/modulation/modulation_factories.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor.h"
#include "srsran/phy/lower/sampling_rate.h"
#include <memory>

namespace srsran {

class task_executor;

/// Collects the necessary parameters to create a PDxCH processor.
struct pdxch_processor_configuration {
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Subcarrier spacing.
  subcarrier_spacing scs;
  /// Sampling rate.
  sampling_rate srate;
  /// Sector bandwidth as a number of resource blocks.
  unsigned bandwidth_rb;
  /// Uplink center frequency in Hz.
  double center_freq_Hz;
  /// Number of transmit ports.
  unsigned nof_tx_ports;
  /// Task executor for asynchronous modulation.
  task_executor& modulation_executor;
};

/// Lower physical layer PDxCH processor - Factory interface.
class pdxch_processor_factory
{
public:
  /// Default destructor.
  virtual ~pdxch_processor_factory() = default;

  /// Creates a lower PHY PDxCH processor.
  virtual std::unique_ptr<pdxch_processor> create(const pdxch_processor_configuration& config) = 0;
};

/// \brief Creates a software based PDxCH processor factory.
///
/// \param[in] ofdm_mod_factory          OFDM modulator factory.
/// \param[in] amplitude_control_factory Amplitude control factory.
/// \return A PDxCH processor factory.
std::shared_ptr<pdxch_processor_factory>
create_pdxch_processor_factory_sw(std::shared_ptr<ofdm_modulator_factory>       ofdm_mod_factory,
                                  std::shared_ptr<amplitude_controller_factory> amplitude_control_factory);

} // namespace srsran
