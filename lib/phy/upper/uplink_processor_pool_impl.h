/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "processor_pool_helpers.h"
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
  uplink_pdu_slot_repository_pool& get_slot_pdu_repository() override { return *this; }

private:
  // See uplink_slot_processor_pool interface for documentation.
  uplink_slot_processor& get_slot_processor(slot_point slot) override;

  // See uplink_pdu_slot_repository_pool interface for documentation.
  unique_uplink_pdu_slot_repository get_pdu_slot_repository(slot_point slot) override;

  /// Repository of uplink processors.
  processor_pool_repository<uplink_processor> processors;
};

} // namespace srsran
