#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_RESOURCE_MAPPER
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_RESOURCE_MAPPER

#include "srsgnb/phy/resource_grid.h"
#include <complex>
#include <vector>

namespace srsgnb {

class pdsch_resource_mapper
{
public:
  // Receive a vector of symbols and a list of reserved indices and return a vector of resource grids, one grid per
  // layer or antenna port.
  // Need to add a cfg argument to command the mapper how to do its job.
  virtual std::vector<resource_grid> map(const std::vector<std::complex<float> >& input,
                                         const std::vector<unsigned>&             reserved_pattern) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_RESOURCE_MAPPER
