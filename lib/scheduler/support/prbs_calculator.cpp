/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "prbs_calculator.h"

using namespace srsgnb;

// This comparison function is needed for the search with std::upper_bound().
static bool less_then_eq_to(const unsigned& size, const unsigned& tbs)
{
  return size <= tbs;
}

// TODO: merge this function with the same used for TBS calculator.
static float tbs_calculator_pdsch_get_scaling_factor(unsigned scaling)
{
  srsran_assert(scaling < 3, "Invalid scaling value ({}, max 2).", scaling);
  return 1.0F / static_cast<float>(pow2(scaling));
}

unsigned srsgnb::get_nof_prbs(unsigned payload_size,
                              unsigned nof_symb_sh,
                              unsigned nof_dmrs_prb,
                              unsigned nof_oh_prb,
                              unsigned mod_order,
                              float    target_code_rate,
                              unsigned nof_layers,
                              unsigned tb_scaling_field)
{
  // TS38.214 Table 5.1.3.2-1.
  static const std::array<unsigned, 93> table_valid_tbs = {
      24,   32,   40,   48,   56,   64,   72,   80,   88,   96,   104,  112,  120,  128,  136,  144,  152,  160,  168,
      176,  184,  192,  208,  224,  240,  256,  272,  288,  304,  320,  336,  352,  368,  384,  408,  432,  456,  480,
      504,  528,  552,  576,  608,  640,  672,  704,  736,  768,  808,  848,  888,  928,  984,  1032, 1064, 1128, 1160,
      1192, 1224, 1256, 1288, 1320, 1352, 1416, 1480, 1544, 1608, 1672, 1736, 1800, 1864, 1928, 2024, 2088, 2152, 2216,
      2280, 2408, 2472, 2536, 2600, 2664, 2728, 2792, 2856, 2976, 3104, 3240, 3368, 3496, 3624, 3752, 3824};

  srsran_sanity_check(
      payload_size <= table_valid_tbs[92], "Payload size ({}) exceeds the maximum (3824)", payload_size);

  // Get TBS greater or equal to payload_size.
  const unsigned* it = std::upper_bound(table_valid_tbs.begin(), table_valid_tbs.end(), payload_size, less_then_eq_to);
  srsran_assert(it, "Estimate of nof_info ({}) exceeds the maximum (3824).", *it);

  // This is an estimate of the N_info (as per Section 5.1.3.2, TS 38.214), approximated as the TBS that is greater or
  // equal to the payload size.
  float nof_info_estimate = static_cast<float>(*it);

  // Get N_re (as per Section 5.1.3.2, TS 38.214) from N_info.
  float nof_re =
      nof_info_estimate / (target_code_rate * static_cast<float>(mod_order) * static_cast<float>(nof_layers) *
                           tbs_calculator_pdsch_get_scaling_factor(tb_scaling_field));

  // N_info_prime as per Section 5.1.3.2, TS 38.214.
  unsigned nof_re_prime =
      static_cast<unsigned>(srsgnb::NOF_SUBCARRIERS_PER_RB) * nof_symb_sh - nof_dmrs_prb - nof_oh_prb;

  // Get the number of PRBs from the N_re and N_info_prime.
  unsigned nof_prbs = static_cast<unsigned>(std::ceil(nof_re / static_cast<float>(std::min(nof_re_prime, 156U))));

  return nof_prbs;
}