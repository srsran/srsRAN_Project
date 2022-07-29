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
  /// Slot context within the system frame.
  slot_point slot;
  /// Provides the sector identifier.
  // todo(): can we move sector first, so that slot and symbol are close to one another?
  unsigned sector;
  /// Index of the first symbol of the PRACH reception window from the given slot.
  // todo(): form the given slot? isn't it within the given slot?
  unsigned start_symbol;
  /// Number of slots to read.
  // todo(): slots or symbols?
  unsigned window_length;
};

} // namespace srsgnb
