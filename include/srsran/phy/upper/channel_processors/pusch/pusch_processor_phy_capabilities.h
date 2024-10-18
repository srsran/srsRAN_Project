/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

/// \file
/// \brief Declaration of the physical layer PUSCH processor capabilities.

namespace srsran {

/// Physical layer PUSCH processor capabilities.
struct pusch_processor_phy_capabilities {
  /// Maximum number of layers.
  unsigned max_nof_layers;
};

/// Gets the physical layer PUSCH processor capabilities.
pusch_processor_phy_capabilities get_pusch_processor_phy_capabilities();

} // namespace srsran