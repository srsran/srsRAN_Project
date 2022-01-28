#ifndef SRSGNB_PHY_RESOURCE_GRID_H
#define SRSGNB_PHY_RESOURCE_GRID_H

#include <complex>

namespace srsgnb {

/// According to TS 38.211: There is one resource grid for a given antenna port, subcarrier spacing configuration, and
/// transmission direction (downlink or uplink).
/// For this reason, I think we could keep separate resource grid instances and combine them at the end of the phy
/// processing?
class resource_grid
{
public:
  void                put(unsigned k, unsigned l, std::complex<float> value) {}
  std::complex<float> get(unsigned k, unsigned l) const { return {}; }

  //: TODO: provide a set of handy iterator classes to traverse the grid following the most common patterns.
};

} // namespace srsgnb

#endif // SRSGNB_PHY_RESOURCE_GRID_H
