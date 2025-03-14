/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier.h"
#include "srsran/srslog/srslog.h"
#include <atomic>

namespace srsran {
namespace ofh {

/// Open Fronthaul transmission window checker.
class tx_window_checker : public ota_symbol_boundary_notifier
{
public:
  tx_window_checker(srslog::basic_logger& logger_,
                    unsigned              sector_id_,
                    uint32_t              advance_time_in_symbols_,
                    uint32_t              nof_symbols_) :
    logger(logger_), sector_id(sector_id_), advance_time_in_symbols(advance_time_in_symbols_), nof_symbols(nof_symbols_)
  {
  }

  // See interface for documentation.
  void on_new_symbol(const slot_symbol_point_context& symbol_point_context) override
  {
    // This atomic is only written from a single thread.
    count_val.store(symbol_point_context.symbol_point.to_uint(), std::memory_order::memory_order_relaxed);
  }

  /// Returns true if the given slot is already late compared to the current OTA time, otherwise false.
  bool is_late(slot_point slot) const
  {
    slot_symbol_point ota_symbol_point(
        slot.numerology(), count_val.load(std::memory_order::memory_order_relaxed), nof_symbols);

    // Use symbol 0 as the worst case for the resource grid slot.
    slot_symbol_point rg_point(slot, 0, nof_symbols);
    rg_point -= advance_time_in_symbols;

    if (ota_symbol_point < rg_point) {
      return false;
    }

    logger.debug("Sector#{}: a late upper-PHY downlink request arrived to OFH in slot '{}_{}' with current "
                 "ota_slot='{}_{}', OFH processing time requires a minimum of '{}' symbols",
                 sector_id,
                 slot,
                 0,
                 ota_symbol_point.get_slot(),
                 ota_symbol_point.get_symbol_index(),
                 advance_time_in_symbols);

    return true;
  }

private:
  srslog::basic_logger& logger;
  const unsigned        sector_id;
  const uint32_t        advance_time_in_symbols;
  const uint32_t        nof_symbols;
  std::atomic<uint32_t> count_val;
};

} // namespace ofh
} // namespace srsran
