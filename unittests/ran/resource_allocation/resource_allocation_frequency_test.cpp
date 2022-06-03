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
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;

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