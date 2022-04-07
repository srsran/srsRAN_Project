
#include "srsgnb/phy/lower/lower_phy_factory.h"
#include "lower_phy_impl.h"

using namespace srsgnb;

std::unique_ptr<lower_phy_control> lower_phy_factory::create(lower_phy_configuration& config)
{
  return std::make_unique<lower_phy_impl>(config);
}