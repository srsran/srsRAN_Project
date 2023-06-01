/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
