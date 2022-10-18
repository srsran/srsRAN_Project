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

/// PUCCH context.
struct ul_pucch_context {
  /// Slot description (also specifies the numerology).
  slot_point slot;
  /// Radio Network Temporary Identifier (RNTI).
  rnti_t rnti;
  /// PUCCH format type.
  pucch_format format;
  /// Scheduling request opportunity indicator.
  bool is_sr_opportunity;
  /// Number of HARQ.
  unsigned nof_harq;
};

} // namespace srsgnb
