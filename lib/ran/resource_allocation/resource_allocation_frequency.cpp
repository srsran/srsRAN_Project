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

#include "srsran/ran/resource_allocation/resource_allocation_frequency.h"
#include "srsran/ran/sliv.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

unsigned int srsran::ra_frequency_type1_get_riv(const ra_frequency_type1_configuration& config)
{
  srsran_assert(config.length_vrb >= 1,
                "The number of contiguous allocated blocks must be greater than or equal to 1.");
  srsran_assert(config.length_vrb <= (config.N_bwp_size - config.start_vrb),
                "The number of contiguous allocated blocks exceeds the maximum ({}).",
                config.N_bwp_size - config.start_vrb);

  return sliv_from_s_and_l(config.N_bwp_size, config.start_vrb, config.length_vrb);
}

ra_frequency_type1_configuration srsran::ra_frequency_type1_from_riv(unsigned N_bwp_size, unsigned riv)
{
  ra_frequency_type1_configuration out;
  out.N_bwp_size = N_bwp_size;
  sliv_to_s_and_l(N_bwp_size, riv, out.start_vrb, out.length_vrb);
  return out;
}

unsigned srsran::ra_frequency_type1_special_get_riv(const ra_frequency_type1_special_configuration& config)
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
