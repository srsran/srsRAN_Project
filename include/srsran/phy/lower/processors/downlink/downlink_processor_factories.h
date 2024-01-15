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
#include "srsran/phy/lower/processors/downlink/downlink_processor.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_factories.h"
#include "srsran/phy/lower/sampling_rate.h"
#include <memory>

namespace srsran {

/// Describes a sector configuration.
struct downlink_processor_configuration {
  /// Sector identifier.
  unsigned sector_id;
  /// Subcarrier spacing.
  subcarrier_spacing scs;
  /// Cyclic prefix configuration.
  cyclic_prefix cp;
  /// Baseband sampling rate.
  sampling_rate rate;
  /// Bandwidth in PRB.
  unsigned bandwidth_prb;
  /// Center frequency in Hz.
  double center_frequency_Hz;
  /// Number of transmit ports.
  unsigned nof_tx_ports;
  /// Number of slots notified in advance in the TTI boundary event.
  unsigned nof_slot_tti_in_advance;
  /// Logger.
  srslog::basic_logger* logger;
};

/// Lower physical layer downlink processor - Factory interface.
class lower_phy_downlink_processor_factory
{
public:
  /// Default destructor.
  virtual ~lower_phy_downlink_processor_factory() = default;

  /// Creates a lower PHY downlink processor.
  virtual std::unique_ptr<lower_phy_downlink_processor> create(const downlink_processor_configuration& config) = 0;
};

/// Creates a software based downlink processor factory.
std::shared_ptr<lower_phy_downlink_processor_factory>
create_downlink_processor_factory_sw(std::shared_ptr<pdxch_processor_factory>      pdxch_proc_factory,
                                     std::shared_ptr<amplitude_controller_factory> amplitude_control_factory);

} // namespace srsran