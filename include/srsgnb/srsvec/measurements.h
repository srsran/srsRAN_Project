/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SRSVEC_MEASUREMENTS_H
#define SRSGNB_SRSVEC_MEASUREMENTS_H

#include "srsgnb/srsvec/dot_prod.h"

namespace srsgnb {

namespace srsvec {

/// \brief Finds the maximum absolute value in a complex span.
/// \param[in] x Input samples.
/// \return The index of the values with the maximum absolute value in the span.
inline unsigned max_abs_element(span<const cf_t> x)
{
  unsigned max_index = 0;
  float    max_abs   = 0.0F;

  for (unsigned i = 0; i != x.size(); ++i) {
    cf_t  value = x[i];
    float abs   = value.real() * value.real() + value.imag() * value.imag();
    if (abs > max_abs) {
      max_index = i;
      max_abs   = abs;
    }
  }

  return max_index;
}

/// \brief Estimates the average power of a complex vector - linear scale.
inline float average_power(span<const cf_t> x)
{
  if (x.empty()) {
    return 0.0F;
  }

  return std::real(srsvec::dot_prod_conj(x, x, 0.0F)) / static_cast<float>(x.size());
}

} // namespace srsvec
} // namespace srsgnb

#endif // SRSGNB_SRSVEC_MEASUREMENTS_H
