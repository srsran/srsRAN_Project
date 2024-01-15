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
#include "srsran/support/srsran_test.h"

using namespace srsran;

static void test_ra_frequency_type1()
{
  for (unsigned N_bwp_size : {24, 48, 96}) {
    for (unsigned start = 0; start != N_bwp_size - 1; ++start) {
      for (unsigned length = 1, length_end = N_bwp_size - start + 1; length != length_end; ++length) {
        ra_frequency_type1_configuration config;
        config.N_bwp_size = N_bwp_size;
        config.start_vrb  = start;
        config.length_vrb = length;

        unsigned riv = ra_frequency_type1_get_riv(config);

        unsigned expected;
        if ((length - 1) <= (N_bwp_size / 2)) {
          expected = N_bwp_size * (length - 1) + start;
        } else {
          expected = N_bwp_size * (N_bwp_size - length + 1) + (N_bwp_size - 1 - start);
        }

        TESTASSERT_EQ(riv, expected);
      }
    }
  }
}

static void test_ra_frequency_type1_special()
{
  for (unsigned N_bwp_initial : {24, 48, 96}) {
    for (unsigned N_bwp_active : {25, 52, 104, 272}) {
      // Determine K.
      unsigned K = 8;
      while (K > 1) {
        if (K <= N_bwp_active / N_bwp_initial) {
          break;
        }
        K /= 2;
      }

      for (unsigned start = 0; start < (N_bwp_initial - 1) * K; start += K) {
        for (unsigned length = K; length < N_bwp_initial * K - start; length += K) {
          ra_frequency_type1_special_configuration config;
          config.N_bwp_initial = N_bwp_initial;
          config.N_bwp_active  = N_bwp_active;
          config.start_vrb     = start;
          config.length_vrb    = length;

          unsigned riv = ra_frequency_type1_special_get_riv(config);

          unsigned start_prime  = start / K;
          unsigned length_prime = length / K;

          unsigned expected;
          if ((length_prime - 1) <= (N_bwp_initial / 2)) {
            expected = N_bwp_initial * (length_prime - 1) + start_prime;
          } else {
            expected = N_bwp_initial * (N_bwp_initial - length_prime + 1) + (N_bwp_initial - 1 - start_prime);
          }

          TESTASSERT_EQ(riv, expected);
        }
      }
    }
  }
}

int main()
{
  test_ra_frequency_type1();
  test_ra_frequency_type1_special();
  return 0;
}