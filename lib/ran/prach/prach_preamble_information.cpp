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

#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/ran/frame_types.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

prach_preamble_information srsran::get_prach_preamble_long_info(prach_format_type format)
{
  srsran_assert(is_long_preamble(format), "Invalid preamble format. It must be a long preamble.");
  switch (format) {
    case prach_format_type::zero:
      return {839U,
              prach_subcarrier_spacing::kHz1_25,
              phy_time_unit::from_units_of_kappa(24576),
              phy_time_unit::from_units_of_kappa(3168),
              true};
    case prach_format_type::one:
      return {839U,
              prach_subcarrier_spacing::kHz1_25,
              phy_time_unit::from_units_of_kappa(2 * 24576),
              phy_time_unit::from_units_of_kappa(21024),
              true};
    case prach_format_type::two:
      return {839U,
              prach_subcarrier_spacing::kHz1_25,
              phy_time_unit::from_units_of_kappa(4 * 24576),
              phy_time_unit::from_units_of_kappa(4688),
              true};
    case prach_format_type::three:
    default:
      return {839U,
              prach_subcarrier_spacing::kHz5,
              phy_time_unit::from_units_of_kappa(4 * 6144),
              phy_time_unit::from_units_of_kappa(3168),
              true};
  }
}

prach_preamble_information
srsran::get_prach_preamble_short_info(prach_format_type format, prach_subcarrier_spacing ra_scs, bool last_occasion)
{
  srsran_assert(is_short_preamble(format) && is_short_preamble(ra_scs),
                "The preamble format (i.e., {}) is not short or subcarrier spacing (i.e., {}) is not compatible with "
                "short preambles.",
                to_string(format),
                to_string(ra_scs));

  prach_preamble_information info;
  info.sequence_length         = 139U;
  info.scs                     = ra_scs;
  info.support_restricted_sets = false;

  unsigned      numerology             = to_numerology_value(ra_scs);
  phy_time_unit symbol_length_x0_kappa = phy_time_unit::from_units_of_kappa((1U * 2048) >> numerology);
  phy_time_unit symbol_length_x1_kappa = phy_time_unit::from_units_of_kappa((2U * 2048) >> numerology);
  phy_time_unit symbol_length_x2_kappa = phy_time_unit::from_units_of_kappa((4U * 2048) >> numerology);
  phy_time_unit symbol_length_x3_kappa = phy_time_unit::from_units_of_kappa((6U * 2048) >> numerology);
  phy_time_unit symbol_length_x4_kappa = phy_time_unit::from_units_of_kappa((12U * 2048) >> numerology);
  phy_time_unit cp_length_A1_kappa     = phy_time_unit::from_units_of_kappa(288U >> numerology);
  phy_time_unit cp_length_A2_kappa     = phy_time_unit::from_units_of_kappa(576U >> numerology);
  phy_time_unit cp_length_A3_kappa     = phy_time_unit::from_units_of_kappa(864U >> numerology);
  phy_time_unit cp_length_B1_kappa     = phy_time_unit::from_units_of_kappa(216U >> numerology);
  phy_time_unit cp_length_B2_kappa     = phy_time_unit::from_units_of_kappa(360U >> numerology);
  phy_time_unit cp_length_B3_kappa     = phy_time_unit::from_units_of_kappa(504U >> numerology);
  phy_time_unit cp_length_B4_kappa     = phy_time_unit::from_units_of_kappa(936U >> numerology);
  phy_time_unit cp_length_C0_kappa     = phy_time_unit::from_units_of_kappa(1240U >> numerology);
  phy_time_unit cp_length_C2_kappa     = phy_time_unit::from_units_of_kappa(2048U >> numerology);

  switch (format) {
    case prach_format_type::A1:
      info.symbol_length = symbol_length_x1_kappa;
      info.cp_length     = cp_length_A1_kappa;
      break;
    case prach_format_type::A2:
      info.symbol_length = symbol_length_x2_kappa;
      info.cp_length     = cp_length_A2_kappa;
      break;
    case prach_format_type::A3:
      info.symbol_length = symbol_length_x3_kappa;
      info.cp_length     = cp_length_A3_kappa;
      break;
    case prach_format_type::B1:
      info.symbol_length = symbol_length_x1_kappa;
      info.cp_length     = cp_length_B1_kappa;
      break;
    case prach_format_type::B4:
      info.symbol_length = symbol_length_x4_kappa;
      info.cp_length     = cp_length_B4_kappa;
      break;
    case prach_format_type::C0:
      info.symbol_length = symbol_length_x0_kappa;
      info.cp_length     = cp_length_C0_kappa;
      break;
    case prach_format_type::C2:
      info.symbol_length = symbol_length_x2_kappa;
      info.cp_length     = cp_length_C2_kappa;
      break;
    case prach_format_type::A1_B1:
      info.symbol_length = symbol_length_x1_kappa;
      info.cp_length     = (last_occasion) ? cp_length_B1_kappa : cp_length_A1_kappa;
      break;
    case prach_format_type::A2_B2:
      info.symbol_length = symbol_length_x2_kappa;
      info.cp_length     = (last_occasion) ? cp_length_B2_kappa : cp_length_A2_kappa;
      break;
    case prach_format_type::A3_B3:
    default:
      info.symbol_length = symbol_length_x3_kappa;
      info.cp_length     = (last_occasion) ? cp_length_B3_kappa : cp_length_A3_kappa;
      break;
  }
  return info;
}

phy_time_unit srsran::get_prach_window_duration(srsran::prach_format_type  format,
                                                srsran::subcarrier_spacing pusch_scs,
                                                unsigned                   start_symbol_index,
                                                unsigned                   nof_td_occasions)
{
  // Cyclic prefix extension for short preambles at 0 and 0.5 ms from the start of the subframe.
  static constexpr phy_time_unit sixteen_kappa = phy_time_unit::from_units_of_kappa(16);

  // PUSCH symbol duration, it assumes that RA SCS and PUSCH SCS are the same for short preambles.
  phy_time_unit pusch_symbol_duration = phy_time_unit::from_units_of_kappa(144U + 2048U);
  if (is_short_preamble(format)) {
    pusch_symbol_duration = pusch_symbol_duration / pow2(to_numerology_value(pusch_scs));
  }

  // Calculate time-domain occasion start time.
  phy_time_unit t_start = pusch_symbol_duration * start_symbol_index;

  // Add sixteen kappa units if the symbol doesn't start at the beginning of the slot.
  if (t_start > phy_time_unit::from_seconds(0.0)) {
    t_start += sixteen_kappa;
  }

  // Add sixteen kappa units if the symbol starts more than 0.5 ms after the beginning of the slot.
  if (t_start > phy_time_unit::from_seconds(0.5e-3)) {
    t_start += sixteen_kappa;
  }

  // If the preamble format is short...
  phy_time_unit t_end;
  if (is_short_preamble(format)) {
    unsigned td_occasion_duration = get_preamble_duration(format);
    t_end                         = t_start + pusch_symbol_duration * td_occasion_duration * nof_td_occasions;

    // Add sixteen kappa to the cyclic prefix length if ...
    // The window overlaps with time zero.
    if ((t_start <= phy_time_unit::from_seconds(0.0)) && (t_end >= phy_time_unit::from_seconds(0.0))) {
      t_end += sixteen_kappa;
    }

    // The window overlaps with time 0.5ms from the beginning of the slot.
    if ((t_start <= phy_time_unit::from_seconds(0.5e-3)) && (t_end > phy_time_unit::from_seconds(0.5e-3))) {
      t_end += sixteen_kappa;
    }
  } else {
    prach_preamble_information preamble_info = get_prach_preamble_long_info(format);
    t_end                                    = t_start + preamble_info.cp_length + preamble_info.symbol_length;

    // Round to t_end to the next subframe for long preambles.
    t_end = phy_time_unit::from_seconds(1e-3 * std::ceil(t_end.to_seconds() * 1e3));
  }

  return t_end;
}

prach_symbols_slots_duration srsran::get_prach_duration_info(const prach_configuration& prach_cfg,
                                                             subcarrier_spacing         pusch_scs)
{
  srsran_assert(pusch_scs == subcarrier_spacing::kHz15 or pusch_scs == subcarrier_spacing::kHz30,
                "Only SCS 15kHz and 30kHz supported");

  prach_symbols_slots_duration output{};

  // With SCS 15kHz and 30kHz, only normal CP is supported.
  static const unsigned nof_symbols_per_slot = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
  static const double   symbol_duration_msec =
      (double)SUBFRAME_DURATION_MSEC / (double)(get_nof_slots_per_subframe(pusch_scs) * nof_symbols_per_slot);

  if (is_long_preamble(prach_cfg.format)) {
    // Derive PRACH subcarrier spacing and other parameters.
    const prach_preamble_information info = get_prach_preamble_long_info(prach_cfg.format);

    const double length_msecs = (info.cp_length.to_seconds() + info.symbol_length.to_seconds()) * 1000;
    output.nof_symbols        = ceil(length_msecs / symbol_duration_msec);
    // Map the starting symbol with from the SCS 15kHz FR1 reference for PRACH into PUSCH SCS.
    const unsigned start_symbol_pusch_scs_in_subframe =
        prach_cfg.starting_symbol * (1U << to_numerology_value(pusch_scs));
    // Start slot within the subframe. For FR1, the start_symbol_pusch_scs_in_subframe refers to the SCS 15kHz.
    output.start_slot_pusch_scs   = start_symbol_pusch_scs_in_subframe / nof_symbols_per_slot;
    output.start_symbol_pusch_scs = start_symbol_pusch_scs_in_subframe % nof_symbols_per_slot;
    output.prach_length_slots =
        static_cast<unsigned>(ceil(static_cast<double>(output.start_symbol_pusch_scs + output.nof_symbols) /
                                   (static_cast<double>(nof_symbols_per_slot))));
  } else {
    const prach_subcarrier_spacing prach_scs = to_ra_subcarrier_spacing(pusch_scs);
    output.nof_symbols = static_cast<unsigned>(prach_cfg.nof_occasions_within_slot * prach_cfg.duration);
    // Starting slot within the subframe; this can be derived from Section 5.3.2, TS 38.211.
    const bool prach_starts_in_even_slot =
        prach_scs == prach_subcarrier_spacing::kHz15 or
        (prach_scs == prach_subcarrier_spacing::kHz30 and prach_cfg.nof_prach_slots_within_subframe != 1);
    output.start_slot_pusch_scs = prach_starts_in_even_slot ? 0U : 1U;
    // The burst of PRACH opportunities for short PRACH formats can be over 1 or 2 slots, as per Section 5.3.2,  and
    // Tables 6.3.3.2-2 and 6.3.3.2-3, TS 38.211.
    output.prach_length_slots =
        prach_scs == prach_subcarrier_spacing::kHz30 and prach_cfg.nof_prach_slots_within_subframe != 1 ? 2U : 1U;
  }

  return output;
}
