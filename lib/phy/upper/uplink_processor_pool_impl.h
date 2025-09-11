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
#include "srsran/adt/circular_array.h"
#include "srsran/phy/upper/uplink_pdu_slot_repository.h"
#include "srsran/phy/upper/uplink_processor.h"
#include "srsran/phy/upper/uplink_slot_processor.h"

namespace srsran {

/// Defines the structure to configure the uplink processor pool.
struct uplink_processor_pool_impl_config {
  /// Set of uplink processors for a numerology.
  struct uplink_processor_set {
    /// Subcarrier spacing.
    subcarrier_spacing scs;
    /// Vector of uplink processors for this numerology.
    std::vector<std::unique_ptr<uplink_processor>> procs;
  };

  /// Vector of \c info objects, which contains the uplink processors for a given numerology.
  std::vector<uplink_processor_set> procs;
  /// Default uplink processor used for handling PRACH and receive symbols when no processor has been assigned.
  std::unique_ptr<uplink_processor> default_processor;
};

/// Uplink processor pool implementation.
class uplink_processor_pool_impl : public uplink_processor_pool,
                                   private uplink_slot_processor_pool,
                                   private uplink_pdu_slot_repository_pool
{
public:
  /// \brief Constructs an uplink processor pool with the given configuration.
  explicit uplink_processor_pool_impl(uplink_processor_pool_impl_config dl_processors);

  // See uplink_processor_pool interface for documentation.
  uplink_slot_processor_pool& get_slot_processor_pool() override { return *this; }

  // See uplink_processor_pool interface for documentation.
  uplink_pdu_slot_repository_pool& get_slot_pdu_repository_pool() override { return *this; }

  // See uplink_pdu_slot_repository_pool interface for documentation.
  void stop() override;

private:
  /// Maximum number of simultaneous assigned processors.
  static constexpr unsigned nof_assigned_processors = 16;

  // See uplink_slot_processor_pool interface for documentation.
  uplink_slot_processor& get_slot_processor(slot_point slot) override;

  // See uplink_pdu_slot_repository_pool interface for documentation.
  unique_uplink_pdu_slot_repository get_pdu_slot_repository(slot_point slot) override;

  /// Repository of uplink processors.
  processor_pool_repository<uplink_processor> processors;
  /// Default uplink processor used for handling PRACH and receive symbols when no processor has been assigned.
  std::unique_ptr<uplink_processor> default_processor;
  /// Circular uplink processor assignation.
  circular_array<std::atomic<uplink_processor*>, nof_assigned_processors> assigned_processors = {};
};

} // namespace srsran
