#ifndef SRSGNB_SUPPORT_RESOURCE_GRID_H
#define SRSGNB_SUPPORT_RESOURCE_GRID_H

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/phy/constants.h"
#include <memory>

namespace srsgnb {

/// According to TS 38.211: There is one resource grid for a given antenna port, subcarrier spacing configuration, and
/// transmission direction (downlink or uplink).

struct resource_grid_coordinate {
  uint8_t  symbol     = 0;
  uint16_t subcarrier = 0;

  resource_grid_coordinate() = default;

  resource_grid_coordinate(unsigned symbol_, unsigned subcarrier_)
  {
    symbol     = static_cast<uint8_t>(symbol_);
    subcarrier = static_cast<uint16_t>(subcarrier_);
  }

  bool operator==(const resource_grid_coordinate& other) const
  {
    return (other.symbol == symbol) && (subcarrier == other.subcarrier);
  }
};

/// Describer a resource grid writer interface
class resource_grid_writer
{
public:
  /// Default destructor
  virtual ~resource_grid_writer() = default;

  /// Put a number of elements in the grid according to a list of coordinates
  /// \param [in] coordinates provides the list of grid symbol-subcarrier coordinates
  /// \param [in] symbols provides the list with the symbols to map to the coordinates
  virtual void put(span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols) = 0;

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

  /// Get a number of elements in the grid according to a list of coordinates
  /// \param [in] coordinates provides the list of grid symbol-subcarrier coordinates
  /// \param [out] symbols provides the list with the symbols to map to the coordinates
  virtual void get(span<const resource_grid_coordinate> coordinates, span<cf_t> symbols) const = 0;

  /// Get a number of resource elements in the resource grid using a mask to indicate what subcarriers are mapped and
  /// which not.
  /// \param [in] l is the symbol index
  /// \param [in] mask provides the mask to be used
  /// \param [in,out] symbol_buffer provides the symbol buffer
  virtual void get(unsigned l, span<const bool> mask, span<cf_t>& symbol_buffer) const = 0;

  /// Get a consecutive number of resource elements for the symbol l starting at k_init
  /// \param l is the symbol index
  /// \param k_init is the initial subcarrier index
  /// \param symbols provides the symbols to map in the resource grid
  virtual void get(unsigned l, unsigned k_init, span<cf_t> symbols) const = 0;
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
