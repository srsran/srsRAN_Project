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

#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Describes a PRACH buffer context.
struct prach_buffer_context {
  /// Provides the slot context within the system frame.
  slot_point slot;
  /// Provides the sector identifier.
  unsigned sector;
  /// Indicates the index of the first symbol of the PRACH reception window from the given slot.
  unsigned start_symbol;
  /// Indicates the number of slots to read.
  unsigned window_length;
};

} // namespace srsgnb
