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

#include "srsgnb/ran/pucch/pucch_mapping.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// PUCCH context for Format 0 or Format 1.
struct ul_pucch_f0_f1_context {
  /// Scheduling request opportunity indicator.
  bool is_sr_opportunity;
};

/// PUCCH context.
struct ul_pucch_context {
  /// Slot description (also specifies the numerology).
  slot_point slot;
  /// Radio Network Temporary Identifier (RNTI).
  rnti_t rnti;
  /// PUCCH format type.
  pucch_format format;
  /// Format 0 or Format 1 context.
  optional<ul_pucch_f0_f1_context> context_f0_f1;
};

} // namespace srsgnb
