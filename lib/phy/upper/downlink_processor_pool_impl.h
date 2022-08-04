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

#include "processor_pool_helpers.h"
#include "srsgnb/phy/upper/downlink_processor.h"

namespace srsgnb {

/// Describes all downlink processors in a pool.
struct downlink_processor_pool_impl_config {
  /// Downlink processors for a given sector and numerology.
  struct info {
    /// Radio sector identifier.
    unsigned sector;
    /// Numerology index.
    unsigned numerology;
    /// Pointers to the actual downlink processors.
    std::vector<std::unique_ptr<downlink_processor>> procs;
  };

  /// Collection of all downlink processors, organized by radio sector and numerology.
  std::vector<info> procs;
  /// Number of radio sectors.
  unsigned num_sectors;
};

/// Implementation of a downlink processor pool.
class downlink_processor_pool_impl : public downlink_processor_pool
{
public:
  /// \brief Constructs a downlink processor pool using the given configuration.
  ///
  /// \param[in] dl_processors Pool configuration.
  explicit downlink_processor_pool_impl(downlink_processor_pool_impl_config dl_processors);

  // See interface for documentation.
  downlink_processor& get_processor(slot_point slot, unsigned sector_id) override;

private:
  /// Collection of downlink processors managed by the pool.
  std::vector<pool_procesor_sector<downlink_processor>> processors;
};

} // namespace srsgnb
