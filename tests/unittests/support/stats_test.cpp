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

/// \file
/// \brief Unit test for the sample statistics class.
///
/// The test takes a sample of 100 random real values and compares the statistics provided by the sample_statistics
/// class with benchmark ones computed off line.

#include "srsran/support/srsran_test.h"
#include "srsran/support/stats.h"
#include <array>

/// \cond
bool is_within_tolerance(float a, float b)
{
  float rr = std::abs((a - b) / b);
  // Relative difference less than 0.1%.
  return rr < 0.001;
}

int main()
{
  using bias = srsran::sample_statistics<float>::bias;

  constexpr unsigned                            NOF_OBSERVATIONS = 100;
  constexpr std::array<float, NOF_OBSERVATIONS> SAMPLES          = {
               -9.70239867e-01, -1.16246488e+00, -6.15919117e-01, -2.68661833e-01, 3.63608129e-01,  -1.27996396e+00,
               -1.07055465e+00, 9.69757419e-01,  2.69440768e-01,  9.82452599e-02,  1.04926594e+00,  -2.81540376e-01,
               -3.52506821e-02, -1.06468427e+00, -1.11925195e-01, -1.64412925e+00, 3.46079357e-01,  -8.77805820e-01,
               3.10879823e-01,  2.24979364e-03,  7.03471119e-01,  3.21592989e-01,  -1.49248228e+00, 1.81830171e+00,
               -5.61604606e-01, 4.73660621e-01,  1.85460860e+00,  1.73283665e+00,  -8.38160693e-01, -1.16197125e+00,
               1.80652970e+00,  -2.90473421e-01, -9.97514187e-01, -6.56651431e-01, -2.58450830e-02, -2.84345143e-01,
               1.97413080e+00,  -6.75607267e-01, 1.08142798e+00,  1.56484369e+00,  4.25765405e-01,  -1.89206760e-01,
               -7.60136160e-01, 1.28031645e-01,  6.01499376e-02,  -7.71625502e-01, 1.25289098e+00,  -1.18299274e+00,
               1.44970511e+00,  5.04894251e-01,  -6.39663564e-01, -7.80649639e-02, 7.13491324e-01,  8.06704512e-01,
               5.19300171e-01,  8.63631179e-01,  -1.43066757e+00, 1.02154181e+00,  8.82240293e-02,  1.12694082e+00,
               -8.19356346e-01, -1.06676141e+00, -1.75314104e+00, -1.49578591e+00, 5.23322131e-01,  -2.54435075e+00,
               -2.91677491e-01, -4.64153460e-01, -6.94795537e-01, 1.54623170e-01,  7.45714035e-01,  -1.05012701e+00,
               2.25854399e-01,  -1.51013036e+00, -8.43902383e-01, -5.78443786e-01, 2.34369262e-01,  -1.00134902e-01,
               -2.09236301e-01, 1.74960656e+00,  5.26616371e-01,  -2.66952923e-02, -1.73727499e+00, 7.66674190e-02,
               1.44884486e-01,  5.03000515e-01,  4.44257589e-01,  6.68722678e-01,  -9.37277754e-01, -4.49562904e-02,
               -1.79960368e-01, 1.52587798e+00,  3.02382742e-01,  -5.97732707e-01, 1.95775408e+00,  7.29133168e-01,
               -1.53216943e+00, -4.72758032e-01, -5.14532394e-01, -1.98497960e-01};

  struct results {
    float max             = 0;
    float min             = 0;
    float mean            = 0;
    float var             = 0;
    float var_biased      = 0;
    float std             = 0;
    float std_biased      = 0;
    float sem             = 0;
    float sem_biased      = 0;
    float skew            = 0;
    float skew_biased     = 0;
    float kurtosis        = 0;
    float kurtosis_biased = 0;
  };

  // Benchmark values computed with Python numpy and scipy, after half and all of the samples.
  results half = {1.9741307997659492,
                  -1.644129246235781,
                  0.025812224711411914,
                  0.9673107823020193,
                  0.9479645666559788,
                  0.9835195891806219,
                  0.9736347193151952,
                  0.139090674187885,
                  0.13769274248528707,
                  0.40804032532619733,
                  0.39569565399184337,
                  2.2677957225375387,
                  2.221347398977466};
  results full = {1.9741307997659492,
                  -2.5443507526004048,
                  -0.04869016333314352,
                  0.9311648831478798,
                  0.921853234316401,
                  0.9649688508692288,
                  0.9601318838140941,
                  0.09649688508692288,
                  0.0960131883814094,
                  0.1318703780708755,
                  0.12988402234028384,
                  2.6646109072534867,
                  2.6217413025654213};

  srsran::sample_statistics<float> stats    = {};
  unsigned                         i_sample = 0, stop_sample = NOF_OBSERVATIONS / 2;
  for (; i_sample != stop_sample; ++i_sample) {
    stats.update(SAMPLES[i_sample]);
  }
  TESTASSERT_EQ(stats.get_nof_observations(), stop_sample, "Number of observations.");
  TESTASSERT(is_within_tolerance(stats.get_max(), half.max), "Maximum value.");
  TESTASSERT(is_within_tolerance(stats.get_min(), half.min), "Minimum value.");
  TESTASSERT(is_within_tolerance(stats.get_mean(), half.mean), "Mean.");
  TESTASSERT(is_within_tolerance(stats.get_variance(), half.var), "Variance.");
  TESTASSERT(is_within_tolerance(stats.get_variance(bias::BIASED), half.var_biased), "Biased variance.");
  TESTASSERT(is_within_tolerance(stats.get_std(), half.std), "Standard deviation.");
  TESTASSERT(is_within_tolerance(stats.get_std(bias::BIASED), half.std_biased), "Biased standard deviation.");
  TESTASSERT(is_within_tolerance(stats.get_sem(), half.sem), "Standard mean error.");
  TESTASSERT(is_within_tolerance(stats.get_sem(bias::BIASED), half.sem_biased), "Biased standard mean error.");
  TESTASSERT(is_within_tolerance(stats.get_skewness(), half.skew), "Skewness.");
  TESTASSERT(is_within_tolerance(stats.get_skewness(bias::BIASED), half.skew_biased), "Biased skewness.");
  TESTASSERT(is_within_tolerance(stats.get_kurtosis(), half.kurtosis), "Kurtosis.");
  TESTASSERT(is_within_tolerance(stats.get_kurtosis(bias::BIASED), half.kurtosis_biased), "Biased kurtosis.");

  for (; i_sample != NOF_OBSERVATIONS; ++i_sample) {
    stats.update(SAMPLES[i_sample]);
  }
  TESTASSERT_EQ(stats.get_nof_observations(), NOF_OBSERVATIONS, "Number of observations.");
  TESTASSERT(is_within_tolerance(stats.get_max(), full.max), "Maximum value.");
  TESTASSERT(is_within_tolerance(stats.get_min(), full.min), "Minimum value.");
  TESTASSERT(is_within_tolerance(stats.get_mean(), full.mean), "Mean.");
  TESTASSERT(is_within_tolerance(stats.get_variance(), full.var), "Variance.");
  TESTASSERT(is_within_tolerance(stats.get_variance(bias::BIASED), full.var_biased), "Biased variance.");
  TESTASSERT(is_within_tolerance(stats.get_std(), full.std), "Standard deviation.");
  TESTASSERT(is_within_tolerance(stats.get_std(bias::BIASED), full.std_biased), "Biased standard deviation.");
  TESTASSERT(is_within_tolerance(stats.get_sem(), full.sem), "Standard mean error.");
  TESTASSERT(is_within_tolerance(stats.get_sem(bias::BIASED), full.sem_biased), "Biased standard mean error.");
  TESTASSERT(is_within_tolerance(stats.get_skewness(), full.skew), "Skewness.");
  TESTASSERT(is_within_tolerance(stats.get_skewness(bias::BIASED), full.skew_biased), "Biased skewness.");
  TESTASSERT(is_within_tolerance(stats.get_kurtosis(), full.kurtosis), "Kurtosis.");
  TESTASSERT(is_within_tolerance(stats.get_kurtosis(bias::BIASED), full.kurtosis_biased), "Biased kurtosis.");

  stats.reset();
  // Recompute the statistics to ensure reset worked properly.
  for (i_sample = 0; i_sample != NOF_OBSERVATIONS; ++i_sample) {
    stats.update(SAMPLES[i_sample]);
  }
  TESTASSERT_EQ(stats.get_nof_observations(), NOF_OBSERVATIONS, "Number of observations.");
  TESTASSERT(is_within_tolerance(stats.get_max(), full.max), "Maximum value.");
  TESTASSERT(is_within_tolerance(stats.get_min(), full.min), "Minimum value.");
  TESTASSERT(is_within_tolerance(stats.get_mean(), full.mean), "Mean.");
  TESTASSERT(is_within_tolerance(stats.get_variance(), full.var), "Variance.");
  TESTASSERT(is_within_tolerance(stats.get_variance(bias::BIASED), full.var_biased), "Biased variance.");
  TESTASSERT(is_within_tolerance(stats.get_std(), full.std), "Standard deviation.");
  TESTASSERT(is_within_tolerance(stats.get_std(bias::BIASED), full.std_biased), "Biased standard deviation.");
  TESTASSERT(is_within_tolerance(stats.get_sem(), full.sem), "Standard mean error.");
  TESTASSERT(is_within_tolerance(stats.get_sem(bias::BIASED), full.sem_biased), "Biased standard mean error.");
  TESTASSERT(is_within_tolerance(stats.get_skewness(), full.skew), "Skewness.");
  TESTASSERT(is_within_tolerance(stats.get_skewness(bias::BIASED), full.skew_biased), "Biased skewness.");
  TESTASSERT(is_within_tolerance(stats.get_kurtosis(), full.kurtosis), "Kurtosis.");
  TESTASSERT(is_within_tolerance(stats.get_kurtosis(bias::BIASED), full.kurtosis_biased), "Biased kurtosis.");
}
/// \endcond