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

#include "srsran/ran/sch/tbs_calculator.h"
#include "srsran/ran/resource_block.h"
#include "srsran/support/math_utils.h"
#include "srsran/support/srsran_assert.h"
#include <algorithm>

using namespace srsran;

static unsigned tbs_calculator_step3(float nof_info)
{
  unsigned n = 3;
  if (nof_info > pow2(9U)) {
    n = static_cast<unsigned>(std::floor(std::log2(nof_info))) - 6U;
  }

  unsigned nof_info_prime =
      std::max(24U, pow2(n) * static_cast<unsigned>(std::floor(nof_info / static_cast<float>(pow2(n)))));

  // Find the closest TBS that is not less than nof_info_prime.
  return tbs_calculator_table_find_smallest_not_less_than(nof_info_prime);
}

static unsigned tbs_calculator_step4(float nof_info, float tcr)
{
  unsigned n = static_cast<unsigned>(std::floor(std::log2(nof_info - 24)) - 5.0F);

  unsigned nof_info_prime =
      std::max(3840U, pow2(n) * static_cast<unsigned>(std::round((nof_info - 24) / static_cast<float>(pow2(n)))));

  unsigned C = 1;
  if (tcr <= 0.25F) {
    C = divide_ceil(nof_info_prime + 24, 3816);
  } else if (nof_info_prime > 8424) {
    C = divide_ceil(nof_info_prime + 24, 8424);
  }

  return 8 * C * divide_ceil(nof_info_prime + 24, 8 * C) - 24;
}

static unsigned
tbs_calculator_step2(float scaling, unsigned nof_re, float tcr, unsigned modulation_level, unsigned nof_layers)
{
  float nof_info = scaling * static_cast<float>(nof_re) * tcr * static_cast<float>(modulation_level) *
                   static_cast<float>(nof_layers);

  // Step 3. Determine TBS when nof_info <= 3824.
  if (nof_info <= 3824) {
    return tbs_calculator_step3(nof_info);
  }

  // Step 4. Determine TBS when nof_info > 3824.
  return tbs_calculator_step4(nof_info, tcr);
}

float srsran::tbs_calculator_pdsch_get_scaling_factor(unsigned scaling)
{
  srsran_assert(scaling < 3, "Invalid scaling value ({}, max 2).", scaling);
  return 1.0F / static_cast<float>(pow2(scaling));
}

unsigned srsran::tbs_calculator_table_find_smallest_not_less_than(unsigned nof_info_prime)
{
  // LUT indexed by nof_info_prime in bytes based on TS38.214 Table 5.1.3.2-1 where the contents are the smallest not
  // less than nof_info_prime.
  static const std::array<uint16_t, 479> table_valid_tbs = {
      24,   24,   24,   24,   32,   40,   48,   56,   64,   72,   80,   88,   96,   104,  112,  120,  128,  136,  144,
      152,  160,  168,  176,  184,  192,  208,  208,  224,  224,  240,  240,  256,  256,  272,  272,  288,  288,  304,
      304,  320,  320,  336,  336,  352,  352,  368,  368,  384,  384,  408,  408,  408,  432,  432,  432,  456,  456,
      456,  480,  480,  480,  504,  504,  504,  528,  528,  528,  552,  552,  552,  576,  576,  576,  608,  608,  608,
      608,  640,  640,  640,  640,  672,  672,  672,  672,  704,  704,  704,  704,  736,  736,  736,  736,  768,  768,
      768,  768,  808,  808,  808,  808,  808,  848,  848,  848,  848,  848,  888,  888,  888,  888,  888,  928,  928,
      928,  928,  928,  984,  984,  984,  984,  984,  984,  984,  1032, 1032, 1032, 1032, 1032, 1032, 1064, 1064, 1064,
      1064, 1128, 1128, 1128, 1128, 1128, 1128, 1128, 1128, 1160, 1160, 1160, 1160, 1192, 1192, 1192, 1192, 1224, 1224,
      1224, 1224, 1256, 1256, 1256, 1256, 1288, 1288, 1288, 1288, 1320, 1320, 1320, 1320, 1352, 1352, 1352, 1352, 1416,
      1416, 1416, 1416, 1416, 1416, 1416, 1416, 1480, 1480, 1480, 1480, 1480, 1480, 1480, 1480, 1544, 1544, 1544, 1544,
      1544, 1544, 1544, 1544, 1608, 1608, 1608, 1608, 1608, 1608, 1608, 1608, 1672, 1672, 1672, 1672, 1672, 1672, 1672,
      1672, 1736, 1736, 1736, 1736, 1736, 1736, 1736, 1736, 1800, 1800, 1800, 1800, 1800, 1800, 1800, 1800, 1864, 1864,
      1864, 1864, 1864, 1864, 1864, 1864, 1928, 1928, 1928, 1928, 1928, 1928, 1928, 1928, 2024, 2024, 2024, 2024, 2024,
      2024, 2024, 2024, 2024, 2024, 2024, 2024, 2088, 2088, 2088, 2088, 2088, 2088, 2088, 2088, 2152, 2152, 2152, 2152,
      2152, 2152, 2152, 2152, 2216, 2216, 2216, 2216, 2216, 2216, 2216, 2216, 2280, 2280, 2280, 2280, 2280, 2280, 2280,
      2280, 2408, 2408, 2408, 2408, 2408, 2408, 2408, 2408, 2408, 2408, 2408, 2408, 2408, 2408, 2408, 2408, 2472, 2472,
      2472, 2472, 2472, 2472, 2472, 2472, 2536, 2536, 2536, 2536, 2536, 2536, 2536, 2536, 2600, 2600, 2600, 2600, 2600,
      2600, 2600, 2600, 2664, 2664, 2664, 2664, 2664, 2664, 2664, 2664, 2728, 2728, 2728, 2728, 2728, 2728, 2728, 2728,
      2792, 2792, 2792, 2792, 2792, 2792, 2792, 2792, 2856, 2856, 2856, 2856, 2856, 2856, 2856, 2856, 2976, 2976, 2976,
      2976, 2976, 2976, 2976, 2976, 2976, 2976, 2976, 2976, 2976, 2976, 2976, 3104, 3104, 3104, 3104, 3104, 3104, 3104,
      3104, 3104, 3104, 3104, 3104, 3104, 3104, 3104, 3104, 3240, 3240, 3240, 3240, 3240, 3240, 3240, 3240, 3240, 3240,
      3240, 3240, 3240, 3240, 3240, 3240, 3240, 3368, 3368, 3368, 3368, 3368, 3368, 3368, 3368, 3368, 3368, 3368, 3368,
      3368, 3368, 3368, 3368, 3496, 3496, 3496, 3496, 3496, 3496, 3496, 3496, 3496, 3496, 3496, 3496, 3496, 3496, 3496,
      3496, 3624, 3624, 3624, 3624, 3624, 3624, 3624, 3624, 3624, 3624, 3624, 3624, 3624, 3624, 3624, 3624, 3752, 3752,
      3752, 3752, 3752, 3752, 3752, 3752, 3752, 3752, 3752, 3752, 3752, 3752, 3752, 3752, 3824, 3824, 3824, 3824, 3824,
      3824, 3824, 3824, 3824};

  unsigned index = divide_ceil(nof_info_prime, 8);
  srsran_assert(index < table_valid_tbs.size(),
                "The number of information bits exceeds {} the maximum {}.",
                nof_info_prime,
                3824);

  return table_valid_tbs[index];
}

unsigned srsran::tbs_calculator_calculate(const tbs_calculator_configuration& config)
{
  static constexpr unsigned NOF_SC_RB = NOF_SUBCARRIERS_PER_RB;

  // Step 1. determine the number of REs within the slot.
  unsigned nof_re_prime = NOF_SC_RB * config.nof_symb_sh - config.nof_dmrs_prb - config.nof_oh_prb;
  unsigned nof_re       = std::min(nof_re_prime, 156U) * config.n_prb;

  float scaling = tbs_calculator_pdsch_get_scaling_factor(config.tb_scaling_field);

  srsran_assert(config.mcs_descr.modulation > modulation_scheme::BPSK,
                "Modulation scheme should be QPSK or higher, provided {}.",
                config.mcs_descr.modulation);

  // Step 2. Intermediate number of information bits.
  return tbs_calculator_step2(scaling,
                              nof_re,
                              config.mcs_descr.get_normalised_target_code_rate(),
                              get_bits_per_symbol(config.mcs_descr.modulation),
                              config.nof_layers);
}
