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

#include "srsran/ran/pucch/pucch_mapping.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

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
  std::optional<ul_pucch_f0_f1_context> context_f0_f1;
};

/// Sounding Reference Signals context.
struct ul_srs_context {
  /// Slot description (also specifies the numerology).
  slot_point slot;
  /// Radio Network Temporary Identifier (RNTI).
  rnti_t rnti;
  /// Normalized channel IQ matrix report requested.
  bool is_normalized_channel_iq_matrix_report_requested;
  /// Positioning report requested.
  bool is_positioning_report_requested;
};

} // namespace srsran
