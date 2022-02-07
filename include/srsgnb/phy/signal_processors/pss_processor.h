#ifndef SRSGNB_PHY_SIGNAL_PROCESSORS_PSS_PROCESSOR_H
#define SRSGNB_PHY_SIGNAL_PROCESSORS_PSS_PROCESSOR_H

#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/ssb_config.h"

namespace srsgnb {

class pss_processor
{
public:
  virtual void map(resource_grid& grid, const ssb_config& ssb_config) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_SIGNAL_PROCESSORS_PSS_PROCESSOR_H
