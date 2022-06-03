/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/ran/resource_allocation/resource_allocation_frequency.h"
#include "srsgnb/ran/sliv.h"
#include "srsgnb/support/srsran_assert.h"

using namespace srsgnb;

unsigned int srsgnb::ra_frequency_type1_get_riv(const ra_frequency_type1_configuration& config)
{
  srsran_assert(config.length_vrb >= 1,
                "The number of contiguous allocated blocks must be greater than or equal to 1.");
  srsran_assert(config.length_vrb <= (config.N_bwp_size - config.start_vrb),
                "he number of contiguous allocated blocks exceeds the maximum ({}).",
                config.N_bwp_size - config.start_vrb);
  if ((config.length_vrb - 1) <= (config.N_bwp_size / 2)) {
    return config.N_bwp_size * (config.length_vrb - 1) + config.start_vrb;
  }

  return sliv_from_s_and_l(config.N_bwp_size, config.start_vrb, config.length_vrb);
}

unsigned srsgnb::ra_frequency_type1_special_get_riv(const ra_frequency_type1_special_configuration& config)
{
  // Determine K.
  unsigned K = 8;
  while (K > 1) {
    if (K <= (config.N_bwp_active / config.N_bwp_initial)) {
      break;
    }
    K /= 2;
  }

  unsigned start_vrb  = config.start_vrb / K;
  unsigned length_vrb = config.length_vrb / K;

  srsran_assert(length_vrb >= 1, "The number of contiguous allocated blocks must be greater than or equal to 1.");
  srsran_assert(length_vrb <= (config.N_bwp_initial - start_vrb),
                "The number of contiguous allocated blocks ({}) exceeds the maximum ({}). K={} RB_start={} L_RBs={} "
                "N_bwp_initial={} N_bwp_active={}.",
                length_vrb,
                config.N_bwp_initial - start_vrb,
                K,
                config.start_vrb,
                config.length_vrb,
                config.N_bwp_initial,
                config.N_bwp_active);

  return sliv_from_s_and_l(config.N_bwp_initial, start_vrb, length_vrb);
}
