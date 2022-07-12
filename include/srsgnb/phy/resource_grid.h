/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

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
  /// \param[in] symbol_ Provides the \c symbol value.
  /// \param[in] subcarrier_ Provides the \c subcarrier value.
  resource_grid_coordinate(unsigned symbol_, unsigned subcarrier_)
  {
    symbol     = static_cast<uint8_t>(symbol_);
    subcarrier = static_cast<uint16_t>(subcarrier_);
  }

  /// \brief Overloads equal comparison operator.
  ///
  /// Two resource grid coordinates are equal when symbol and subcarrier indexes are equal.
  ///
  /// \param other Provides the reference of other coordinate.
  bool operator==(const resource_grid_coordinate& other) const
  {
    return (other.symbol == symbol) && (subcarrier == other.subcarrier);
  }
};

/// \brief Resource grid writer interface.
///
/// Contains the necessary functions to write resource elements in a resource grid.
///
/// \remark All the methods contained in this interface must not result in writing outside of the resource grid region.
class resource_grid_writer
{
public:
  /// Default destructor
  virtual ~resource_grid_writer() = default;

  /// \brief Put a number of elements in the grid for a given port according to a list of coordinates.
  ///
  /// \param[in] port Denotes the port index.
  /// \param[in] coordinates Provides the list of grid symbol-subcarrier coordinates.
  /// \param[in] symbols Provides the list with the symbols to map to the coordinates.
  /// \note the number of elements in \c coordinates and \c symbols shall be the same.
  virtual void put(unsigned port, span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols) = 0;

  /// \brief Put a number of resource elements in the resource grid at the given port and symbol using a mask to
  /// indicate which subcarriers are mapped and which are not.
  ///
  /// \param[in] port Denotes the port index.
  /// \param[in] l Denotes the symbol index.
  /// \param[in] k_init Indicates the initial subcarrier index.
  /// \param[in] mask Provides the mask to be used.
  /// \param[in] symbol_buffer Provides the symbol buffer.
  /// \return It returns a \c span<cf_t> referencing the unused entries of \c symbols.
  /// \note The number of elements of \c mask plus \c k_init shall not exceed the resource grid bandwidth.
  /// \note The number of elements of \c symbol_buffer shall be equal to or greater than the number of true elements in
  /// \c mask.
  virtual span<const cf_t>
  put(unsigned port, unsigned l, unsigned k_init, span<const bool> mask, span<const cf_t> symbols) = 0;

  /// \brief Put a consecutive number of resource elements for the given port and symbol \c l starting at \c k_init.
  ///
  /// \param[in] port Denotes the port index.
  /// \param[in] l Denotes the symbol index.
  /// \param[in] k_init Indicates the initial subcarrier index.
  /// \param[out] symbols Provides the symbols to map in the resource grid.
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

  /// \brief Determines whether a given port is empty.
  ///
  /// A port is considered empty if no \c put method have been called since last zero set.
  ///
  /// If zeros are written using any \c put method, it is considered not empty.
  ///
  /// An invalid port index triggers an assertion.
  ///
  /// \param[in] port Port index.
  /// \return Return true if the given port is empty. Otherwise, false.
  virtual bool is_empty(unsigned port) const = 0;

  /// \brief Get a number of elements from the grid for a given port according to a list of coordinates.
  ///
  /// \param[out] symbols Provides the destination symbol buffer.
  /// \param[in] port Denotes the port index.
  /// \param[in] coordinates Provides the list of grid symbol-subcarrier coordinates.
  /// \note The number of elements in \c coordinates and \c symbols shall be the same.
  virtual void get(span<cf_t> symbols, unsigned port, span<const resource_grid_coordinate> coordinates) const = 0;

  /// \brief Get a number of resource elements in the resource grid at the given port and symbol using a mask to
  /// indicate which subcarriers are mapped and which are not.
  ///
  /// \param[out] symbols Provides the destination symbol buffer.
  /// \param[in] port Denotes the port index.
  /// \param[in] l Denotes the symbol index.
  /// \param[in] k_init Indicates the initial subcarrier index.
  /// \param[in] mask Provides the mask to be used.
  /// \return It returns a \c span<cf_t> referencing the unused entries of \c symbols.
  /// \note The number of elements of \c mask shall be equal to or greater than the resource grid number of subcarriers.
  /// \note The number of elements of \c symbol_buffer shall be equal to or greater than the number of true elements in
  /// \c mask.
  virtual span<cf_t>
  get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init, span<const bool> mask) const = 0;

  /// \brief Get a consecutive number of resource elements for a given port and symbol \c l starting at \c k_init.
  ///
  /// \param[out] symbols Provides the destination symbol buffer.
  /// \param[in] port Denotes the port index.
  /// \param[in] l Denotes the symbol index.
  /// \param[in] k_init Indicates the initial subcarrier index.
  /// \note The sum of \c k_init and the number of elements in \c symbols shall not exceed the resource grid number of
  /// subcarriers.
  virtual void get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init) const = 0;
};

/// Describes a resource grid class with writer and reader interfaces.
class resource_grid : public resource_grid_writer, public resource_grid_reader
{
public:
  /// Set all resource elements in the grid to zero.
  virtual void set_all_zero() = 0;
};

/// \brief Creates a generic resource grid instance for a number of ports, symbols and subcarriers.
/// \param[in] nof_ports Provides the number of ports.
/// \param[in] nof_symbols Provides the number of symbols.
/// \param[in] nof_subc Provides the number of subcarriers.
/// \return a unique pointer of a generic resource grid implementation.
std::unique_ptr<resource_grid> create_resource_grid(unsigned nof_ports, unsigned nof_symbols, unsigned nof_subc);

} // namespace srsgnb

#endif // SRSGNB_SUPPORT_RESOURCE_GRID_H
