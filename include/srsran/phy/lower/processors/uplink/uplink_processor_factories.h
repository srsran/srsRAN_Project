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

#include "srsran/phy/lower/modulation/modulation_factories.h"
#include "srsran/phy/lower/processors/uplink/prach/prach_processor_factories.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor_factories.h"
#include "srsran/phy/lower/processors/uplink/uplink_processor.h"
#include "srsran/phy/lower/sampling_rate.h"
#include <memory>

namespace srsran {

/// Describes a sector configuration.
struct uplink_processor_configuration {
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
  /// Number of receive ports.
  unsigned nof_rx_ports;
};

/// Lower physical layer uplink processor - Factory interface.
class lower_phy_uplink_processor_factory
{
public:
  /// Default destructor.
  virtual ~lower_phy_uplink_processor_factory() = default;

  /// Creates a lower PHY uplink processor.
  virtual std::unique_ptr<lower_phy_uplink_processor> create(const uplink_processor_configuration& config) = 0;
};

/// Creates a software based uplink processor factory.
std::shared_ptr<lower_phy_uplink_processor_factory>
create_uplink_processor_factory_sw(std::shared_ptr<prach_processor_factory> prach_proc_factory,
                                   std::shared_ptr<puxch_processor_factory> puxch_proc_factory);

} // namespace srsran
