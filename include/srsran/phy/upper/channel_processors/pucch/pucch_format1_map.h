/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/support/srsran_assert.h"
#include <array>
#include <optional>

namespace srsran {

/// \brief Maps elements of type \c T to the corresponding PUCCH Format 1 initial cyclic shift and time domain OCC.
///
/// The values of the map are treated as optional, and are set to \c nullopt until the key is specifically mapped to a
/// \c T value.
template <typename T>
class pucch_format1_map
{
public:
  /// \brief Returns \c true if the map has an entry corresponding to the given initial cyclic shift and time domain
  /// OCC.
  /// \remark An assertion is triggered if the given initial cyclic shift or the time-domain OCC are out of their
  /// ranges.
  bool has_value(unsigned initial_cyclic_shift, unsigned time_domain_occ) const
  {
    srsran_assert(pucch_constants::format1_initial_cyclic_shift_range.contains(initial_cyclic_shift),
                  "Initial cyclic shift (i.e., {}) is out of the range {}.",
                  initial_cyclic_shift,
                  pucch_constants::format1_initial_cyclic_shift_range);
    srsran_assert(pucch_constants::format1_time_domain_occ_range.contains(time_domain_occ),
                  "Time-domain OCC (i.e., {}) is out of the range {}.",
                  time_domain_occ,
                  pucch_constants::format1_time_domain_occ_range);
    return map[initial_cyclic_shift][time_domain_occ].has_value();
  }

  /// \brief Emplace an entry in the map.
  /// \remark An assertion is triggered if the given initial cyclic shift or the time-domain OCC are out of their
  /// ranges.
  void emplace(unsigned initial_cyclic_shift, unsigned time_domain_occ, const T& value)
  {
    srsran_assert(pucch_constants::format1_initial_cyclic_shift_range.contains(initial_cyclic_shift),
                  "Initial cyclic shift (i.e., {}) is out of the range {}.",
                  initial_cyclic_shift,
                  pucch_constants::format1_initial_cyclic_shift_range);
    srsran_assert(pucch_constants::format1_time_domain_occ_range.contains(time_domain_occ),
                  "Time-domain OCC (i.e., {}) is out of the range {}.",
                  time_domain_occ,
                  pucch_constants::format1_time_domain_occ_range);
    map[initial_cyclic_shift][time_domain_occ].emplace(value);
  }

  /// \brief Gets a modifiable entry in the map.
  /// \remark An assertion is triggered if the given initial cyclic shift or the time-domain OCC are out of their
  /// ranges.
  /// \remark An assertion is triggered if the entry does not contain a value.
  T& get(unsigned initial_cyclic_shift, unsigned time_domain_occ)
  {
    srsran_assert(pucch_constants::format1_initial_cyclic_shift_range.contains(initial_cyclic_shift),
                  "Initial cyclic shift (i.e., {}) is out of the range {}.",
                  initial_cyclic_shift,
                  pucch_constants::format1_initial_cyclic_shift_range);
    srsran_assert(pucch_constants::format1_time_domain_occ_range.contains(time_domain_occ),
                  "Time-domain OCC (i.e., {}) is out of the range {}.",
                  time_domain_occ,
                  pucch_constants::format1_time_domain_occ_range);
    srsran_assert(has_value(initial_cyclic_shift, time_domain_occ), "The map does not contain the given entry.");
    return map[initial_cyclic_shift][time_domain_occ].value();
  }

  /// \brief Gets a constant entry in the map.
  /// \remark An assertion is triggered if the given initial cyclic shift or the time-domain OCC are out of their
  /// ranges.
  /// \remark An assertion is triggered if the entry does not contain a value.
  const T& get(unsigned initial_cyclic_shift, unsigned time_domain_occ) const
  {
    srsran_assert(pucch_constants::format1_initial_cyclic_shift_range.contains(initial_cyclic_shift),
                  "Initial cyclic shift (i.e., {}) is out of the range {}.",
                  initial_cyclic_shift,
                  pucch_constants::format1_initial_cyclic_shift_range);
    srsran_assert(pucch_constants::format1_time_domain_occ_range.contains(time_domain_occ),
                  "Time-domain OCC (i.e., {}) is out of the range {}.",
                  time_domain_occ,
                  pucch_constants::format1_time_domain_occ_range);
    srsran_assert(has_value(initial_cyclic_shift, time_domain_occ), "The map does not contain the given entry.");
    return map[initial_cyclic_shift][time_domain_occ].value();
  }

  /// Clears the current contents of the map.
  void clear() { map = {}; }

private:
  /// Internal storage.
  std::array<std::array<std::optional<T>, pucch_constants::format1_time_domain_occ_range.stop()>,
             pucch_constants::format1_initial_cyclic_shift_range.stop()>
      map;
};

} // namespace srsran
