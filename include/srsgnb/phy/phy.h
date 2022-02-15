#ifndef SRSGNB_PHY_PHY_H
#define SRSGNB_PHY_PHY_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/phy/resource_grid.h"
#include <complex>

namespace srsgnb {

class phy_output_gateway
{
public:
  virtual bool send(const std::vector<std::complex<float> >& samples) = 0;
  // OR
  virtual bool send(const resource_grid_writer& grid) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_PHY_H
