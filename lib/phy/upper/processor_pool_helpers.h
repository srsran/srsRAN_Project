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

#include "srsran/ran/slot_point.h"
#include "srsran/ran/subcarrier_spacing.h"
#include <array>

namespace srsran {

/// \brief Repository of processor pools.
///
/// Stores channel processor pools, organized according to the underlying numerology.
/// \tparam T Type of the processor.
template <typename T>
class processor_pool_repository
{
  /// \brief Container of processors.
  ///
  /// Each entry corresponds to a different slot.
  using processor_pool = std::vector<std::unique_ptr<T>>;

  /// \brief Container of processor pools.
  ///
  /// Each entry corresponds to a different numerology.
  std::array<processor_pool, NOF_NUMEROLOGIES> numerologies;

public:
  /// \brief Returns a reference to a processor using the given slot.
  T& get_processor(slot_point slot)
  {
    unsigned numerology = slot.numerology();
    srsran_assert(numerology < NOF_NUMEROLOGIES, "Invalid numerology ({}).", numerology);

    processor_pool& pool = numerologies[numerology];
    srsran_assert(!pool.empty(), "Numerology ({}) has no processors.", numerology);

    unsigned index = slot.system_slot() % pool.size();
    return *pool[index];
  }

  /// \brief Associates the given processor pool with this repository.
  ///
  /// \param[in] scs Subcarrier spacing.
  /// \param[in] obj New processor pool for the given numerology.
  void insert(subcarrier_spacing scs, processor_pool&& obj)
  {
    unsigned numerology = to_numerology_value(scs);
    srsran_assert(numerology < NOF_NUMEROLOGIES, "Invalid numerology ({}).", numerology);

    processor_pool& pool = numerologies[numerology];
    srsran_assert(pool.empty(), "Numerology ({}) already has processors.", numerology);

    numerologies[numerology] = std::move(obj);
  }
};

} // namespace srsran
