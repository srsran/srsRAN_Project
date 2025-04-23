/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/adt/circular_map.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/support/srsran_assert.h"
#include <utility>

namespace srsran {

/// \brief Maps elements of type \c T to the corresponding PUCCH Format 1 initial cyclic shift and time domain OCC.
///
/// Elements can be accessed with the \c get() methods only if previously initialized with \c emplace() or \c insert().
template <typename T>
class pucch_format1_map
{
  static constexpr unsigned nof_time_domain_occs      = pucch_constants::format1_time_domain_occ_range.stop();
  static constexpr unsigned nof_initial_cyclic_shifts = pucch_constants::format1_initial_cyclic_shift_range.stop();

public:
  /// \brief Read-only iterator.
  ///
  /// Traverses the active entries of the map, in increasing order of initial cyclic shifts, first, and of time domain
  /// OCC indices, second.
  class const_iterator
    : public static_circular_map<unsigned, T, nof_time_domain_occs * nof_initial_cyclic_shifts>::const_iterator
  {
    using map_iterator =
        typename static_circular_map<unsigned, T, nof_time_domain_occs * nof_initial_cyclic_shifts>::const_iterator;

  public:
    /// Return type of the indirected iterator: coordinates and reference to the content of the map.
    struct obj_t {
      unsigned initial_cyclic_shift;
      unsigned time_domain_occ;
      const T& value;
    };

    /// Indirection operator.
    obj_t operator*() const
    {
      auto&    tmp = map_iterator::operator*();
      unsigned ix  = tmp.first;
      return {.initial_cyclic_shift = ix % nof_initial_cyclic_shifts,
              .time_domain_occ      = ix / nof_initial_cyclic_shifts,
              .value                = tmp.second};
    }

  protected:
    friend const_iterator pucch_format1_map<T>::begin() const, pucch_format1_map<T>::cbegin() const,
        pucch_format1_map<T>::end() const, pucch_format1_map<T>::cend() const;

    /// Iterator constructor
    const_iterator(const static_circular_map<unsigned, T, nof_time_domain_occs * nof_initial_cyclic_shifts>* map,
                   size_t                                                                                    idx_) :
      map_iterator(map, idx_)
    {
    }
  };

  // Default constructor.
  pucch_format1_map() = default;

  /// \brief Checks if there is an element at the given coordinates.
  /// \remark An assertion is triggered if the given initial cyclic shift or the time-domain OCC are out of their
  /// ranges.
  bool contains(unsigned initial_cyclic_shift, unsigned time_domain_occ) const noexcept
  {
    return map.contains(convert_coords_to_index(initial_cyclic_shift, time_domain_occ));
  }

  /// \brief Constructs a new element of the map in-place.
  /// \return True if no collision was detected and the object was inserted. False, otherwise.
  /// \remark An assertion is triggered if the given initial cyclic shift or the time-domain OCC are out of their
  /// ranges.
  template <class... Args>
  bool emplace(unsigned initial_cyclic_shift, unsigned time_domain_occ, Args&&... args)
  {
    return map.emplace(convert_coords_to_index(initial_cyclic_shift, time_domain_occ), std::forward<Args>(args)...);
  }

  /// \brief Inserts a new element into the map.
  /// \return True if no collision was detected and the object was inserted. False, otherwise.
  /// \remark An assertion is triggered if the given initial cyclic shift or the time-domain OCC are out of their
  /// ranges.
  bool insert(unsigned initial_cyclic_shift, unsigned time_domain_occ, const T& value)
  {
    return map.insert(convert_coords_to_index(initial_cyclic_shift, time_domain_occ), value);
  }

  /// \brief Gets a modifiable entry in the map.
  /// \remark An assertion is triggered if the given initial cyclic shift or the time-domain OCC are out of their
  /// ranges.
  /// \remark An assertion is triggered if the entry does not contain a value.
  T& get(unsigned initial_cyclic_shift, unsigned time_domain_occ) noexcept
  {
    return map[convert_coords_to_index(initial_cyclic_shift, time_domain_occ)];
  }

  /// \brief Gets a constant entry in the map.
  /// \remark An assertion is triggered if the given initial cyclic shift or the time-domain OCC are out of their
  /// ranges.
  /// \remark An assertion is triggered if the entry does not contain a value.
  const T& get(unsigned initial_cyclic_shift, unsigned time_domain_occ) const noexcept
  {
    return map[convert_coords_to_index(initial_cyclic_shift, time_domain_occ)];
  }

  /// Clears the current contents of the map.
  void clear() noexcept { map.clear(); }

  /// Returns the number of active elements of the map.
  size_t size() const noexcept { return map.size(); }

  /// Returns a const iterator to the beginning of the container.
  const_iterator begin() const { return const_iterator(&map, 0); }
  const_iterator cbegin() const { return const_iterator(&map, 0); }

  /// Returns a const iterator to the end of the container.
  const_iterator end() const { return const_iterator(&map, map.capacity()); }
  const_iterator cend() const { return const_iterator(&map, map.capacity()); }

private:
  /// Converts 2-D coordinates into linear ones.
  static unsigned convert_coords_to_index(unsigned initial_cyclic_shift, unsigned time_domain_occ)
  {
    srsran_assert(initial_cyclic_shift < nof_initial_cyclic_shifts,
                  "The given initial cyclic shift {} exceeds the maximum acceptable value {}.",
                  initial_cyclic_shift,
                  nof_initial_cyclic_shifts - 1);
    srsran_assert(time_domain_occ < nof_time_domain_occs,
                  "The given time domain OCC index {} exceeds the maximum acceptable value {}.",
                  time_domain_occ,
                  nof_time_domain_occs - 1);
    return (time_domain_occ * nof_initial_cyclic_shifts + initial_cyclic_shift);
  }

  /// Internal storage.
  static_circular_map<unsigned, T, nof_time_domain_occs * nof_initial_cyclic_shifts> map;
};

} // namespace srsran
