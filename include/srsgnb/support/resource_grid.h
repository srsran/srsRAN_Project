#ifndef SRSGNB_SUPPORT_RESOURCE_GRID_H
#define SRSGNB_SUPPORT_RESOURCE_GRID_H

#include "../../../../../../../usr/include/c++/9/memory"
#include "../adt/complex.h"

namespace srsgnb {

/// According to TS 38.211: There is one resource grid for a given antenna port, subcarrier spacing configuration, and
/// transmission direction (downlink or uplink).

/// Describer a resource grid writer interface
class resource_grid_writer
{
public:
  /// Default destructor
  virtual ~resource_grid_writer() = default;

  /// Put a resource element in the grid
  /// \param [in] l is the symbol in the slot
  /// \param [in] k is the subcarrier index in the symbol
  /// \param [in] value is the resource element to write in the symbol l and subcarrier k
  virtual void put(unsigned l, unsigned k, cf_t value) = 0;
};

/// Describes a resource grid reader interface
class resource_grid_reader
{
public:
  /// Default destructor
  virtual ~resource_grid_reader() = default;

  /// Get a resource element from the resource grid
  /// \param [in] l is the symbol in the slot
  /// \param [in] k is the subcarrier index in the symbol
  /// \return the resource element given by the symbol and subcarrier indexes
  virtual cf_t get(unsigned l, unsigned k) const = 0;
};

/// Describes a resource grid class with writer and reader interfaces
class resource_grid : public resource_grid_writer, public resource_grid_reader
{
  /// Default destructor
  virtual ~resource_grid() = default;
};

} // namespace srsgnb

#endif // SRSGNB_SUPPORT_RESOURCE_GRID_H
