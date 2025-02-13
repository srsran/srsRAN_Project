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

#include "srsran/adt/byte_buffer.h"
#include "srsran/support/timers.h"

namespace srsran {

/// Helper structure used to pass and store TX PDUs/SDUs along the PDCP TX entity, the security engine and post
/// processing.
struct pdcp_tx_xdu_info {
  /// The SDU/PDU buffer (PDU header/footer are added during processing.)
  byte_buffer buf;
  /// The count value of the SDU/PDU
  uint32_t count;
  /// Time of arrival at the PDCP entity
  std::chrono::system_clock::time_point time_of_arrival;
  /// Tick point of arrival at the PDCP entity
  tick_point_t tick_point_of_arrival;
};

} // namespace srsran
