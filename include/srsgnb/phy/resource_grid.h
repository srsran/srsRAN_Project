#ifndef SRSGNB_PHY_RESOURCE_GRID_H
#define SRSGNB_PHY_RESOURCE_GRID_H

#include "srsgnb/adt/complex.h"
#include <memory>

namespace srsgnb {

/// According to TS 38.211: There is one resource grid for a given antenna port, subcarrier spacing configuration, and
/// transmission direction (downlink or uplink).
class resource_grid_writer
{
public:
  virtual ~resource_grid_writer() = default;

  /**
   * @brief Put a resource element in the grid
   * @param l Symbol of the slot
   * @param k Subcarrier index
   * @param value Complex value to write
   */
  virtual void put(unsigned l, unsigned k, cf_t value) = 0;
};

class resource_grid_reader
{
public:
  virtual ~resource_grid_reader() = default;

  /**
   * @brief Get a resource element in the grid
   * @param l Symbol of the slot
   * @param k Subcarrier index
   * @return The complex value read from the grid
   */
  virtual cf_t get(unsigned l, unsigned k) const = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_RESOURCE_GRID_H
