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

#include "srsran/adt/optional.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/slot_point.h"
#include "srsran/ran/srs/srs_constants.h"
#include "srsran/ran/srs/srs_resource_configuration.h"
#include <cstdint>

namespace srsran {

/// \brief Parameters required to receive the Sounding Reference Signals described in 3GPP TS38.211 Section 6.4.1.4.
///
/// Higher layer parameters mentioned in this structure are contained in the 3GPP TS38.331 Section 6.3.2 Information
/// Element \e SRS-Resource.
struct srs_estimator_configuration {
  /// Numerology, SFN and slot index.
  slot_point slot;
  /// SRS resource configuration.
  srs_resource_configuration resource;
  /// Receive port list.
  static_vector<uint8_t, srs_constants::max_nof_rx_ports> ports;
};

} // namespace srsran
