/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ran/tdd_ul_dl_config.h"
#include "srsran/ran/frame_types.h"

using namespace srsran;

static unsigned
nof_active_symbols(const tdd_ul_dl_config_common& cfg, unsigned slot_index, bool cp_extended, bool is_dl)
{
  unsigned period_slots = nof_slots_per_tdd_period(cfg);

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
      return cp_extended ? NOF_OFDM_SYM_PER_SLOT_EXTENDED_CP : NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
    } else if (slot_idx_period == pattern->nof_dl_slots) {
      return pattern->nof_dl_symbols;
    }
    return 0;
  }

  // UL case.
  unsigned ul_pattern_start = period_slots - pattern->nof_ul_slots;
  if (slot_idx_period >= ul_pattern_start) {
    return cp_extended ? NOF_OFDM_SYM_PER_SLOT_EXTENDED_CP : NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
  } else if (slot_idx_period == ul_pattern_start - 1) {
    return pattern->nof_ul_symbols;
  }
  return 0;
}

bool srsran::has_active_tdd_dl_symbols(const tdd_ul_dl_config_common& cfg, unsigned slot_index)
{
  return nof_active_symbols(cfg, slot_index, false, true) > 0;
}

bool srsran::has_active_tdd_ul_symbols(const tdd_ul_dl_config_common& cfg, unsigned slot_index)
{
  return nof_active_symbols(cfg, slot_index, false, false) > 0;
}

ofdm_symbol_range
srsran::get_active_tdd_dl_symbols(const tdd_ul_dl_config_common& cfg, unsigned slot_index, bool cp_extended)
{
  unsigned nof_symbols = nof_active_symbols(cfg, slot_index, cp_extended, true);
  return {0, nof_symbols};
}

ofdm_symbol_range
srsran::get_active_tdd_ul_symbols(const tdd_ul_dl_config_common& cfg, unsigned slot_index, bool cp_extended)
{
  unsigned nof_symbols      = nof_active_symbols(cfg, slot_index, cp_extended, false);
  unsigned symbols_per_slot = cp_extended ? NOF_OFDM_SYM_PER_SLOT_EXTENDED_CP : NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
  return nof_symbols > 0 ? ofdm_symbol_range{symbols_per_slot - nof_symbols, symbols_per_slot} : ofdm_symbol_range{};
}
