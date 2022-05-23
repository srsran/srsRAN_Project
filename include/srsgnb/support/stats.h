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
/// \brief Simple utility to collect basic sample statistics.

#ifndef SRSGNB_SUPPORT_STATS_H
#define SRSGNB_SUPPORT_STATS_H

#include <algorithm>
#include <cmath>
#include <limits>
#include <type_traits>

namespace srsgnb {

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

  /// Constructor: checks that the sample type is arithmetic.
  sample_statistics() { static_assert(std::is_arithmetic<T>::value, "Invalid type."); }

  /// Defalut destructor.
  ~sample_statistics() = default;

  /// Clears the content of the internal registers.
  void reset()
  {
    max = std::numeric_limits<T>::min();
    min = std::numeric_limits<T>::max();

    nof_samples               = 0;
    cumulative_sum            = 0;
    cumulative_sum_of_squares = 0;
    cumulative_sum_of_cubes   = 0;
    cumulative_sum_of_fourths = 0;
  }

  /// Adds a new value to the observed sample.
  void update(T value)
  {
    float value_float = static_cast<float>(value);
    cumulative_sum += value_float;
    float value_float_sqrd = value_float * value_float;
    cumulative_sum_of_squares += value_float_sqrd;
    cumulative_sum_of_cubes += value_float_sqrd * value_float;
    cumulative_sum_of_fourths += value_float_sqrd * value_float_sqrd;
    max = std::max(max, value);
    min = std::min(min, value);
    ++nof_samples;
  }

  /// Returns the maximum sample value.
  T get_max() const { return max; }

  /// Returns the minimum sample value.
  T get_min() const { return min; }

  /// Returns the sample size, i.e. the number of observations.
  unsigned get_nof_samples() const { return nof_samples; }

  /// \brief Returns the sample mean.
  ///
  /// This is simply the arithmetic mean of the \f$n\f$ values in the sample.
  /// \f[
  /// \mu = \frac{1}{n} \sum_{i=0}^{n-1} x_i.
  /// \f]
  float get_mean() const { return cumulative_sum / nof_samples; }

  /// \brief Returns the sample variance.
  ///
  /// The sample variance is given by
  /// \f[
  /// \sigma^2 = \frac{1}{m} \sum_{i=0}^{n-1} (x_i - \mu)^2
  /// \f]
  /// where \f$\mu\f$ is the [sample mean](#sample_statistics::get_mean) and where \f$m = n\f$ or \f$m = n-1\f$ for the
  /// biased and unbiased estimators, respectively, of the statistical variance
  /// \f$\EV\Bigl[\bigl(X - \EV[X]\bigr)^2\Bigr]\f$.
  /// \remark The behavior is undefined when the sample consists of less than two observations.
  float get_variance(bias biased = bias::UNBIASED) const
  {
    if (biased == bias::UNBIASED) {
      return (cumulative_sum_of_squares - cumulative_sum * get_mean()) / (nof_samples - 1);
    }
    return (cumulative_sum_of_squares - cumulative_sum * get_mean()) / nof_samples;
  }

  /// \brief Returns the sample standard deviation.
  ///
  /// This is simply the squared root of the [sample covariance](#sample_statistics::get_variance). The \c biased flag
  /// simply denotes how the sample covariance is computed.
  /// \remark The behavior is undefined when the sample consists of less than two observations.
  float get_std(bias biased = bias::UNBIASED) const { return std::sqrt(get_variance(biased)); }

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
  /// \remark The behavior is undefined when the sample consists of less than two observations.
  float get_sem(bias biased = bias::UNBIASED) const { return get_std(biased) / std::sqrt(nof_samples); }

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
  /// \remark The behavior is undefined when the sample consists of less than three observations.
  /// \note The skewness provides a rough indication of the symmetry of the distribution of the sample around its mean.
  /// More specifically, for unimodal distributions, a negative-valued skewness means that the left tail of the
  /// distribution is longer than the right one and most of the mass is concentrated to the right of the mean. In other
  /// words, the median is higher than the mean. A positive skewness means the opposite. Symmetric distributions have
  /// null skewness. However, the opposite is not always true as, e.g., a long left tail and a fat right tail may
  /// compensate each other and give skewness equal to zero.
  float get_skewness(bias biased = bias::UNBIASED) const
  {
    float cum_sum3 = cumulative_sum * cumulative_sum * cumulative_sum;

    // For the numerator, we need to compute the third sample centered moment "sum((x - mean)^3) / n". Denoting Sn the
    // cumulative sum of samples raised to the nth power, this is equivalent to computing
    // (S3 - 3 S1 S2 / n + 2 S1^3 / n^2) / n = (((2 S1^3) / n - 3 S1 S2) / n + S3) / n.
    float num = (2 * cum_sum3) / nof_samples;
    num       = (num - 3 * cumulative_sum * cumulative_sum_of_squares) / nof_samples;
    num       = (num + cumulative_sum_of_cubes) / nof_samples;

    // The denominator is just the biased standard deviation to the third power.
    float den = get_std(bias::BIASED);
    den       = den * den * den;

    float result = num / den;

    if (biased == bias::UNBIASED) {
      result *= std::sqrt(nof_samples * (nof_samples - 1)) / (nof_samples - 2);
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
  /// \remark The behavior is undefined when the sample consists of less than four observations.
  /// \note The normal distribution has kurtosis equal to 3 (for this reason, sometimes the kurtosis is defined as
  /// \f$g_2 - 3\f$ to get a value of zero with the normal distribution).
  /// \note Distributions with tails that decrease slower
  /// (faster, respectively) than those of the Gaussian distribution have a kurtosis that is larger
  /// (smaller, respectively) than 3. In other words, samples with a high kurtosis suggest that the underlying
  /// distribution has fat tails and/or that there is a significant number of outliers outside the interval \f$\mu \pm
  /// \sigma\f$.
  float get_kurtosis(bias biased = bias::UNBIASED) const
  {
    float cum_sum2 = cumulative_sum * cumulative_sum;
    float cum_sum4 = cum_sum2 * cum_sum2;

    // For the numerator, we need to compute the fourth sample centered moment "sum((x - mean)^4) / n". Denoting Sn the
    // cumulative sum of samples raised to the nth power, this is equivalent to computing
    // (S4 - 4 S1 S3 / n + 6 S1^2 S2 / n^2 - 3 S1^4 / n^3) / n
    //            = ((((-3 S1^4) / n + 6 S1^2 S2) / n - 4 S1 S3) / n + S4) / n.
    float num = (-3 * cum_sum4) / nof_samples;
    num       = (num + 6 * cum_sum2 * cumulative_sum_of_squares) / nof_samples;
    num       = (num - 4 * cumulative_sum * cumulative_sum_of_cubes) / nof_samples;
    num       = (num + cumulative_sum_of_fourths) / nof_samples;

    // The denominator is just the biased variance squared.
    float den = get_variance(bias::BIASED);
    den *= den;

    float result = num / den;

    if (biased == bias::UNBIASED) {
      result = result * (nof_samples + 1) - 3 * (nof_samples - 1);
      result = result * (nof_samples - 1) / ((nof_samples - 2) * (nof_samples - 3)) + 3;
    }
    return result;
  }

private:
  /// Records the maximum observed value.
  T max = std::numeric_limits<T>::min();
  /// Records the minimum observed value.
  T min = std::numeric_limits<T>::max();
  /// Records the sample size.
  unsigned nof_samples = 0;
  /// Records the sum of all observed values.
  float cumulative_sum = 0;
  /// Records the sum of the squares of all observed values.
  float cumulative_sum_of_squares = 0;
  /// Records the sum of the cubes of all observed values.
  float cumulative_sum_of_cubes = 0;
  /// Records the sum of the fourth powers of all observed values.
  float cumulative_sum_of_fourths = 0;
};

} // namespace srsgnb

#endif // SRSGNB_SUPPORT_STATS_H
