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

#include "srsran/phy/lower/lower_phy.h"
#include "srsran/phy/lower/lower_phy_configuration.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor_factories.h"
#include "srsran/phy/lower/processors/uplink/uplink_processor_factories.h"
#include <memory>

namespace srsran {

/// Lower physical layer factory.
class lower_phy_factory
{
public:
  /// Default destructor.
  virtual ~lower_phy_factory() = default;

  /// Creates a lower physical layer.
  virtual std::unique_ptr<lower_phy> create(lower_phy_configuration& config) = 0;
};

/// Creates a software based lower physical layer factory.
std::shared_ptr<lower_phy_factory>
create_lower_phy_factory_sw(std::shared_ptr<lower_phy_downlink_processor_factory> downlink_proc_factory,
                            std::shared_ptr<lower_phy_uplink_processor_factory>   uplink_proc_factory);

} // namespace srsran
