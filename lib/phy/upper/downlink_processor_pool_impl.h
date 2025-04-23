/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
  /// Downlink processors for a given numerology.
  struct sector_dl_processor {
    /// Subcarrier spacing.
    subcarrier_spacing scs;
    /// Pointers to the actual downlink processors.
    std::vector<std::unique_ptr<downlink_processor_base>> procs;
  };

  /// Collection of all downlink processors, organized by numerology.
  std::vector<sector_dl_processor> procs;
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
  downlink_processor_controller& get_processor_controller(slot_point slot) override;

  // See interface for documentation.
  void stop() override;

private:
  /// Container for downlink processors. Each entry belongs to a different sector.
  processor_pool_repository<downlink_processor_base> processors;
};

} // namespace srsran
