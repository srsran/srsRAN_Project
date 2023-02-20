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
/// \brief Unit test for the clipping functions in the \c srsvec vector library.

#include "srsran/srsvec/aligned_vec.h"
#include "srsran/srsvec/clip.h"
#include "srsran/support/srsran_test.h"
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_MAX_ERROR = 1e-6;

using namespace srsran;

static srsvec::aligned_vec<cf_t> input;
static srsvec::aligned_vec<cf_t> output;
static srsvec::aligned_vec<cf_t> output_gold;

static void setup(std::size_t nof_samples, const float max_amplitude)
{
  input.resize(nof_samples);
  output.resize(nof_samples);
  output_gold.resize(nof_samples);

  // Generate pseudorandom complex numbers.
  std::uniform_real_distribution<float> dist(-max_amplitude, max_amplitude);
  for (cf_t& v : input) {
    v = {dist(rgen), dist(rgen)};
  }
}

static void test_clip_magnitude_function(float clip_threshold)
{
  // Build the golden sequence.
  std::size_t nof_samples          = input.size();
  unsigned    nof_clipped_sps_gold = 0;
  for (std::size_t i = 0; i != nof_samples; ++i) {
    if (std::abs(input[i]) > clip_threshold) {
      output_gold[i] = std::polar(clip_threshold, std::arg(input[i]));
      ++nof_clipped_sps_gold;
    } else {
      output_gold[i] = input[i];
    }
  }

  // Clip with the tested function.
  unsigned nof_clipped_sps = srsvec::clip_magnitude(output, input, clip_threshold);

  // Determine the amount of error.
  for (std::size_t i = 0; i != nof_samples; ++i) {
    float err = std::abs(output_gold[i] - output[i]);
    TESTASSERT(err < ASSERT_MAX_ERROR);
    TESTASSERT(nof_clipped_sps == nof_clipped_sps_gold,
               "number of clipped samples ({}) should be {}.",
               nof_clipped_sps,
               nof_clipped_sps_gold);
  }
}

static void test_clip_iq_function(float clip_threshold)
{
  // Build the golden sequence.
  std::size_t nof_samples          = input.size();
  unsigned    nof_clipped_sps_gold = 0;
  for (std::size_t i = 0; i != nof_samples; ++i) {
    if (input[i].real() > clip_threshold) {
      output_gold[i].real(clip_threshold);
      ++nof_clipped_sps_gold;
    } else if (input[i].real() < -clip_threshold) {
      output_gold[i].real(-clip_threshold);
      ++nof_clipped_sps_gold;
    } else {
      output_gold[i].real(input[i].real());
    }

    if (input[i].imag() > clip_threshold) {
      output_gold[i].imag(clip_threshold);
      ++nof_clipped_sps_gold;
    } else if (input[i].imag() < -clip_threshold) {
      output_gold[i].imag(-clip_threshold);
      ++nof_clipped_sps_gold;
    } else {
      output_gold[i].imag(input[i].imag());
    }
  }

  // Clip with the tested function.
  unsigned nof_clipped_sps = srsvec::clip_iq(output, input, clip_threshold);

  // Determine the amount of error.
  for (std::size_t i = 0; i != nof_samples; ++i) {
    float err = std::abs(output_gold[i] - output[i]);
    TESTASSERT(err < ASSERT_MAX_ERROR);
    TESTASSERT(nof_clipped_sps == nof_clipped_sps_gold,
               "number of clipped samples ({}) should be {}.",
               nof_clipped_sps,
               nof_clipped_sps_gold);
  }
}

static void test_clip_function(float clip_threshold)
{
  // Reinterpret the input, output and golden vectors as floating point.
  std::size_t       nof_fp_samples = input.size() * 2;
  span<const float> input_fp       = span<const float>(reinterpret_cast<const float*>(input.data()), nof_fp_samples);
  span<float>       output_fp      = span<float>(reinterpret_cast<float*>(output.data()), nof_fp_samples);
  span<float>       output_gold_fp = span<float>(reinterpret_cast<float*>(output_gold.data()), nof_fp_samples);

  // Build the golden sequence.
  unsigned nof_clipped_sps_gold = 0;
  for (std::size_t i = 0; i != nof_fp_samples; ++i) {
    if (input_fp[i] > clip_threshold) {
      output_gold_fp[i] = clip_threshold;
      ++nof_clipped_sps_gold;
    } else if (input_fp[i] < -clip_threshold) {
      output_gold_fp[i] = -clip_threshold;
      ++nof_clipped_sps_gold;
    } else {
      output_gold_fp[i] = input_fp[i];
    }
  }

  // Clip with the tested function.
  unsigned nof_clipped_sps = srsvec::clip(output_fp, input_fp, clip_threshold);

  // Determine the amount of error.
  for (std::size_t i = 0; i != nof_fp_samples; ++i) {
    float err = std::abs(output_gold_fp[i] - output_fp[i]);
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
    for (const float max_amp : max_amplitudes) {
      for (const float thr : clip_thresholds) {
        // Generate the input signals.
        setup(size, max_amp);

        // Test the floating point clipping function.
        test_clip_function(thr);

        // Test the IQ clipping function.
        test_clip_iq_function(thr);

        // Test the magnitude clipping function.
        test_clip_magnitude_function(thr);
      }
    }
  }
}
