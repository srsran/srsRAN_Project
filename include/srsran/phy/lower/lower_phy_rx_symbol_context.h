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

#include "srsran/ran/slot_point.h"

namespace srsran {

/// Describes the context of a newly received symbol.
struct lower_phy_rx_symbol_context {
  /// Slot context.
  slot_point slot;
  /// Radio sector identifier.
  unsigned sector;
  /// The last processed symbol index within the slot.
  unsigned nof_symbols;
};

} // namespace srsran
