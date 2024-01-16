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

#include "srsran/ran/pdcch/pdcch_type0_css_occasions.h"

using namespace srsran;

pdcch_type0_css_occasion_pattern1_description
srsran::pdcch_type0_css_occasions_get_pattern1(const pdcch_type0_css_occasion_pattern1_configuration& config)
{
  srsran_assert((!config.is_fr2 && config.ss_zero_index < 16) || (config.is_fr2 && config.ss_zero_index < 14),
                "SearchSpaceZero ({}) must be (0...{}) for {}.",
                config.ss_zero_index,
                config.is_fr2 ? 13 : 15,
                config.is_fr2 ? "FR2" : "FR1");

  static const unsigned N_SYMB_CORESET = UINT32_MAX;

  // TS38.213 Table 13-11 for FR1.
  static const std::array<pdcch_type0_css_occasion_pattern1_description, 16> TABLE_FR1 = {{
      {0, 1.0, {0}},
      {0, 0.5, {0, N_SYMB_CORESET}},
      {2, 1.0, {0}},
      {2, 0.5, {0, N_SYMB_CORESET}},
      {5, 1.0, {0}},
      {5, 0.5, {0, N_SYMB_CORESET}},
      {7, 1.0, {0}},
      {7, 0.5, {0, N_SYMB_CORESET}},
      {0, 2.0, {0}},
      {5, 2.0, {0}},
      {0, 1.0, {1}},
      {0, 1.0, {2}},
      {2, 1.0, {1}},
      {2, 1.0, {2}},
      {5, 1.0, {1}},
      {5, 1.0, {2}},
  }};

  // TS38.213 Table 13-12 for FR2.
  static const std::array<pdcch_type0_css_occasion_pattern1_description, 14> TABLE_FR2 = {{
      {0, 1.0, {0}},
      {0, 0.5, {0, 7}},
      {2.5, 1.0, {0}},
      {2.5, 0.5, {0, 7}},
      {5.0, 1.0, {0}},
      {5.0, 0.5, {0, 7}},
      {0.0, 0.5, {0, N_SYMB_CORESET}},
      {2.5, 0.5, {0, N_SYMB_CORESET}},
      {5.0, 0.5, {0, N_SYMB_CORESET}},
      {7.5, 1.0, {0}},
      {7.5, 0.5, {0, 7}},
      {7.5, 0.5, {0, N_SYMB_CORESET}},
      {0.0, 2.0, {0}},
      {5.0, 2.0, {0}},
  }};

  // Select occasion from the tables.
  pdcch_type0_css_occasion_pattern1_description occasion = TABLE_FR1[config.ss_zero_index];
  if (config.is_fr2) {
    occasion = TABLE_FR2[config.ss_zero_index];
  }

  // Substitute the start symbol with the CORESET number of symbols.
  if (occasion.start_symbol.size() == occasion.MAX_NOF_OCCASIONS_PER_SLOT and
      occasion.start_symbol.back() == N_SYMB_CORESET) {
    occasion.start_symbol.back() = config.nof_symb_coreset;
  }

  return occasion;
}

unsigned srsran::type0_pdcch_css_n0_is_even_frame(double   table_13_11_and_13_12_O,
                                                  double   table_13_11_and_13_12_M,
                                                  uint8_t  numerology_mu,
                                                  unsigned ssb_index)
{
  // This is only used to retrieve the nof_slots_per_frame.
  slot_point sl_point{numerology_mu, 0};

  // Compute floor( ( O * 2^mu + floor(i*M) ) / nof_slots_per_frame  ) mod 2, as per TS 38.213, Section 13.
  unsigned is_even = static_cast<unsigned>(floor(static_cast<double>(table_13_11_and_13_12_O * (1U << numerology_mu)) +
                                                 floor(static_cast<double>(ssb_index) * table_13_11_and_13_12_M)) /
                                           sl_point.nof_slots_per_frame()) %
                     2;
  return is_even;
}

slot_point srsran::get_type0_pdcch_css_n0(double             table_13_11_and_13_12_O,
                                          double             table_13_11_and_13_12_M,
                                          subcarrier_spacing scs_common,
                                          unsigned           ssb_index)
{
  // Initialize n0 to a slot_point = 0.
  const uint8_t numerology_mu = to_numerology_value(scs_common);
  slot_point    type0_pdcch_css_n0{numerology_mu, 0};

  // Compute n0 = ( O * 2^mu + floor(i*M)  )  % nof_slots_per_frame, as per TS 38.213, Section 13.
  type0_pdcch_css_n0 += static_cast<unsigned>(static_cast<double>(table_13_11_and_13_12_O * (1U << numerology_mu)) +
                                              floor(static_cast<double>(ssb_index) * table_13_11_and_13_12_M)) %
                        type0_pdcch_css_n0.nof_slots_per_frame();

  // We want to express n0 as a value from 0 to max_nof_slots. Since the mod operation above cap n0 to
  // (nof_slots_per_frame - 1), we need to add nof_slots_per_frame to n0 if this falls into an odd frame.
  type0_pdcch_css_n0 +=
      type0_pdcch_css_n0_is_even_frame(table_13_11_and_13_12_O, table_13_11_and_13_12_M, numerology_mu, ssb_index) *
      type0_pdcch_css_n0.nof_slots_per_frame();

  return type0_pdcch_css_n0;
}
