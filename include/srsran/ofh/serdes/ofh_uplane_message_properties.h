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

#include "srsran/ofh/compression/compression_params.h"
#include "srsran/ofh/serdes/ofh_message_properties.h"
#include "srsran/ran/slot_point.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul User-Plane message parameters.
struct uplane_message_params {
  /// Data direction.
  data_direction direction;
  /// Frame, subframe and slot information.
  slot_point slot;
  /// Filter index.
  filter_index_type filter_index;
  /// Initial PRB index used in the given /c symbol_id.
  unsigned start_prb;
  /// Number of PRBs in this symbol.
  unsigned nof_prb;
  /// Symbol identifier.
  unsigned symbol_id;
  /// Section type of the message to be built.
  section_type sect_type;
  /// IQ data compression parameters.
  ru_compression_params compression_params;
};

} // namespace ofh
} // namespace srsran
