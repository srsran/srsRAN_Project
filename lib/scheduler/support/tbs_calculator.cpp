/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "tbs_calculator.h"
#include "srsgnb/ran/resource_block.h"
#include "srsgnb/support/math_utils.h"
#include "srsgnb/support/srsran_assert.h"
#include <algorithm>

using namespace srsgnb;

static unsigned tbs_calculator_step3(float nof_info)
{
  unsigned n = 3;
  if (nof_info > pow2(9U)) {
    n = static_cast<unsigned>(std::floor(std::log2(nof_info))) - 6U;
  }

  unsigned nof_info_prime =
      std::max(24U, pow2(n) * static_cast<unsigned>(std::floor(nof_info / static_cast<float>(pow2(n)))));

  // Find the closest TBS that is not less than nof_info_prime.
  const unsigned* it = std::upper_bound(table_valid_tbs.begin(), table_valid_tbs.end(), nof_info_prime - 1);
  srsran_assert(
      it != table_valid_tbs.end(), "Number of information bits ({}) exceeds the maximum (3824).", nof_info_prime);

  return *it;
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

float srsgnb::tbs_calculator_pdsch_get_scaling_factor(unsigned scaling)
{
  srsran_assert(scaling < 3, "Invalid scaling value ({}, max 2).", scaling);
  return 1.0F / static_cast<float>(pow2(scaling));
}

unsigned srsgnb::tbs_calculator_pdsch_calculate(const tbs_calculator_pdsch_configuration& config)
{
  static constexpr unsigned NOF_SC_RB = NOF_SUBCARRIERS_PER_RB;

  // Step 1. determine the number of REs within the slot.
  unsigned nof_re_prime = NOF_SC_RB * config.nof_symb_sh - config.nof_dmrs_prb - config.nof_oh_prb;
  unsigned nof_re       = std::min(nof_re_prime, 156U) * config.n_prb;

  float scaling = tbs_calculator_pdsch_get_scaling_factor(config.tb_scaling_field);

  // Step 2. Intermediate number of information bits.
  return tbs_calculator_step2(scaling, nof_re, config.tcr, config.modulation_order, config.nof_layers);
}