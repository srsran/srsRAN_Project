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

#include "srsran/ofh/ofh_ota_symbol_handler.h"
#include <atomic>

namespace srsran {
namespace ofh {

/// Open Fronthaul transmission window checker.
class tx_window_checker : public ota_symbol_handler
{
public:
  tx_window_checker(srslog::basic_logger& logger_,
                    uint32_t              advance_time_in_symbols_,
                    uint32_t              nof_symbols_,
                    uint32_t              numerology_) :
    logger(logger_),
    advance_time_in_symbols(advance_time_in_symbols_),
    nof_symbols(nof_symbols_),
    numerology(numerology_)
  {
  }

  // See interface for documentation.
  void handle_new_ota_symbol(slot_symbol_point symbol_point) override { count_val = symbol_point.system_slot(); }

  /// Returns true if the given slot is already late compared to the current OTA time, otherwise false.
  bool is_late(slot_point slot) const
  {
    slot_symbol_point ota_symbol_point(numerology, count_val, nof_symbols);

    // Use symbol 0 as the worst case for the resource grid slot.
    slot_symbol_point rg_point(slot, 0, nof_symbols);
    rg_point -= advance_time_in_symbols;

    if (ota_symbol_point < rg_point) {
      return false;
    }

    logger.warning(
        "Detected late downlink request in slot={}_{}, current ota_slot={}_{}, processing time takes symbols={}",
        slot,
        0,
        ota_symbol_point.get_slot(),
        ota_symbol_point.get_symbol_index(),
        advance_time_in_symbols);

    return true;
  }

private:
  srslog::basic_logger& logger;
  const uint32_t        advance_time_in_symbols;
  const uint32_t        nof_symbols;
  const uint32_t        numerology;
  std::atomic<uint32_t> count_val;
};

} // namespace ofh
} // namespace srsran
