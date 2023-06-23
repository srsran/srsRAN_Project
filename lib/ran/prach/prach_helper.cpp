/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ran/prach/prach_helper.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"

using namespace srsran;

error_type<interval<uint8_t>> srsran::prach_helper::prach_fits_in_tdd_pattern(subcarrier_spacing pusch_scs,
                                                                              cyclic_prefix      cp,
                                                                              uint8_t            prach_cfg_idx,
                                                                              const tdd_ul_dl_config_common& tdd_cfg)
{
  const unsigned nof_slots_per_subframe = get_nof_slots_per_subframe(pusch_scs);

  const prach_configuration prach_cfg = prach_configuration_get(frequency_range::FR1, duplex_mode::TDD, prach_cfg_idx);
  if (prach_cfg.format == prach_format_type::invalid) {
    return interval<uint8_t>{};
  }
  const prach_symbols_slots_duration dur = get_prach_duration_info(prach_cfg, pusch_scs);

  // For each subframe with PRACH, check if all slots are UL.
  for (uint8_t subframe_index : prach_cfg.subframe) {
    // There are configuration for which the PRACH starts in an odd slot within the subframe
    // (for numerologies > mu(SCS 15kHz)); the addition of start_slot_pusch_scs compensate for this.
    uint8_t start_slot_index = subframe_index * nof_slots_per_subframe + dur.start_slot_pusch_scs;
    for (uint8_t prach_slot_idx = 0; prach_slot_idx != dur.prach_length_slots; ++prach_slot_idx) {
      uint8_t           slot_index        = start_slot_index + prach_slot_idx;
      ofdm_symbol_range active_ul_symbols = get_active_tdd_ul_symbols(tdd_cfg, slot_index, cp);

      // Note: For now, PRACH in special slots is not supported.
      if (active_ul_symbols.length() != get_nsymb_per_slot(cp)) {
        // No UL symbols exist in this slot.
        return interval<uint8_t>{start_slot_index, start_slot_index + dur.prach_length_slots};
      }
    }
  }

  return {};
}

optional<uint8_t> srsran::prach_helper::find_valid_prach_config_index(subcarrier_spacing             pusch_scs,
                                                                      cyclic_prefix                  cp,
                                                                      const tdd_ul_dl_config_common& tdd_cfg)
{
  static constexpr size_t NOF_PRACH_CONFIG_INDEXES = 256;

  // Iterate over different PRACH configuration indexes until a valid one is found.
  for (unsigned prach_cfg_idx = 0; prach_cfg_idx != NOF_PRACH_CONFIG_INDEXES; ++prach_cfg_idx) {
    if (prach_fits_in_tdd_pattern(pusch_scs, cp, prach_cfg_idx, tdd_cfg).has_value()) {
      return prach_cfg_idx;
    }
  }
  return nullopt;
}
