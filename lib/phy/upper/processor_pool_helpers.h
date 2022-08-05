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

#include "srsgnb/adt/circular_map.h"
#include "srsgnb/ran/subcarrier_spacing.h"

namespace srsgnb {

/// \brief Represents a sector in the pool processor.
template <typename T>
class pool_procesor_sector
{
public:
  using processor_pool = std::vector<std::unique_ptr<T>>;

private:
  static constexpr unsigned MAX_NUM_NUMEROLOGIES = 5U;
  using numerology_container                     = circular_map<uint32_t, processor_pool, MAX_NUM_NUMEROLOGIES>;

  const unsigned       sector_id;
  numerology_container numerologies;

public:
  /// \brief Default constructor.
  ///
  /// Sets the sector identifier to an invalid value.
  pool_procesor_sector() : sector_id(-1) {}

  /// \brief Constructs a sector with the given sector identifier and numerology container.
  ///
  /// \param[in] sector_id Sector identifier.
  /// \param[in] cont Contains the numerology container to store.
  pool_procesor_sector(unsigned sector_id, numerology_container cont) :
    sector_id(sector_id), numerologies(std::move(cont))
  {
  }

  /// \brief Returns the pool of processors associated with the given subcarrier spacing.
  processor_pool& operator[](subcarrier_spacing scs)
  {
    unsigned numerology = to_numerology_value(scs);
    auto     iter       = numerologies.find(numerology);
    srsgnb_assert(iter != numerologies.end(), "Invalid numerology ({}) in sector ({}).", numerology, sector_id);

    return iter->second;
  }

  /// \brief Associates the given processor pool with this sector.
  ///
  /// \param[in] scs Subcarrier spacing.
  /// \param[in] obj New processor pool for the current sector.
  void insert(subcarrier_spacing scs, processor_pool&& obj)
  {
    unsigned numerology = to_numerology_value(scs);
    srsgnb_assert(
        !numerologies.contains(numerology), "Subcarrier spacing ({}) already present in the pool", numerology);

    numerologies.insert(numerology, std::move(obj));
  }
};

} // namespace srsgnb
