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

#include <chrono>
#include <cmath>

namespace srsran {
namespace ofh {

/// Structure storing the transmission window timing parameters expressed in a number of symbols.
struct tx_window_timing_parameters {
  /// Offset from the current OTA symbol to the first symbol at which DL Control-Plane message can be sent, or in
  /// other words it is the offset to the start of DL Control-Plane transmission window. Must be calculated based on
  /// \c T1a_max_cp_dl parameter.
  unsigned sym_cp_dl_start;
  /// Offset from the current OTA symbol to the last symbol at which DL Control-Plane message can be sent within its
  /// transmission window. Must be calculated based on \c T1a_min_cp_dl parameter.
  unsigned sym_cp_dl_end;
  /// Offset from the current OTA symbol to the first symbol at which UL Control-Plane message can be sent within its
  /// transmission window. Must be calculated based on \c T1a_max_cp_ul parameter.
  unsigned sym_cp_ul_start;
  /// Offset from the current OTA symbol to the last symbol at which UL Control-Plane message can be sent within its
  /// transmission window. Must be calculated based on \c T1a_min_cp_ul parameter.
  unsigned sym_cp_ul_end;
  /// Offset from the current OTA symbol to the first symbol at which DL User-Plane message can be sent within its
  /// transmission window. Must be calculated based on \c T1a_max_up parameter.
  unsigned sym_up_dl_start;
  /// Offset from the current OTA symbol to the last symbol at which DL User-Plane message can be sent within its
  /// transmission window. Must be calculated based on \c T1a_min_up parameter.
  unsigned sym_up_dl_end;
};

} // namespace ofh
} // namespace srsran
