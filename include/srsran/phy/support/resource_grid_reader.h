/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once
#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/support/resource_grid_base.h"

namespace srsran {

/// Describes a resource grid reader interface.
class resource_grid_reader : public resource_grid_base
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

  /// \brief Determines whether the resource grid is empty.
  ///
  /// The grid is considered empty if no \c put method have been called since last zero set.
  ///
  /// If zeros are written using any \c put method, the grid is considered not empty.
  ///
  /// \return \c true if the resource grid is empty. Otherwise, \c false.
  virtual bool is_empty() const = 0;

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

  /// \brief Gets a view of all resource elements for a given port and symbol \c l.
  ///
  /// \param[in]  port    Port index.
  /// \param[in]  l       Symbol index.
  virtual span<const cf_t> get_view(unsigned port, unsigned l) const = 0;
};

} // namespace srsran
