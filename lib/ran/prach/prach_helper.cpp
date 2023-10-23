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

#include "srsran/ran/prach/prach_helper.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"

using namespace srsran;

error_type<std::string> srsran::prach_helper::prach_config_index_is_valid(uint8_t prach_cfg_idx, duplex_mode mode)
{
  bool       is_paired_spectrum         = mode == duplex_mode::FDD;
  const bool is_prach_cfg_idx_supported = is_paired_spectrum
                                              ? prach_cfg_idx <= 107U or (prach_cfg_idx > 197U and prach_cfg_idx < 219U)
                                              : prach_cfg_idx <= 86U or (prach_cfg_idx > 144U and prach_cfg_idx < 169U);
  if (not is_prach_cfg_idx_supported) {
    return fmt::format(
        "PRACH configuration index {} not supported. For {}, the supported PRACH configuration indices are {}\n",
        prach_cfg_idx,
        is_paired_spectrum ? "FDD" : "TDD",
        is_paired_spectrum ? "[0, 107] and [198, 218]" : "[0, 86] and [145, 168]");
  }

  return {};
}

error_type<std::string> srsran::prach_helper::zero_correlation_zone_is_valid(uint8_t     zero_correlation_zone,
                                                                             uint8_t     prach_cfg_idx,
                                                                             duplex_mode dplx_mode)
{
  prach_configuration prach_config = prach_configuration_get(frequency_range::FR1, dplx_mode, prach_cfg_idx);
  if (prach_config.format == prach_format_type::invalid) {
    return fmt::format("Invalid PRACH configuration index: {}\n", prach_cfg_idx);
  }

  if (dplx_mode == duplex_mode::FDD) {
    // Paired spectrum case.
    if ((prach_config.format == prach_format_type::B4) && (zero_correlation_zone != 0) &&
        (zero_correlation_zone != 11)) {
      return fmt::format(
          "PRACH Zero Correlation Zone index (i.e., {}) with Format B4 is not supported for FDD. Use 0 or 11.\n",
          zero_correlation_zone);
    }
  } else {
    // Unpaired spectrum case.
    if ((prach_config.format == prach_format_type::B4) && (zero_correlation_zone != 0) &&
        (zero_correlation_zone != 14)) {
      return fmt::format(
          "PRACH Zero Correlation Zone index (i.e., {}) with Format B4 is not supported for TDD. Use 0 or 14.\n",
          zero_correlation_zone);
    }
  }

  return {};
}

error_type<interval<uint8_t>> srsran::prach_helper::prach_fits_in_tdd_pattern(subcarrier_spacing pusch_scs,
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
      uint8_t slot_index = start_slot_index + prach_slot_idx;

      // Note: For now, PRACH in special slots is not supported.
      if (not is_tdd_full_ul_slot(tdd_cfg, slot_index)) {
        // No UL symbols exist in this slot.
        return interval<uint8_t>{start_slot_index, start_slot_index + dur.prach_length_slots};
      }
    }
  }

  return {};
}

optional<uint8_t> srsran::prach_helper::find_valid_prach_config_index(subcarrier_spacing pusch_scs,
                                                                      uint8_t            zero_correlation_zone,
                                                                      const tdd_ul_dl_config_common& tdd_cfg)
{
  static constexpr size_t NOF_PRACH_CONFIG_INDEXES = 256;

  // Iterate over different PRACH configuration indexes until a valid one is found.
  for (unsigned prach_cfg_idx = 0; prach_cfg_idx != NOF_PRACH_CONFIG_INDEXES; ++prach_cfg_idx) {
    if (prach_config_index_is_valid(prach_cfg_idx, duplex_mode::TDD).has_value() and
        zero_correlation_zone_is_valid(zero_correlation_zone, prach_cfg_idx, duplex_mode::TDD).has_value() and
        prach_fits_in_tdd_pattern(pusch_scs, prach_cfg_idx, tdd_cfg).has_value()) {
      return prach_cfg_idx;
    }
  }
  return nullopt;
}
