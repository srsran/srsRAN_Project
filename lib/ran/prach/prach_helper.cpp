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

#include "srsran/ran/prach/prach_helper.h"
#include "srsran/adt/span.h"
#include "srsran/adt/to_array.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"

using namespace srsran;

error_type<std::string>
srsran::prach_helper::prach_config_index_is_valid(uint8_t prach_cfg_idx, frequency_range fr, duplex_mode mode)
{
  // Supported PRACH Configuration Index values for FDD in FR1, as per TS38.211 Table 6.3.3.2-2.
  static constexpr auto fr1_tdd_intervals = to_array<interval<uint8_t>>({{0, 87}, {145, 169}});
  // Supported PRACH Configuration Index values for TDD in FR1, as per TS38.211 Table 6.3.3.2-3.
  static constexpr auto fr1_fdd_intervals = to_array<interval<uint8_t>>({{0, 108}, {198, 219}});
  // Supported PRACH Configuration Index values for TDD in FR2, as per TS38.211 Table 6.3.3.2-4.
  static constexpr auto fr2_tdd_intervals = to_array<interval<uint8_t>>({{112, 144}});

  span<const interval<uint8_t>> intervals =
      (mode == duplex_mode::FDD)     ? span<const interval<uint8_t>>(fr1_fdd_intervals)
      : (fr == frequency_range::FR1) ? span<const interval<uint8_t>>(fr1_tdd_intervals)
                                     : span<const interval<uint8_t>>(fr2_tdd_intervals);

  bool is_prach_cfg_idx_supported = std::any_of(
      intervals.begin(), intervals.end(), [prach_cfg_idx](const auto& range) { return range.contains(prach_cfg_idx); });

  if (not is_prach_cfg_idx_supported) {
    return make_unexpected(fmt::format("PRACH configuration index {} not supported. For {} and {}, the supported PRACH "
                                       "configuration indices are {}.\n",
                                       prach_cfg_idx,
                                       to_string(fr),
                                       to_string(mode),
                                       intervals));
  }

  return {};
}

error_type<std::string> srsran::prach_helper::zero_correlation_zone_is_valid(uint8_t         zero_correlation_zone,
                                                                             uint8_t         prach_cfg_idx,
                                                                             frequency_range freq_range,
                                                                             duplex_mode     dplx_mode)
{
  const prach_configuration prach_config = prach_configuration_get(freq_range, dplx_mode, prach_cfg_idx);
  if (prach_config.format == prach_format_type::invalid) {
    return make_unexpected(fmt::format("Invalid PRACH configuration index: {}\n", prach_cfg_idx));
  }

  if (dplx_mode == duplex_mode::FDD) {
    // Paired spectrum case.
    if ((prach_config.format == prach_format_type::B4) && (zero_correlation_zone != 0) &&
        (zero_correlation_zone != 11)) {
      return make_unexpected(fmt::format(
          "PRACH Zero Correlation Zone index (i.e., {}) with Format B4 is not supported for FDD. Use 0 or 11.\n",
          zero_correlation_zone));
    }
  } else {
    // Unpaired spectrum case.
    if ((prach_config.format == prach_format_type::B4) && (zero_correlation_zone != 0) &&
        (zero_correlation_zone != 14)) {
      return make_unexpected(fmt::format(
          "PRACH Zero Correlation Zone index (i.e., {}) with Format B4 is not supported for TDD. Use 0 or 14.\n",
          zero_correlation_zone));
    }
  }

  return {};
}

error_type<interval<uint8_t>> srsran::prach_helper::prach_fits_in_tdd_pattern(subcarrier_spacing pusch_scs,
                                                                              uint8_t            prach_cfg_idx,
                                                                              const tdd_ul_dl_config_common& tdd_cfg)
{
  const frequency_range fr = (pusch_scs < subcarrier_spacing::kHz120) ? frequency_range::FR1 : frequency_range::FR2;

  const prach_configuration prach_cfg = prach_configuration_get(fr, duplex_mode::TDD, prach_cfg_idx);
  if (prach_cfg.format == prach_format_type::invalid) {
    return make_unexpected(interval<uint8_t>{});
  }
  const prach_symbols_slots_duration dur = get_prach_duration_info(prach_cfg, pusch_scs);

  subcarrier_spacing ref_slot_scs = subcarrier_spacing::kHz15;
  if (fr == frequency_range::FR2) {
    ref_slot_scs = subcarrier_spacing::kHz60;
  }

  // Get the number slots per reference slot.
  const unsigned nof_slots_per_ref_slot = pow2(to_numerology_value(pusch_scs) - to_numerology_value(ref_slot_scs));

  // For each slot with PRACH, check if all slots are UL.
  for (uint8_t ref_slot_index : prach_cfg.slots) {
    // There are configuration for which the PRACH starts in an odd slot within the subframe
    // (for numerologies > mu(SCS 15kHz)); the addition of start_slot_pusch_scs compensate for this.
    uint8_t start_slot_index = ref_slot_index * nof_slots_per_ref_slot + dur.start_slot_pusch_scs;
    for (uint8_t prach_slot_idx = 0; prach_slot_idx != dur.prach_length_slots; ++prach_slot_idx) {
      uint8_t slot_index = start_slot_index + prach_slot_idx;

      // Note: For now, PRACH in special slots is not supported.
      if (not is_tdd_full_ul_slot(tdd_cfg, slot_index)) {
        // No UL symbols exist in this slot.
        return make_unexpected(interval<uint8_t>{start_slot_index, start_slot_index + dur.prach_length_slots});
      }
    }
  }

  return {};
}

std::optional<uint8_t> srsran::prach_helper::find_valid_prach_config_index(subcarrier_spacing pusch_scs,
                                                                           uint8_t            zero_correlation_zone,
                                                                           const tdd_ul_dl_config_common& tdd_cfg)
{
  static constexpr size_t NOF_PRACH_CONFIG_INDEXES = 256;

  const frequency_range fr = (pusch_scs < subcarrier_spacing::kHz120) ? frequency_range::FR1 : frequency_range::FR2;

  // Iterate over different PRACH configuration indexes until a valid one is found.
  for (unsigned prach_cfg_idx = 0; prach_cfg_idx != NOF_PRACH_CONFIG_INDEXES; ++prach_cfg_idx) {
    if (prach_config_index_is_valid(prach_cfg_idx, fr, duplex_mode::TDD).has_value() and
        zero_correlation_zone_is_valid(zero_correlation_zone, prach_cfg_idx, fr, duplex_mode::TDD).has_value() and
        prach_fits_in_tdd_pattern(pusch_scs, prach_cfg_idx, tdd_cfg).has_value()) {
      return prach_cfg_idx;
    }
  }
  return std::nullopt;
}

error_type<std::string>
srsran::prach_helper::nof_ssb_per_ro_and_nof_cb_preambles_per_ssb_is_valid(ssb_per_rach_occasions nof_ssb_per_ro,
                                                                           uint8_t nof_cb_preambles_per_ssb)
{
  bool is_valid = true;

  if (nof_ssb_per_ro <= ssb_per_rach_occasions::one) {
    // As per TS 38.331, ssb-perRACH-OccasionAndCB-PreamblesPerSSB, valid values are {4, 8, 12, 16, 20, 24, 28, 32, 36,
    // 40, 44, 48, 52, 56, 60, 64}.
    if (nof_cb_preambles_per_ssb < 4 or nof_cb_preambles_per_ssb > 64 or nof_cb_preambles_per_ssb % 4 != 0) {
      is_valid = false;
    }
  } else if (nof_ssb_per_ro == ssb_per_rach_occasions::two) {
    // As per TS 38.331, ssb-perRACH-OccasionAndCB-PreamblesPerSSB, valid values are {4, 8, 12, 16, 20, 24, 28, 32}.
    if (nof_cb_preambles_per_ssb < 4 or nof_cb_preambles_per_ssb > 32 or nof_cb_preambles_per_ssb % 4 != 0) {
      is_valid = false;
    }
  } else if (nof_ssb_per_ro == ssb_per_rach_occasions::four) {
    // As per TS 38.331, ssb-perRACH-OccasionAndCB-PreamblesPerSSB, valid values are {1,...,16}.
    if (nof_cb_preambles_per_ssb < 1 or nof_cb_preambles_per_ssb > 16) {
      is_valid = false;
    }
  } else if (nof_ssb_per_ro == ssb_per_rach_occasions::eight) {
    // As per TS 38.331, ssb-perRACH-OccasionAndCB-PreamblesPerSSB, valid values are {1,...,8}.
    if (nof_cb_preambles_per_ssb < 1 or nof_cb_preambles_per_ssb > 8) {
      is_valid = false;
    }
  } else if (nof_ssb_per_ro == ssb_per_rach_occasions::sixteen) {
    // As per TS 38.331, ssb-perRACH-OccasionAndCB-PreamblesPerSSB, valid values are {1,...,4}.
    if (nof_cb_preambles_per_ssb < 1 or nof_cb_preambles_per_ssb > 4) {
      is_valid = false;
    }
  }

  if (not is_valid) {
    return make_unexpected(
        fmt::format("Invalid nof. contention based preambles per SSB ({}) for nof. SSB per RACH occasion ({}).\n",
                    nof_cb_preambles_per_ssb,
                    ssb_per_rach_occ_to_float(nof_ssb_per_ro)));
  }

  return {};
}

error_type<std::string> srsran::prach_helper::prach_root_sequence_index_is_valid(unsigned          prach_root_seq_idx,
                                                                                 prach_format_type format)
{
  if (format >= prach_format_type::invalid) {
    return make_unexpected(fmt::format("Invalid PRACH format {}.\n", to_string(format)));
  }

  // As per Section 6.3.3.1, TS 38.211.
  const unsigned max_prach_root_seq_idx = format < prach_format_type::A1 ? 837U : 137U;
  if (prach_root_seq_idx > max_prach_root_seq_idx) {
    return make_unexpected(
        fmt::format("Invalid PRACH root sequence index {}: valid values for format {} are [0, {}).\n",
                    prach_root_seq_idx,
                    to_string(format),
                    max_prach_root_seq_idx));
  }

  return {};
}
