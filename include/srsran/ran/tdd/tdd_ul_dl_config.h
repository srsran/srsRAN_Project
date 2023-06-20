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

#include "srsran/adt/optional.h"
#include "srsran/ran/ofdm_symbol_range.h"
#include "srsran/ran/subcarrier_spacing.h"

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
  subcarrier_spacing          ref_scs;
  tdd_ul_dl_pattern           pattern1;
  optional<tdd_ul_dl_pattern> pattern2;
};

/// \brief Calculates number of slots, using TDD reference SCS, of the TDD UL-DL configuration.
inline unsigned nof_slots_per_tdd_period(const tdd_ul_dl_config_common& cfg)
{
  return cfg.pattern1.dl_ul_tx_period_nof_slots +
         (cfg.pattern2.has_value() ? cfg.pattern2->dl_ul_tx_period_nof_slots : 0U);
}

/// \brief Calculates whether there are symbols for DL in the current slot index.
bool has_active_tdd_dl_symbols(const tdd_ul_dl_config_common& cfg, unsigned slot_index);

/// \brief Calculates whether there are symbols for UL in the current slot index.
bool has_active_tdd_ul_symbols(const tdd_ul_dl_config_common& cfg, unsigned slot_index);

/// \brief Calculates the number of active DL symbols in the current slot_index.
ofdm_symbol_range get_active_tdd_dl_symbols(const tdd_ul_dl_config_common& cfg, unsigned slot_index, bool cp_extended);

/// \brief Calculates the number of active UL symbols in the current slot_index.
ofdm_symbol_range get_active_tdd_ul_symbols(const tdd_ul_dl_config_common& cfg, unsigned slot_index, bool cp_extended);

} // namespace srsran
