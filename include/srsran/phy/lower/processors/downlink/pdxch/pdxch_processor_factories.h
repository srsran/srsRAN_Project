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
/// \param[in] request_queue_size Number of PDxCH processing requests that can be enqueued.
/// \param[in] ofdm_mod_factory   OFDM modulator factory.
/// \return A PDxCH processor factory.
std::shared_ptr<pdxch_processor_factory>
create_pdxch_processor_factory_sw(unsigned                                request_queue_size,
                                  std::shared_ptr<ofdm_modulator_factory> ofdm_mod_factory);

} // namespace srsran