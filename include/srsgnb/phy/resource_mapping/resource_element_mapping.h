#ifndef SRSGNB_PHY_RESOURCE_MAPPING_RESOURCE_ELEMENT_MAPPING
#define SRSGNB_PHY_RESOURCE_MAPPING_RESOURCE_ELEMENT_MAPPING

#include "srsgnb/support/resource_grid.h"
#include <complex>
#include <vector>

namespace srsgnb {

class resource_element_mapping
{
public:
  // I'm assuming this interface is generic for any type of channel mapping, if different channels need very different
  // cfg structs we will need to provide different interfaces.
  virtual resource_grid map(const std::vector<std::complex<float> >& input, const std::vector<unsigned>& indices) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_RESOURCE_MAPPING_RESOURCE_ELEMENT_MAPPING
