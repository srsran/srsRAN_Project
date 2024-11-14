/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/support/resource_grid_base.h"

namespace srsran {

/// \brief Resource grid writer interface.
///
/// Contains the necessary functions to write resource elements in a resource grid.
///
/// \remark All the methods contained in this interface must not result in writing outside of the resource grid region.
class resource_grid_writer : public resource_grid_base
{
public:
  /// Default destructor
  virtual ~resource_grid_writer() = default;

  /// \brief Puts a number of resource elements in the resource grid at the given port and symbol using a bounded bitset
  /// to indicate which subcarriers are allocated and which are not.
  ///
  /// \param[in] port    Port index.
  /// \param[in] l       Symbol index.
  /// \param[in] k_init  Initial subcarrier index.
  /// \param[in] mask    Bitset denoting the subcarriers to be written (if \c true), starting from \c k_init.
  /// \param[in] symbols Symbols to be written into the resource grid.
  /// \return A view to the unused entries of \c symbols.
  /// \note The number of elements of \c mask shall be equal to or lower than the resource grid number of subcarriers.
  /// \note The number of elements of \c symbols shall be equal to or greater than the number of true elements in
  /// \c mask.
  virtual span<const cf_t> put(unsigned                            port,
                               unsigned                            l,
                               unsigned                            k_init,
                               const bounded_bitset<NRE * MAX_RB>& mask,
                               span<const cf_t>                    symbols) = 0;

  /// \brief Puts a number of resource elements in the resource grid at the given port and symbol using a bounded bitset
  /// to indicate which subcarriers are allocated and which are not.
  ///
  /// \param[in] port    Port index.
  /// \param[in] l       Symbol index.
  /// \param[in] k_init  Initial subcarrier index.
  /// \param[in] mask    Bitset denoting the subcarriers to be written (if \c true), starting from \c k_init.
  /// \param[in] symbols Symbols to be written into the resource grid.
  /// \return A view to the unused entries of \c symbols.
  /// \note The number of elements of \c mask shall be equal to or lower than the resource grid number of subcarriers.
  /// \note The number of elements of \c symbols shall be equal to or greater than the number of true elements in
  /// \c mask.
  virtual span<const cbf16_t> put(unsigned                            port,
                                  unsigned                            l,
                                  unsigned                            k_init,
                                  const bounded_bitset<NRE * MAX_RB>& mask,
                                  span<const cbf16_t>                 symbols) = 0;

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

  /// \brief Puts a number of resource elements for the given \c port and symbol \c l, starting at \c
  /// k_init and at a distance of \c stride.
  ///
  /// \param[in] port    Port index.
  /// \param[in] l       Symbol index.
  /// \param[in] k_init  Initial subcarrier index.
  /// \param[in] stride  Distance between adjacent symbols. A stride of 1 means that the allocated REs are contiguous.
  /// \param[in] symbols Symbols to be written into the resource grid.
  /// \note The RE positions given \c k_init, the number of elements in \c symbols and the \c stride shall be within the
  /// resource grid number of subcarriers.
  virtual void put(unsigned port, unsigned l, unsigned k_init, unsigned stride, span<const cbf16_t> symbols) = 0;

  /// \brief Gets a read-write view of an OFDM symbol for a given port.
  ///
  /// \param[in] port Port index.
  /// \param[in] l    OFDM symbol index.
  /// \return Resource grid view.
  virtual span<cbf16_t> get_view(unsigned port, unsigned l) = 0;
};

} // namespace srsran
