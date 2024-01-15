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

#include "processor_pool_helpers.h"
#include "srsran/phy/upper/uplink_processor.h"

namespace srsran {

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
  std::vector<processor_pool_repository<uplink_processor>> processors;
};

} // namespace srsran
