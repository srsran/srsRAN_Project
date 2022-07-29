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
#include <memory>

namespace srsgnb {

/// Describes a generic lower physical layer factory.
class lower_phy_factory
{
public:
  /// Default destructor.
  virtual ~lower_phy_factory() = default;

  /// \brief Creates a generic lower physical layer control.
  /// \param[in] config Provides the required configuration.
  virtual std::unique_ptr<lower_phy> create(const lower_phy_configuration& config) = 0;
};

/// Describes the necessary parameters to create a generic lower PHY factory.
struct lower_phy_factory_generic_configuration {
  /// Provides the OFDM modulation factory.
  std::shared_ptr<ofdm_modulator_factory> modulator_factory;
  /// Provides the OFDM demodulation factory.
  std::shared_ptr<ofdm_demodulator_factory> demodulator_factory;
};

std::unique_ptr<lower_phy_factory> create_lower_phy_factory_generic(lower_phy_factory_generic_configuration& config);

} // namespace srsgnb
