/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/ran/tdd/tdd_ul_dl_config.h"

using namespace srsran;

static unsigned
nof_active_symbols(const tdd_ul_dl_config_common& cfg, unsigned slot_index, cyclic_prefix cp, bool is_dl)
{
  const unsigned period_slots = nof_slots_per_tdd_period(cfg);

  // Calculate slot index within the TDD overall period.
  unsigned slot_idx_period = slot_index % period_slots; // Slot index within the TDD pattern period.

  // Select pattern
  const tdd_ul_dl_pattern* pattern = &cfg.pattern1;
  if (slot_idx_period >= cfg.pattern1.dl_ul_tx_period_nof_slots) {
    pattern = &cfg.pattern2.value();
    slot_idx_period -= cfg.pattern1.dl_ul_tx_period_nof_slots; // Remove pattern 1 offset
  }

  if (is_dl) {
    // Check DL boundaries. Both fully DL slots and partially DL slots return a positive value.
    if (slot_idx_period < pattern->nof_dl_slots) {
      return get_nsymb_per_slot(cp);
    } else if (slot_idx_period == pattern->nof_dl_slots) {
      return pattern->nof_dl_symbols;
    }
    return 0;
  }

  // UL case.
  const unsigned ul_pattern_start = pattern->dl_ul_tx_period_nof_slots - pattern->nof_ul_slots;
  if (slot_idx_period >= ul_pattern_start) {
    return get_nsymb_per_slot(cp);
  }
  if (slot_idx_period == ul_pattern_start - 1) {
    return pattern->nof_ul_symbols;
  }
  return 0;
}

bool srsran::has_active_tdd_dl_symbols(const tdd_ul_dl_config_common& cfg, unsigned slot_index)
{
  return nof_active_symbols(cfg, slot_index, cyclic_prefix::NORMAL, true) > 0;
}

bool srsran::has_active_tdd_ul_symbols(const tdd_ul_dl_config_common& cfg, unsigned slot_index)
{
  return nof_active_symbols(cfg, slot_index, cyclic_prefix::NORMAL, false) > 0;
}

bool srsran::is_tdd_full_dl_slot(const tdd_ul_dl_config_common& cfg, unsigned slot_index)
{
  return nof_active_symbols(cfg, slot_index, cyclic_prefix::NORMAL, true) == NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
}

bool srsran::is_tdd_full_ul_slot(const tdd_ul_dl_config_common& cfg, unsigned slot_index)
{
  return nof_active_symbols(cfg, slot_index, cyclic_prefix::NORMAL, false) == NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
}

ofdm_symbol_range
srsran::get_active_tdd_dl_symbols(const tdd_ul_dl_config_common& cfg, unsigned slot_index, cyclic_prefix cp)
{
  const unsigned nof_symbols = nof_active_symbols(cfg, slot_index, cp, true);
  return {0, nof_symbols};
}

ofdm_symbol_range
srsran::get_active_tdd_ul_symbols(const tdd_ul_dl_config_common& cfg, unsigned slot_index, cyclic_prefix cp)
{
  const unsigned nof_symbols      = nof_active_symbols(cfg, slot_index, cp, false);
  const unsigned symbols_per_slot = get_nsymb_per_slot(cp);
  return nof_symbols > 0 ? ofdm_symbol_range{symbols_per_slot - nof_symbols, symbols_per_slot} : ofdm_symbol_range{};
}

optional<unsigned> srsran::find_next_tdd_dl_slot(const tdd_ul_dl_config_common& cfg, unsigned start_slot_index)
{
  const unsigned period = nof_slots_per_tdd_period(cfg);

  optional<unsigned> ret;
  for (unsigned i = start_slot_index; i < period; ++i) {
    if (has_active_tdd_dl_symbols(cfg, i)) {
      ret = i;
      break;
    }
  }
  return ret;
}

optional<unsigned> srsran::find_next_tdd_full_dl_slot(const tdd_ul_dl_config_common& cfg, unsigned start_slot_index)
{
  const unsigned period = nof_slots_per_tdd_period(cfg);

  optional<unsigned> ret;
  for (unsigned i = start_slot_index; i < period; ++i) {
    if (get_active_tdd_dl_symbols(cfg, i, cyclic_prefix::NORMAL).length() == NOF_OFDM_SYM_PER_SLOT_NORMAL_CP) {
      ret = i;
      break;
    }
  }
  return ret;
}

optional<unsigned> srsran::find_next_tdd_ul_slot(const tdd_ul_dl_config_common& cfg, unsigned start_slot_index)
{
  const unsigned period = nof_slots_per_tdd_period(cfg);

  optional<unsigned> ret;
  for (unsigned i = start_slot_index; i < period; ++i) {
    if (has_active_tdd_ul_symbols(cfg, i)) {
      ret = i;
      break;
    }
  }
  return ret;
}

optional<unsigned> srsran::find_next_tdd_full_ul_slot(const tdd_ul_dl_config_common& cfg, unsigned start_slot_index)
{
  const unsigned period = nof_slots_per_tdd_period(cfg);

  optional<unsigned> ret;
  for (unsigned i = start_slot_index; i < period; ++i) {
    if (get_active_tdd_ul_symbols(cfg, i, cyclic_prefix::NORMAL).length() == NOF_OFDM_SYM_PER_SLOT_NORMAL_CP) {
      ret = i;
      break;
    }
  }
  return ret;
}
