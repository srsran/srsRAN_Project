/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
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

namespace srsgnb {

/// \brief Represents a sector in the pool of processor.
template <typename T>
class pool_procesor_sector
{
  /// Maximum number of supported numerologies.
  static constexpr unsigned MAX_NUM_NUMEROLOGIES = 5U;

  /// Container of processors<T>. Each entry belongs to a different slot.
  using processor_pool = std::vector<std::unique_ptr<T>>;

  /// Container of processor pools. Each entry belongs to a different subcarrier spacing.
  std::array<processor_pool, MAX_NUM_NUMEROLOGIES> numerologies;

public:
  /// \brief Returns a reference to the processor using the given slot.
  T& get_processor(slot_point slot)
  {
    unsigned        numerology = slot.numerology();
    processor_pool& pool       = numerologies[numerology];

    srsgnb_assert(!pool.empty(), "Invalid subcarrier spacing ({}).", numerology);

    unsigned index = slot.system_slot() % pool.size();
    return *pool[index];
  }

  /// \brief Associates the given processor pool with this sector.
  ///
  /// \param[in] scs Subcarrier spacing.
  /// \param[in] obj New processor pool for the current sector.
  void insert(subcarrier_spacing scs, processor_pool&& obj)
  {
    unsigned numerology = to_numerology_value(scs);
    srsgnb_assert(numerology < MAX_NUM_NUMEROLOGIES, "Invalid Subcarrier spacing ({}).", numerology);

    processor_pool& pool = numerologies[numerology];
    srsgnb_assert(pool.empty(), "Subcarrier spacing ({}) already present in the pool.", numerology);

    numerologies[numerology] = std::move(obj);
  }
};

} // namespace srsgnb
