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

#include "srsgnb/ran/slot_point.h"
#include "srsgnb/ran/subcarrier_spacing.h"
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
    srsgnb_assert(numerology < NOF_NUMEROLOGIES, "Invalid numerology ({}).", numerology);

    processor_pool& pool = numerologies[numerology];
    srsgnb_assert(!pool.empty(), "Numerology ({}) has no processors.", numerology);

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
    srsgnb_assert(numerology < NOF_NUMEROLOGIES, "Invalid numerology ({}).", numerology);

    processor_pool& pool = numerologies[numerology];
    srsgnb_assert(pool.empty(), "Numerology ({}) already has processors.", numerology);

    numerologies[numerology] = std::move(obj);
  }
};

} // namespace srsran
