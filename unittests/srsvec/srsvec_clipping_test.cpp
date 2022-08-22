/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Unit test for the \c srsvec::clip \c function.

#include "srsgnb/srsvec/add.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include "srsgnb/srsvec/clip.h"
#include "srsgnb/support/srsgnb_test.h"
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_MAX_ERROR = 1e-6;

using namespace srsgnb;

static void test_clip_function(std::size_t nof_samples, float max_amplitude, float clip_threshold)
{
  srsvec::aligned_vec<cf_t> input(nof_samples);
  srsvec::aligned_vec<cf_t> output(nof_samples);
  srsvec::aligned_vec<cf_t> output_gold(nof_samples);

  // Generate pseudorandom complex numbers with a uniformly distributed magnitude.
  std::uniform_real_distribution<float> dist_mag(0.0F, max_amplitude);
  std::uniform_real_distribution<float> dist_phase(-M_PI, M_PI);

  for (cf_t& v : input) {
    v = std::polar(dist_mag(rgen), dist_phase(rgen));
  }

  // Build the golden sequence.
  unsigned nof_clipped_sps_gold = 0;
  for (size_t i = 0; i != nof_samples; ++i) {
    if (std::abs(input[i]) > clip_threshold) {
      output_gold[i] = std::polar(clip_threshold, std::arg(input[i]));
      ++nof_clipped_sps_gold;
    } else {
      output_gold[i] = input[i];
    }
  }
  // Clip with the tested function.
  unsigned nof_clipped_sps = srsvec::clip(input, output, clip_threshold);

  // Determine the amount of error.
  for (size_t i = 0; i != nof_samples; ++i) {
    float err = std::abs(output_gold[i] - output[i]);
    TESTASSERT(err < ASSERT_MAX_ERROR);
    TESTASSERT(nof_clipped_sps == nof_clipped_sps_gold,
               "number of clipped samples ({}) should be {}.",
               nof_clipped_sps,
               nof_clipped_sps_gold);
  }
}

int main()
{
  std::vector<std::size_t> sizes           = {1, 5, 7, 19, 23, 257};
  std::vector<float>       max_amplitudes  = {0.1F, 0.5F, 2.0F};
  std::vector<float>       clip_thresholds = {0.01F, 0.3F, 2.1F};

  for (std::size_t size : sizes) {
    for (float max_amp : max_amplitudes) {
      for (float thr : clip_thresholds) {
        test_clip_function(size, max_amp, thr);
      }
    }
  }
}