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
#include "srsran/phy/upper/uplink_processor.h"

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
class uplink_processor_pool_impl : public uplink_processor_pool
{
public:
  /// \brief Constructs an uplink processor pool with the given configuration.
  explicit uplink_processor_pool_impl(uplink_processor_pool_impl_config dl_processors);

  // See interface for documentation.
  uplink_processor& get_processor(slot_point slot) override;

private:
  processor_pool_repository<uplink_processor> processors;
};

} // namespace srsran
