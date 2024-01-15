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
/// \brief Simple utility to collect basic sample statistics.

#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <type_traits>

namespace srsran {

/// \brief Keeps track of sample statistics.
///
/// \tparam T Type (must be arithmetic) of the sample values.
template <typename T>
class sample_statistics
{
public:
  /// Estimator types depending on bias.
  enum class bias {
    /// Denotes unbiased estimators.
    UNBIASED,
    /// Denotes biased estimators.
    BIASED
  };

  /// Default constructor.
  sample_statistics() = default;

  /// Clears the content of the internal registers.
  void reset() { *this = sample_statistics(); }

  /// Updates the statistics by adding an observation to the sample.
  void update(T value)
  {
    float delta = value - mean;
    ++nof_observations;
    mean += delta / nof_observations;

    // The following update formulas are inspired by
    // Philippe Pebay, "Formulas for Robust, One-Pass Parallel Computation of Covariances and Arbitrary-Order
    // Statistical Moments," SANDIA REPORT SAND2008-6212 (2008).
    // [Online]. Available: https://www.osti.gov/servlets/purl/1028931
    float nof_obs_sqrd = static_cast<float>(nof_observations) * static_cast<float>(nof_observations);
    float delta_sqrd   = delta * delta;
    centered_power_sum_ord4 += -4 * centered_power_sum_ord3 / nof_observations * delta +
                               6 * centered_power_sum_ord2 / nof_obs_sqrd * delta_sqrd +
                               (nof_observations - 1.0F) * (nof_obs_sqrd - 3.0F * nof_observations + 3.0F) /
                                   (nof_obs_sqrd * nof_observations) * delta_sqrd * delta_sqrd;

    centered_power_sum_ord3 += -3 * centered_power_sum_ord2 / nof_observations * delta +
                               (nof_obs_sqrd - 3.0F * nof_observations + 2.0F) / nof_obs_sqrd * delta_sqrd * delta;

    centered_power_sum_ord2 += (nof_observations - 1.0F) / nof_observations * delta_sqrd;

    max = std::max(max, value);
    min = std::min(min, value);
  }

  /// \brief Returns the maximum sample value.
  /// \remark The function returns std::numeric_limits<T>::min() when the sample is empty.
  T get_max() const { return max; }

  /// \brief Returns the minimum sample value.
  /// \remark The function returns std::numeric_limits<T>::max() when the sample is empty.
  T get_min() const { return min; }

  /// Returns the sample size, i.e. the number of observations.
  size_t get_nof_observations() const { return nof_observations; }

  /// \brief Returns the sample mean.
  ///
  /// This is simply the arithmetic mean of the \f$n\f$ values in the sample.
  /// \f[
  /// \mu = \frac{1}{n} \sum_{i=0}^{n-1} x_i.
  /// \f]
  /// \remark The function returns NAN when the sample is empty.
  float get_mean() const
  {
    if (nof_observations < 1) {
      return NAN;
    }
    return mean;
  }

  /// \brief Returns the sample variance.
  ///
  /// The sample variance is given by
  /// \f[
  /// \sigma^2 = \frac{1}{m} \sum_{i=0}^{n-1} (x_i - \mu)^2
  /// \f]
  /// where \f$\mu\f$ is the [sample mean](#sample_statistics::get_mean) and where \f$m = n\f$ or \f$m = n-1\f$ for the
  /// biased and unbiased estimators, respectively, of the statistical variance
  /// \f$\EV\Bigl[\bigl(X - \EV[X]\bigr)^2\Bigr]\f$.
  /// \remark The function returns NAN when the sample consists of less than two observations.
  float get_variance(bias biased = bias::UNBIASED) const
  {
    if (nof_observations < 2) {
      return NAN;
    }

    if (biased == bias::UNBIASED) {
      return centered_power_sum_ord2 / (nof_observations - 1);
    }
    return centered_power_sum_ord2 / nof_observations;
  }

  /// \brief Returns the sample standard deviation.
  ///
  /// This is simply the squared root of the [sample covariance](#sample_statistics::get_variance). The \c biased flag
  /// simply denotes how the sample covariance is computed.
  /// \remark The function returns NAN when the sample consists of less than two observations.
  float get_std(bias biased = bias::UNBIASED) const
  {
    if (nof_observations < 2) {
      return NAN;
    }
    return std::sqrt(get_variance(biased));
  }

  /// \brief Returns the standard error of the sample mean.
  ///
  /// The standard error of the mean is an estimation of the standard deviation of the
  /// [sample mean](#sample_statistics::get_mean) (seen itself as a random quantity):
  /// \f[
  /// \sigma_{\mu} = \frac{\sigma}{\sqrt{n}}
  /// \f]
  /// where \f$n\f$ is the sample size and \f$\sigma\f$ is the [sample covariance](#sample_statistics::get_variance).
  /// The \c biased flag simply denotes how the sample covariance is computed.
  /// \note It is implicitly assumed that the observations are statistically independent.
  /// \remark The function returns NAN when the sample consists of less than two observations.
  float get_sem(bias biased = bias::UNBIASED) const
  {
    if (nof_observations < 2) {
      return NAN;
    }
    return get_std(biased) / std::sqrt(nof_observations);
  }

  /// \brief Returns the sample skewness.
  ///
  /// The skewness of a distribution with mean \f$\mu_*\f$ and standard deviation \f$\sigma_*\f$ is defined as
  /// \f[
  /// s_* = \frac{\EV\bigl[(X-\mu_*)^3\bigr]}{\sigma_*^3}.
  /// \f]
  /// For a sample of \f$n\f$ observations with [sample mean](#sample_statistics::get_mean) \f$\mu\f$, the \em biased
  /// sample skewness is given by the Fisher&ndash;Pearson coefficient
  /// \f[
  /// g_1 = \frac{\frac{1}{n}\sum_{i=0}^{n-1}(x_i - \mu)^3}{\Bigl[\frac{1}{n}\sum_{i=0}^{n-1}(x_i - \mu)^2\Bigr]^{3/2}}.
  /// \f]
  /// The \em unbiased sample skewness is obtained by applying the adjustment
  /// \f[
  /// G_1 = \frac{\sqrt{n(n-1)}}{n-2}g_1.
  /// \f]
  /// \remark The function returns NAN when the sample consists of less than three observations.
  /// \note The skewness provides a rough indication of the symmetry of the distribution of the sample around its mean.
  /// More specifically, for unimodal distributions, a negative-valued skewness means that the left tail of the
  /// distribution is longer than the right one and most of the mass is concentrated to the right of the mean. In other
  /// words, the median is higher than the mean. A positive skewness means the opposite. Symmetric distributions have
  /// null skewness. However, the opposite is not always true as, e.g., a long left tail and a fat right tail may
  /// compensate each other and give skewness equal to zero.
  float get_skewness(bias biased = bias::UNBIASED) const
  {
    if (nof_observations < 3) {
      return NAN;
    }
    float num     = centered_power_sum_ord3 / nof_observations;
    float den_tmp = std::sqrt(centered_power_sum_ord2 / nof_observations);
    float den     = den_tmp * den_tmp * den_tmp;

    float result = num / den;

    if (biased == bias::UNBIASED) {
      result *= std::sqrt(nof_observations * (nof_observations - 1.0F)) / (nof_observations - 2.0F);
    }
    return result;
  }

  /// \brief Returns the sample kurtosis.
  ///
  /// The kurtosis of a distribution with mean \f$\mu_*\f$ and standard deviation \f$\sigma_*\f$ is defined as
  /// \f[
  /// k_* = \frac{\EV\bigl[(X-\mu_*)^4\bigr]}{\sigma_*^4}.
  /// \f]
  /// For a sample of \f$n\f$ observations with [sample mean](#sample_statistics::get_mean) \f$\mu\f$, the \em biased
  /// sample kurtosis is given by the expression
  /// \f[
  /// g_2 = \frac{\frac{1}{n}\sum_{i=0}^{n-1}(x_i - \mu)^4}{\Bigl[\frac{1}{n}\sum_{i=0}^{n-1}(x_i - \mu)^2\Bigr]^{2}}.
  /// \f]
  /// The \em unbiased sample kurtosis is obtained by applying the adjustment
  /// \f[
  /// G_2 = \frac{n-1}{(n-2)(n-3)}\bigl[(n+1)g_1 - 3(n-1)\bigr] + 3.
  /// \f]
  /// \remark The returns NAN when the sample consists of less than four observations.
  /// \note The normal distribution has kurtosis equal to 3 (for this reason, sometimes the kurtosis is defined as
  /// \f$g_2 - 3\f$ to get a value of zero with the normal distribution).
  /// \note Distributions with tails that decrease slower
  /// (faster, respectively) than those of the Gaussian distribution have a kurtosis that is larger
  /// (smaller, respectively) than 3. In other words, samples with a high kurtosis suggest that the underlying
  /// distribution has fat tails and/or that there is a significant number of outliers outside the interval \f$\mu \pm
  /// \sigma\f$.
  float get_kurtosis(bias biased = bias::UNBIASED) const
  {
    if (nof_observations < 4) {
      return NAN;
    }
    float num     = centered_power_sum_ord4 / nof_observations;
    float den_tmp = centered_power_sum_ord2 / nof_observations;
    float den     = den_tmp * den_tmp;

    float result = num / den;

    if (biased == bias::UNBIASED) {
      result = result * (nof_observations + 1) - 3.0F * (nof_observations - 1);
      result = result * (nof_observations - 1) / ((nof_observations - 2.0F) * (nof_observations - 3.0F)) + 3.0F;
    }
    return result;
  }

private:
  /// Ensure the sample type is arithmetic.
  static_assert(std::is_arithmetic<T>::value, "Invalid type.");
  /// Records the maximum observed value.
  T max = std::numeric_limits<T>::min();
  /// Records the minimum observed value.
  T min = std::numeric_limits<T>::max();
  /// Records the sample size.
  size_t nof_observations = 0;
  /// Records the mean of the sample.
  float mean = 0;
  /// Records the centered power sum of order 2 of all observations, i.e. \f$\sum_n (X_n - \bar{X})^2\f$.
  float centered_power_sum_ord2 = 0;
  /// Records the centered power sum of order 3 of all observations, i.e. \f$\sum_n (X_n - \bar{X})^3\f$.
  float centered_power_sum_ord3 = 0;
  /// Records the centered power sum of order 4 of all observations, i.e. \f$\sum_n (X_n - \bar{X})^4\f$.
  float centered_power_sum_ord4 = 0;
};

} // namespace srsran
