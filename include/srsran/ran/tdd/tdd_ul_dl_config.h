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

#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/resource_allocation/ofdm_symbol_range.h"
#include "srsran/ran/subcarrier_spacing.h"
#include <optional>

namespace srsran {

/// \remark See TS 38.331, "TDD-UL-DL-Pattern".
struct tdd_ul_dl_pattern {
  /// Periodicity of the DL-UL pattern.
  unsigned dl_ul_tx_period_nof_slots;
  /// Values: (0..maxNrofSlots=320).
  unsigned nof_dl_slots;
  /// Values: (0..maxNrofSymbols-1=13).
  unsigned nof_dl_symbols;
  /// Values: (0..maxNrofSlots=320).
  unsigned nof_ul_slots;
  /// Values: (0..maxNrofSymbols-1=13).
  unsigned nof_ul_symbols;
};

/// \remark See TS 38.331, "TDD-UL-DL-ConfigCommon".
struct tdd_ul_dl_config_common {
  /// Reference SCS used to determine the time domain boundaries in the UL-DL pattern. The network ensures that
  /// this value is not larger than any SCS of configured BWPs for the serving cell.
  subcarrier_spacing               ref_scs;
  tdd_ul_dl_pattern                pattern1;
  std::optional<tdd_ul_dl_pattern> pattern2;
};

/// \brief Calculates number of slots, using TDD reference SCS, of the TDD UL-DL configuration.
inline unsigned nof_slots_per_tdd_period(const tdd_ul_dl_config_common& cfg)
{
  return cfg.pattern1.dl_ul_tx_period_nof_slots +
         (cfg.pattern2.has_value() ? cfg.pattern2->dl_ul_tx_period_nof_slots : 0U);
}

/// \brief Calculates number of slots with all DL symbols in the TDD UL-DL configuration.
inline unsigned nof_full_dl_slots_per_tdd_period(const tdd_ul_dl_config_common& cfg)
{
  return cfg.pattern1.nof_dl_slots + (cfg.pattern2.has_value() ? cfg.pattern2->nof_dl_slots : 0U);
}

/// \brief Calculates number of slots with DL symbols in the TDD UL-DL configuration.
inline unsigned nof_dl_slots_per_tdd_period(const tdd_ul_dl_config_common& cfg)
{
  const auto pattern1_additional_dl_slot = cfg.pattern1.nof_dl_symbols > 0 ? 1 : 0;
  const auto pattern2_additional_dl_slot =
      cfg.pattern2.has_value() ? (cfg.pattern2.value().nof_dl_symbols > 0 ? 1 : 0) : 0;
  return cfg.pattern1.nof_dl_slots + pattern1_additional_dl_slot +
         (cfg.pattern2.has_value() ? cfg.pattern2->nof_dl_slots + pattern2_additional_dl_slot : 0U);
}

/// \brief Calculates number of slots with all UL symbols in the TDD UL-DL configuration.
inline unsigned nof_full_ul_slots_per_tdd_period(const tdd_ul_dl_config_common& cfg)
{
  return cfg.pattern1.nof_ul_slots + (cfg.pattern2.has_value() ? cfg.pattern2->nof_ul_slots : 0U);
}

/// \brief Calculates whether there are symbols for DL in the current slot index.
bool has_active_tdd_dl_symbols(const tdd_ul_dl_config_common& cfg, unsigned slot_index);

/// \brief Calculates whether there are symbols for UL in the current slot index.
bool has_active_tdd_ul_symbols(const tdd_ul_dl_config_common& cfg, unsigned slot_index);

/// \brief Checks if all symbols in the current slot index are active for DL.
bool is_tdd_full_dl_slot(const tdd_ul_dl_config_common& cfg, unsigned slot_index);

/// \brief Checks if all symbols in the current slot index are active for UL.
bool is_tdd_full_ul_slot(const tdd_ul_dl_config_common& cfg, unsigned slot_index);

/// \brief Checks if some symbols in the current slot index, but not all of them, are active for UL.
bool is_tdd_partial_ul_slot(const tdd_ul_dl_config_common& cfg, unsigned slot_index);

/// \brief Calculates the number of active DL symbols in the current slot_index.
ofdm_symbol_range get_active_tdd_dl_symbols(const tdd_ul_dl_config_common& cfg, unsigned slot_index, cyclic_prefix cp);

/// \brief Calculates the number of active UL symbols in the current slot_index.
ofdm_symbol_range get_active_tdd_ul_symbols(const tdd_ul_dl_config_common& cfg, unsigned slot_index, cyclic_prefix cp);

/// \brief Finds the next TDD slot index with DL symbols within the TDD period, starting from the given slot index.
/// \return The slot index, if found, or an empty optional otherwise. The case of empty optional would the case of
/// start_slot_index being past the last DL slot within the period.
std::optional<unsigned> find_next_tdd_dl_slot(const tdd_ul_dl_config_common& cfg, unsigned start_slot_index = 0);

/// \brief Finds the next TDD slot index with all UL symbols, starting from the given slot index.
/// \return The slot index, if found, or an empty optional otherwise. The case of empty optional would the case of
/// start_slot_index being past the last full DL slot within the period.
std::optional<unsigned> find_next_tdd_full_dl_slot(const tdd_ul_dl_config_common& cfg, unsigned start_slot_index = 0);

/// \brief Finds the next TDD slot index with UL symbols, starting from the given slot index.
/// \return The slot index, if found, or an empty optional otherwise. The case of empty optional would the case of
/// start_slot_index being >= TDD period in slots.
std::optional<unsigned> find_next_tdd_ul_slot(const tdd_ul_dl_config_common& cfg, unsigned start_slot_index = 0);

/// \brief Finds the next TDD slot index with all UL symbols, starting from the given slot index.
/// \return The slot index, if found, or an empty optional otherwise. The case of empty optional would the case of
/// start_slot_index being >= TDD period in slots.
std::optional<unsigned> find_next_tdd_full_ul_slot(const tdd_ul_dl_config_common& cfg, unsigned start_slot_index = 0);

} // namespace srsran
