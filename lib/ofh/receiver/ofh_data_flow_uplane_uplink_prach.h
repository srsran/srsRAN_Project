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

#include "srsran/adt/span.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul User-Plane uplink PRACH data flow.
class data_flow_uplane_uplink_prach
{
public:
  /// Default destructor.
  virtual ~data_flow_uplane_uplink_prach() = default;

  /// Decodes the given Open Fronthaul message associated to the given eAxC.
  virtual void decode_type1_message(unsigned eaxc, span<const uint8_t> message) = 0;
};

} // namespace ofh
} // namespace srsran
