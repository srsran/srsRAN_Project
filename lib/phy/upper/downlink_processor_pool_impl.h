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
#include "srsran/phy/upper/downlink_processor.h"

namespace srsran {

/// Describes all downlink processors in a pool.
struct downlink_processor_pool_impl_config {
  /// Downlink processors for a given sector and numerology.
  struct sector_dl_processor {
    /// Radio sector identifier.
    unsigned sector;
    /// Subcarrier spacing.
    subcarrier_spacing scs;
    /// Pointers to the actual downlink processors.
    std::vector<std::unique_ptr<downlink_processor>> procs;
  };

  /// Collection of all downlink processors, organized by radio sector and numerology.
  std::vector<sector_dl_processor> procs;
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
  /// Container for downlink processors. Each entry belongs to a different sector.
  std::vector<processor_pool_repository<downlink_processor>> processors;
};

} // namespace srsran
