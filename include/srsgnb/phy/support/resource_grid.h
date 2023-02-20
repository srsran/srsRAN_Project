/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/phy/constants.h"

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
  /// \param[in] symbol_     Provides the \c symbol value.
  /// \param[in] subcarrier_ Provides the \c subcarrier value.
  resource_grid_coordinate(unsigned symbol_, unsigned subcarrier_) :
    symbol(static_cast<uint8_t>(symbol_)), subcarrier(static_cast<uint16_t>(subcarrier_))
  {
  }

  /// \brief Overloads equal comparison operator.
  ///
  /// Two resource grid coordinates are equal when symbol and subcarrier indexes are equal.
  /// \param[in] other The comparing coordinate.
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

  /// \brief Puts a number of elements in the grid for a given port according to a list of coordinates.
  ///
  /// \param[in] port        Port index.
  /// \param[in] coordinates List of grid symbol&ndash;subcarrier coordinates.
  /// \param[in] symbols     List of symbols to written at the specified coordinates.
  /// \note The number of elements in \c coordinates and \c symbols shall be the same.
  virtual void put(unsigned port, span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols) = 0;

  /// \brief Puts a number of resource elements in the resource grid at the given port and symbol using a mask to
  /// indicate which subcarriers are allocated and which are not.
  ///
  /// \param[in] port    Port index.
  /// \param[in] l       Symbol index.
  /// \param[in] k_init  Initial subcarrier index.
  /// \param[in] mask    Boolean mask denoting the subcarriers to be written (if \c true), starting from \c k_init.
  /// \param[in] symbols Symbols to be written into the resource grid.
  /// \return A view to the unused entries of \c symbols.
  /// \note The number of elements of \c mask shall be equal to or greater than the resource grid number of subcarriers.
  /// \note The number of elements of \c symbols shall be equal to or greater than the number of true elements in
  /// \c mask.
  virtual span<const cf_t>
  put(unsigned port, unsigned l, unsigned k_init, span<const bool> mask, span<const cf_t> symbols) = 0;

  /// \brief Puts a number of resource elements in the resource grid at the given port and symbol using a bounded bitset
  /// to indicate which subcarriers are allocated and which are not.
  ///
  /// \param[in] port    Port index.
  /// \param[in] l       Symbol index.
  /// \param[in] k_init  Initial subcarrier index.
  /// \param[in] mask    Bitset denoting the subcarriers to be written (if \c true), starting from \c k_init.
  /// \param[in] symbols Symbols to be written into the resource grid.
  /// \return A view to the unused entries of \c symbols.
  /// \note The number of elements of \c mask shall be equal to or greater than the resource grid number of subcarriers.
  /// \note The number of elements of \c symbols shall be equal to or greater than the number of true elements in
  /// \c mask.
  virtual span<const cf_t> put(unsigned                            port,
                               unsigned                            l,
                               unsigned                            k_init,
                               const bounded_bitset<NRE * MAX_RB>& mask,
                               span<const cf_t>                    symbols) = 0;

  /// \brief Puts a consecutive number of resource elements for the given \c port and symbol \c l, starting at \c
  /// k_init.
  ///
  /// \param[in] port    Port index.
  /// \param[in] l       Symbol index.
  /// \param[in] k_init  Initial subcarrier index.
  /// \param[in] symbols Symbols to be written into the resource grid.
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
  /// If zeros are written using any \c put method, the port is considered not empty.
  ///
  /// An invalid port index triggers an assertion.
  ///
  /// \param[in] port Port index.
  /// \return True if the given port is empty. Otherwise, false.
  virtual bool is_empty(unsigned port) const = 0;

  /// \brief Gets a number of elements from the grid for a given port according to a list of coordinates.
  ///
  /// \param[out] symbols     Destination symbol buffer.
  /// \param[in]  port        Port index.
  /// \param[in]  coordinates List of grid symbol&ndash;subcarrier coordinates.
  /// \note The number of elements in \c coordinates and \c symbols shall be the same.
  virtual void get(span<cf_t> symbols, unsigned port, span<const resource_grid_coordinate> coordinates) const = 0;

  /// \brief Gets a number of resource elements in the resource grid at the given port and symbol using a mask to
  /// indicate which subcarriers are allocated and which are not.
  ///
  /// \param[out] symbols Destination symbol buffer.
  /// \param[in]  port    Port index.
  /// \param[in]  l       Symbol index.
  /// \param[in]  k_init  Initial subcarrier index.
  /// \param[in] mask     Boolean mask denoting the subcarriers to be read (if \c true), starting from \c k_init.
  /// \return A view to the unused entries of \c symbols.
  /// \note The number of elements of \c mask shall be equal to or greater than the resource grid number of subcarriers.
  /// \note The number of elements of \c symbol shall be equal to or greater than the number of true elements in
  /// \c mask.
  virtual span<cf_t>
  get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init, span<const bool> mask) const = 0;

  /// \brief Gets a number of resource elements in the resource grid at the given port and symbol using a bounded bitset
  /// to indicate which subcarriers are allocated and which are not.
  ///
  /// \param[out] symbols Destination symbol buffer.
  /// \param[in]  port    Port index.
  /// \param[in]  l       Symbol index.
  /// \param[in]  k_init  Initial subcarrier index.
  /// \param[in] mask     Boolean mask denoting the subcarriers to be read (if \c true), starting from \c k_init.
  /// \return A view to the unused entries of \c symbols.
  /// \note The number of elements of \c mask shall be equal to or greater than the resource grid number of subcarriers.
  /// \note The number of elements of \c symbol shall be equal to or greater than the number of true elements in
  /// \c mask.
  virtual span<cf_t> get(span<cf_t>                          symbols,
                         unsigned                            port,
                         unsigned                            l,
                         unsigned                            k_init,
                         const bounded_bitset<MAX_RB * NRE>& mask) const = 0;

  /// \brief Gets a consecutive number of resource elements for a given port and symbol \c l starting at \c k_init.
  ///
  /// \param[out] symbols Destination symbol buffer.
  /// \param[in]  port    Port index.
  /// \param[in]  l       Symbol index.
  /// \param[in]  k_init  Initial subcarrier index.
  /// \note The sum of \c k_init and the number of elements in \c symbols shall not exceed the resource grid number of
  /// subcarriers.
  virtual void get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init) const = 0;
};

/// Describes a resource grid class with writer and reader interfaces.
class resource_grid : public resource_grid_writer, public resource_grid_reader
{
public:
  /// Sets all resource elements in the grid to zero.
  virtual void set_all_zero() = 0;
};

} // namespace srsgnb
