/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_PHY_UPPER_DOWNLINK_PROCESSOR_POOL_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_DOWNLINK_PROCESSOR_POOL_IMPL_H

#include "srsgnb/adt/circular_map.h"
#include "srsgnb/phy/upper/downlink_processor.h"

namespace srsgnb {

/// Defines the structure to configure the downlink processor pool.
struct downlink_processor_pool_config_impl {
  struct info {
    unsigned                                          sector;
    unsigned                                          numerology;
    std::vector<std::unique_ptr<downlink_processor> > procs;
  };

  std::vector<info> procs;
  unsigned          num_sectors;
};

/// Implementation of a downlink processor pool.
class downlink_processor_pool_impl : public downlink_processor_pool
{
  static constexpr unsigned MAX_NUM_NUMEROLOGIES = 5U;

  using processor_pool       = std::vector<std::unique_ptr<downlink_processor> >;
  using numerology_container = circular_map<uint32_t, processor_pool, MAX_NUM_NUMEROLOGIES>;

  /// \brief Represents a sector in the container of the pool.
  class sector
  {
    const unsigned       sector_id;
    numerology_container numerologies;

  public:
    /// \brief Default constructor.
    sector() : sector_id(-1) {}

    /// \brief Builds a sector with the given sector ID and numerology container.
    ///
    /// \param[in] sector_id Sector identifier.
    /// \param[in] cont Contains the numerology container to store.
    sector(unsigned sector_id, numerology_container cont) : sector_id(sector_id), numerologies(std::move(cont)) {}

    /// \brief Returns the vector of downlink processor associated to the given numerology.
    ///
    /// \param numerology Numerology to find in the map.
    /// \returns The processor pool associated to the given numerology.
    processor_pool& operator[](unsigned numerology)
    {
      auto iter = numerologies.find(numerology);
      srsran_assert(iter != numerologies.end(), "Invalid numerology ({}) in sector ({}).", numerology, sector_id);

      return iter->second;
    }

    /// \brief Inserts the given key and object into the container.
    ///
    /// \param[in] key Key to insert.
    /// \param[in] obj Object to insert.
    void insert(unsigned key, processor_pool&& obj) { numerologies.insert(key, std::move(obj)); }
  };

public:
  /// \brief Builds a downlink processor pool using the given configuration.
  ///
  /// \param[in] dl_processors Contains the configuration for the pool.
  explicit downlink_processor_pool_impl(downlink_processor_pool_config_impl dl_processors);

  // See interface for documentation.
  downlink_processor& get_processor(const slot_point& slot, unsigned sector_id) override;

private:
  std::vector<sector> processors;
};
} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_DOWNLINK_PROCESSOR_POOL_IMPL_H
