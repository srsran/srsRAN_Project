#ifndef SRSGNB_PHY_RESOURCE_GRID_H
#define SRSGNB_PHY_RESOURCE_GRID_H

#include "srsgnb/adt/complex.h"
#include <memory>

namespace srsgnb {

/// According to TS 38.211: There is one resource grid for a given antenna port, subcarrier spacing configuration, and
/// transmission direction (downlink or uplink).
/// For this reason, I think we could keep separate resource grid instances and combine them at the end of the phy
/// processing?
class resource_grid
{
public:
  virtual ~resource_grid() = default;

  /**
   * @brief Put a resource element in the grid
   * @param l Symbol of the slot
   * @param k Subcarrier index
   * @param value Complex value to write
   */
  virtual void put(unsigned l, unsigned k, cf_t value) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_RESOURCE_GRID_H
