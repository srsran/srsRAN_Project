#ifndef SRSGNB_SUPPORT_RESOURCE_GRID_H
#define SRSGNB_SUPPORT_RESOURCE_GRID_H

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include <memory>

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

  /// Put a number of resource elements in the resource grid using a mask to indicate what subcarriers are mapped and
  /// which not.
  /// \param [in] l is the symbol index
  /// \param [in] mask provides the mask to be used
  /// \param [in,out] symbol_buffer provides the symbol buffer
  virtual void put(unsigned l, span<const bool> mask, span<const cf_t>& symbol_buffer) = 0;

  /// Put a consecutive number of resource elements for the symbol l starting at k_init
  /// \param l is the symbol index
  /// \param k_init is the initial subcarrier index
  /// \param symbols provides the symbols to map in the resource grid
  virtual void put(unsigned l, unsigned k_init, span<const cf_t> symbols) = 0;
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

  /// Get a number of resource elements in the resource grid using a mask to indicate what subcarriers are mapped and
  /// which not.
  /// \param [in] l is the symbol index
  /// \param [in] mask provides the mask to be used
  /// \param [in,out] symbol_buffer provides the symbol buffer
  virtual void get(unsigned l, span<const bool> mask, span<cf_t>& symbol_buffer) = 0;

  /// Get a consecutive number of resource elements for the symbol l starting at k_init
  /// \param l is the symbol index
  /// \param k_init is the initial subcarrier index
  /// \param symbols provides the symbols to map in the resource grid
  virtual void get(unsigned l, unsigned k_init, span<cf_t> symbols) = 0;
};

/// Describes a resource grid class with writer and reader interfaces
class resource_grid : public resource_grid_writer, public resource_grid_reader
{
public:
  /// Set all resource elements in the grid to zero
  virtual void all_zero() = 0;
};

std::unique_ptr<resource_grid> create_resource_grid(unsigned nof_symbols, unsigned nof_subc);

} // namespace srsgnb

#endif // SRSGNB_SUPPORT_RESOURCE_GRID_H
