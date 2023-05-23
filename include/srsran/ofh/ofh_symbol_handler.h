/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/slot_point.h"

namespace srsran {
namespace ofh {

/// \brief Structure storing the transmission window timing parameters.
struct du_tx_window_timing_parameters {
  /// Offset from the current OTA symbol to the start of DL Control-Plane transmission window.
  std::chrono::microseconds T1a_max_cp_dl;
  /// Offset from the current OTA symbol to the end of DL Control-Plane transmission window.
  std::chrono::microseconds T1a_min_cp_dl;
  /// Offset from the current OTA symbol to the start of UL Control-Plane transmission window.
  std::chrono::microseconds T1a_max_cp_ul;
  /// Offset from the current OTA symbol to the end of UL Control-Plane transmission window.
  std::chrono::microseconds T1a_min_cp_ul;
  /// Offset from the current OTA symbol to the start of DL User-Plane transmission window.
  std::chrono::microseconds T1a_max_up;
  /// Offset from the current OTA symbol to the end of DL User-Plane transmission window.
  std::chrono::microseconds T1a_min_up;
};

/// Configuration used by ofh_symbol_handler implementations.
struct symbol_handler_config {
  /// Transmission window timing parameters for delay management.
  du_tx_window_timing_parameters tx_timing_params;
  /// Number of symbols per slot.
  unsigned symbols_per_slot;
  /// Highest subcarrier spacing.
  subcarrier_spacing scs;
};

/// \brief Open Fronthaul interface to handle symbol-based timing events.
class symbol_handler
{
public:
  /// Default destructor.
  virtual ~symbol_handler() = default;

  /// \brief Callback function called on a new OFDM symbol timing event.
  /// \param[in] slot_point Current slot information (including slot, subframe, frame and sfn).
  /// \param[in] symbol     Symbol index inside the current slot indicated by \c slot_point.
  virtual void handle_new_symbol(slot_point slot, unsigned symbol) = 0;
};

} // namespace ofh
} // namespace srsran
