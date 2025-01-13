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

#include "helpers.h"
#include "srsran/adt/circular_map.h"
#include "srsran/ofh/ofh_constants.h"
#include "srsran/ofh/receiver/ofh_sequence_id_checker.h"

namespace srsran {

/// RU emulator sequence identifier checker.
class ru_emulator_seq_id_checker
{
  using kpi_counter = ru_emu_stats::kpi_counter;

  std::string                                               port_type;
  srslog::basic_logger&                                     logger;
  std::unique_ptr<ofh::sequence_id_checker>                 seq_id_checker;
  std::array<kpi_counter, ofh::MAX_SUPPORTED_EAXC_ID_VALUE> counters;
  ofh::slot_symbol_point                                    last_valid_symbol_point;

public:
  /// Constructor.
  ru_emulator_seq_id_checker(const std::string&                        port_type_,
                             srslog::basic_logger&                     logger_,
                             std::unique_ptr<ofh::sequence_id_checker> seq_id_checker_) :
    port_type(port_type_),
    logger(logger_),
    seq_id_checker(std::move(seq_id_checker_)),
    last_valid_symbol_point({}, 0, get_nsymb_per_slot(cyclic_prefix::NORMAL))
  {
  }

  void update_statistics(unsigned eaxc, uint8_t seq_id, ofh::slot_symbol_point symbol_point)
  {
    int nof_skipped_seq_id = seq_id_checker->update_and_compare_seq_id(eaxc, seq_id);

    auto& counter = counters[eaxc];

    // The message is from the past.
    if (nof_skipped_seq_id < 0) {
      logger.warning("Dropped received Open Fronthaul User-Plane packet for {} eAxC value '{}' in slot '{}' symbol "
                     "'{}' as sequence identifier field is from the past. Last valid slot was '{}', symbol '{}'",
                     port_type,
                     eaxc,
                     symbol_point.get_slot(),
                     symbol_point.get_symbol_index(),
                     last_valid_symbol_point.get_slot(),
                     last_valid_symbol_point.get_symbol_index());
      return counter.increment();
    }

    if (nof_skipped_seq_id > 0) {
      logger.warning("Slot '{}', symbol '{}': potentially lost '{}' messages sent by the DU on {} eAxC '{}'. Last "
                     "valid slot was '{}', symbol '{}'",
                     symbol_point.get_slot(),
                     symbol_point.get_symbol_index(),
                     nof_skipped_seq_id,
                     port_type,
                     eaxc,
                     last_valid_symbol_point.get_slot(),
                     last_valid_symbol_point.get_symbol_index());
      return counter.increment();
    }

    // The message sequence identifier matches the expected one.
    last_valid_symbol_point = symbol_point;
  }

  /// Calculates number of sequence identifier errors accumulated since last call to this function.
  uint64_t calculate_statistics(unsigned eaxc)
  {
    srsran_assert(eaxc < ofh::MAX_SUPPORTED_EAXC_ID_VALUE,
                  "Invalid eAxC value '{}'. Maximum eAxC value is '{}'",
                  eaxc,
                  ofh::MAX_SUPPORTED_EAXC_ID_VALUE);

    return counters[eaxc].calculate_acc_value();
  }
};

} // namespace srsran
