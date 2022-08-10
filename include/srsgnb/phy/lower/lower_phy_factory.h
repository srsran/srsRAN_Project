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

#include "srsgnb/phy/lower/lower_phy.h"
#include "srsgnb/phy/lower/lower_phy_configuration.h"
#include "srsgnb/phy/lower/modulation/modulation_factories.h"
#include "srsgnb/phy/lower/processors/prach/prach_processor_factories.h"
#include <memory>

namespace srsgnb {

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
std::unique_ptr<lower_phy_factory>
create_lower_phy_factory_sw(std::shared_ptr<ofdm_modulator_factory>   modulator_factory,
                            std::shared_ptr<ofdm_demodulator_factory> demodulator_factory,
                            std::shared_ptr<prach_processor_factory>  prach_processor_factory);

} // namespace srsgnb
