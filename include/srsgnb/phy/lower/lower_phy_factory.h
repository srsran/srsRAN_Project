
#ifndef SRSGNB_PHY_LOWER_LOWER_PHY_FACTORY_H
#define SRSGNB_PHY_LOWER_LOWER_PHY_FACTORY_H

#include "srsgnb/phy/lower/lower_phy_configuration.h"
#include "srsgnb/phy/lower/lower_phy_control.h"

namespace srsgnb {

/// Describes a generic lower physical layer factory.
class lower_phy_factory
{
public:
  /// \brief Creates a generic lower physical layer control.
  /// \param[in] config Provides the required configuration.
  std::unique_ptr<srsgnb::lower_phy_control> create(lower_phy_configuration& config);
};

} // namespace srsgnb
#endif // SRSGNB_INCLUDE_SRSGNB_PHY_LOWER_LOWER_PHY_FACTORY_H
