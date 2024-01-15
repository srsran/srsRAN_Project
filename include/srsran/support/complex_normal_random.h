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
/// \brief Complex normal distribution.

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/support/srsran_assert.h"
#include <random>

namespace srsran {

/// \brief Complex normal distribution.
///
/// Generates random complex numbers according to the [complex normal (or Gaussian)
/// random number distribution](https://en.wikipedia.org/wiki/Complex_normal_distribution), circularly symmetric when
/// centered: the real and imaginary parts of the generated numbers are, after centering, independent and identically
/// distributed real-valued normal random variables.
///
/// \tparam ComplexType Complex type of the generated random values.
template <typename ComplexType = cf_t>
class complex_normal_distribution
{
  // NB: complex types are always floating point types.
  static_assert(is_complex<ComplexType>::value, "result_type must be a complex floating point type.");

public:
  /// Type of the generated complex numbers.
  using result_type = ComplexType;
  /// Type of the standard deviation, as well as of the real and imaginary parts.
  using stddev_type = typename ComplexType::value_type;

  /// \brief Parameter set.
  ///
  /// A complex normal distribution is parametrized by its mean and its standard deviation (the square root of the
  /// variance).
  struct param_type {
    using distribution_type = complex_normal_distribution<ComplexType>;

    /// Default constructor (zero mean and unitary variance).
    param_type() : param_type(ComplexType(0, 0)) {}

    /// \brief Constructor: Set mean and standard deviation.
    ///
    /// \param[in] m   Distribution mean.
    /// \param[in] sd  Distribution standard deviation, defaults to 1 if omitted. Must be positive valued.
    explicit param_type(result_type m, stddev_type sd = stddev_type(1)) : mean_v(m), stddev_v(sd)
    {
      srsran_assert(stddev_v > stddev_type(0), "Standard deviation should be positive, given {}.", stddev_v);
    }

    /// Returns the mean.
    result_type mean() const { return mean_v; }

    /// Returns the standard deviation.
    stddev_type stddev() const { return stddev_v; }

    /// Checks whether two parameter sets are the same.
    friend bool operator==(const param_type& p1, const param_type& p2)
    {
      return ((p1.mean_v == p2.mean_v) && (p1.stddev_v == p2.stddev_v));
    }

    /// Checks whether two parameter sets are different.
    friend bool operator!=(const param_type& p1, const param_type& p2) { return !(p1 == p2); }

  private:
    result_type mean_v;
    stddev_type stddev_v;
  };

  /// Default constructor (standard complex normal with zero mean and unitary variance).
  complex_normal_distribution() : complex_normal_distribution(0.0) {}

  /// Constructor: sets the distribution parameters to the given values.
  explicit complex_normal_distribution(result_type mean_v, stddev_type stddev_v = stddev_type(1)) :
    parameters(mean_v, stddev_v)
  {
  }

  /// Constructor: sets the distribution parameters to the given parameter set.
  explicit complex_normal_distribution(const param_type& params) : parameters(params) {}

  /// Resets the internal state of the distribution.
  void reset() { real_normal.reset(); }

  /// Returns the distribution mean.
  result_type mean() const { return parameters.mean(); }

  /// Returns the distribution standard deviation.
  stddev_type stddev() const { return parameters.stddev(); }

  /// Returns the distribution parameter set.
  param_type param() const { return parameters; }

  /// Sets the distribution parameters to the given parameter set.
  void param(const param_type& pp) { parameters = pp; }

  /// \brief Generates the next random number in the distribution.
  template <typename UniformRandomNumberGenerator>
  result_type operator()(UniformRandomNumberGenerator& urng)
  {
    return this->operator()(urng, parameters);
  }

  /// \brief Generates the next random number in the distribution with a different parameter set.
  template <typename UniformRandomNumberGenerator>
  result_type operator()(UniformRandomNumberGenerator& urng, const param_type& pp)
  {
    // Square root of 1/2, for variance normalization.
    static const stddev_type SQRT1_2 = 1 / std::sqrt(2.0);

    result_type tmp(real_normal(urng), real_normal(urng));
    return (tmp * pp.stddev() * SQRT1_2 + pp.mean());
  }

  /// Checks whether two complex normal distributions are the same.
  template <typename ComplexType1>
  friend bool operator==(const complex_normal_distribution<ComplexType1>& distr1,
                         const complex_normal_distribution<ComplexType1>& distr2);

private:
  /// Internal parameter set of the distribution.
  param_type parameters;
  /// Real-valued normal distribution used to generate real and imaginary parts.
  std::normal_distribution<stddev_type> real_normal = {};
};

// Declared as friend inside complex_normal_distribution.
template <typename ComplexType1>
bool operator==(const complex_normal_distribution<ComplexType1>& distr1,
                const complex_normal_distribution<ComplexType1>& distr2)
{
  return ((distr1.parameters == distr2.parameters) && (distr1.real_normal == distr2.real_normal));
}

/// Checks whether two complex normal distributions are different.
template <typename ComplexType>
bool operator!=(const complex_normal_distribution<ComplexType>& distr1,
                const complex_normal_distribution<ComplexType>& distr2)
{
  return !(distr1 == distr2);
}

} // namespace srsran
