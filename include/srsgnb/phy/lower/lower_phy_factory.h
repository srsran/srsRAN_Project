
#ifndef SRSGNB_PHY_LOWER_LOWER_PHY_FACTORY_H
#define SRSGNB_PHY_LOWER_LOWER_PHY_FACTORY_H

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
  virtual std::unique_ptr<srsgnb::lower_phy> create(lower_phy_configuration& config) = 0;
};

/// Describes the necessary parameters to create a generic lower PHY factory.
struct lower_phy_factory_generic_configuration {
  /// Provides the OFDM modulation factory.
  ofdm_modulator_factory* modulator_factory;
  /// Provides the OFDM demodulation factory.
  ofdm_demodulator_factory* demodulator_factory;
};

std::unique_ptr<lower_phy_factory> create_lower_phy_factory_generic(lower_phy_factory_generic_configuration& config);

} // namespace srsgnb
#endif // SRSGNB_PHY_LOWER_LOWER_PHY_FACTORY_H
