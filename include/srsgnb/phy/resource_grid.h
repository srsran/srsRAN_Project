#ifndef SRSGNB_SUPPORT_RESOURCE_GRID_H
#define SRSGNB_SUPPORT_RESOURCE_GRID_H

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/phy/constants.h"
#include <memory>

namespace srsgnb {

/// Describes a resource grid coordinate as symbol index and carrier.
struct resource_grid_coordinate {
  /// Symbol index (0...13).
  uint8_t symbol = 0;
  /// Subcarrier index (0...3299).
  uint16_t subcarrier = 0;

  /// Default destructor
  resource_grid_coordinate() = default;

  /// \brief Constructor from \c unsigned values.
  /// \param [in] symbol_ provides the \c symbol value.
  /// \param [in] subcarrier_ provides the \c subcarrier value.
  resource_grid_coordinate(unsigned symbol_, unsigned subcarrier_)
  {
    symbol     = static_cast<uint8_t>(symbol_);
    subcarrier = static_cast<uint16_t>(subcarrier_);
  }

  /// \brief Overloads equal comparison operator.
  ///
  /// Two resource grid coordinates are equal when symbol and subcarrier indexes are equal.
  ///
  /// \param other provides the reference of other coordinate.
  bool operator==(const resource_grid_coordinate& other) const
  {
    return (other.symbol == symbol) && (subcarrier == other.subcarrier);
  }
};

/// Describer a resource grid writer interface.
class resource_grid_writer
{
public:
  /// Default destructor
  virtual ~resource_grid_writer() = default;

  /// \brief Put a number of elements in the grid for a given port according to a list of coordinates.
  /// \param [in] port is the port index.
  /// \param [in] coordinates provides the list of grid symbol-subcarrier coordinates.
  /// \param [in] symbols provides the list with the symbols to map to the coordinates.
  /// \note the number of elements in \c coordinates and \c symbols shall be the same.
  virtual void put(unsigned port, span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols) = 0;

  /// \brief Put a number of resource elements in the resource grid at the given port and symbol using a mask to
  /// indicate which subcarriers are mapped and which not.
  /// \param [in] port is the port index.
  /// \param [in] l is the symbol index.
  /// \param [in] mask provides the mask to be used.
  /// \param [in,out] symbol_buffer provides the symbol buffer.
  /// \note The number of elements of \c mask shall be equal to or greater than the resource grid number of subcarriers.
  /// \note The number of elements of \c symbol_buffer shall be equal to or greater than the number of true elements in
  /// \c mask.
  virtual void put(unsigned port, unsigned l, span<const bool> mask, span<const cf_t>& symbol_buffer) = 0;

  /// \brief Put a consecutive number of resource elements for the given port and symbol \c l starting at \c k_init.
  /// \param [in] port is the port index.
  /// \param [in] l is the symbol index.
  /// \param [in] k_init is the initial subcarrier index.
  /// \param [out] symbols provides the symbols to map in the resource grid.
  /// \note The sum of \c k_init and the number of elements in \c symbols shall not exceed the resource grid number of
  /// subcarriers.
  virtual void put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols) = 0;
};

/// Describes a resource grid reader interface.
class resource_grid_reader
{
public:
  /// Default destructor.
  virtual ~resource_grid_reader() = default;

  /// \brief Get a number of elements from the grid for a given port according to a list of coordinates.
  /// \param [in] port is the port index.
  /// \param [in] coordinates provides the list of grid symbol-subcarrier coordinates.
  /// \param [out] symbols provides the list with the symbols to map to the coordinates.
  /// \note The number of elements in \c coordinates and \c symbols shall be the same.
  virtual void get(unsigned port, span<const resource_grid_coordinate> coordinates, span<cf_t> symbols) const = 0;

  /// \brief Get a number of resource elements in the resource grid at the given port and symbol using a mask to
  /// indicate which subcarriers are mapped and which not.
  /// \param [in] port is the port index.
  /// \param [in] l is the symbol index.
  /// \param [in] mask provides the mask to be used.
  /// \param [in,out] symbol_buffer provides the symbol buffer.
  /// \note The number of elements of \c mask shall be equal to or greater than the resource grid number of subcarriers.
  /// \note The number of elements of \c symbol_buffer shall be equal to or greater than the number of true elements in
  /// \c mask.
  virtual void get(unsigned port, unsigned l, span<const bool> mask, span<cf_t>& symbol_buffer) const = 0;

  /// \brief Get a consecutive number of resource elements for a given port and symbol \c l starting at \c k_init.
  /// \param [in] port is the port index.
  /// \param [in] l is the symbol index.
  /// \param [in] k_init is the initial subcarrier index.
  /// \param [out] symbols provides the symbols to map in the resource grid.
  /// \note The sum of \c k_init and the number of elements in \c symbols shall not exceed the resource grid number of
  /// subcarriers.
  virtual void get(unsigned port, unsigned l, unsigned k_init, span<cf_t> symbols) const = 0;
};

/// Describes a resource grid class with writer and reader interfaces.
class resource_grid : public resource_grid_writer, public resource_grid_reader
{
public:
  /// Set all resource elements in the grid to zero.
  virtual void set_all_zero() = 0;
};

/// \brief Creates a generic resource grid instance for a number of ports, symbols and subcarriers.
/// \param [in] nof_ports provides the number of ports.
/// \param [in] nof_symbols provides the number of symbols.
/// \param [in] nof_subc provides the number of subcarriers.
/// \return a unique pointer of a generic resource grid implementation.
std::unique_ptr<resource_grid> create_resource_grid(unsigned nof_ports, unsigned nof_symbols, unsigned nof_subc);

} // namespace srsgnb

#endif // SRSGNB_SUPPORT_RESOURCE_GRID_H
