/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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