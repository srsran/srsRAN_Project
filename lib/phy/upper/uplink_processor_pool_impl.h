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
#include "srsgnb/phy/upper/uplink_processor.h"

namespace srsgnb {

/// Defines the structure to configure the uplink processor pool.
struct uplink_processor_pool_impl_config {
  /// Helper structure that defines a set of uplink processors for a sector and numerology.
  struct sector_ul_processor {
    /// Sector identifier.
    unsigned sector;
    /// Subcarrier spacing.
    subcarrier_spacing scs;
    /// Vector of uplink processors for this sector and numerology.
    std::vector<std::unique_ptr<uplink_processor>> procs;
  };

  /// Vector of \c info objects, which contains the uplink processors for a given sector and numerology.
  std::vector<sector_ul_processor> procs;
  /// Number of sector that will support this configuration.
  unsigned num_sectors;
};

/// Uplink processor pool implementation.
class uplink_processor_pool_impl : public uplink_processor_pool
{
public:
  /// \brief Constructs an uplink processor pool with the given configuration.
  explicit uplink_processor_pool_impl(uplink_processor_pool_impl_config dl_processors);

  // See interface for documentation.
  uplink_processor& get_processor(slot_point slot, unsigned sector_id) override;

private:
  /// Container for uplink processors. Each entry belongs to a different sector.
  std::vector<pool_procesor_sector<uplink_processor>> processors;
};

} // namespace srsgnb
