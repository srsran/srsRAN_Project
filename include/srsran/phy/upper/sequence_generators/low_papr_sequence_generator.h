/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"

namespace srsran {

/// Describes a low PAPR sequence generator interface that implements TS38.211 Section 5.2.2 for any possible length.
class low_papr_sequence_generator
{
public:
  /// Default destructor.
  virtual ~low_papr_sequence_generator() = default;

  /// \brief Generates a \f$r^{(\alpha, \delta)}_{u,v}\f$ sequence as per TS38.211 Section 5.2.2.
  ///
  /// The cyclic shift \f$\alpha=2\pi\frac{n_{cs}}{n_{cs}^{max}}\f$ is split into the numerator and denominator.
  ///
  /// \param[out] sequence Provides the destination buffer and the sequence length.
  /// \param[in] u         Sequence group {0, ..., 29}.
  /// \param[in] v         Sequence number {0, 1}.
  /// \param[in] alpha_num Cyclic shift numerator, parameter \f$n_{cs}\f$.
  /// \param[in] alpha_den Cyclic shift denominator, parameter \f$n_{cs}^{max}\f$.
  virtual void generate(span<cf_t> sequence, unsigned u, unsigned v, unsigned alpha_num, unsigned alpha_den) = 0;
};

} // namespace srsran
