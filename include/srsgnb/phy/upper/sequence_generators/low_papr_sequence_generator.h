/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"

namespace srsgnb {

/// Describes a low PAPR sequence generator interface that implements TS38.211 Section 5.2.2 for any possible length.
class low_papr_sequence_generator
{
public:
  /// Default destructor.
  virtual ~low_papr_sequence_generator() = default;

  /// \brief Generates a \f$r^{(\alpha, \delta)}_{u,v}\f$ sequence as per TS38.211 Section 5.2.2.
  /// \param[out] sequence Provides the destination buffer and the sequence length.
  /// \param[in] u         Sequence group {0, ..., 29}.
  /// \param[in] v         Sequence number {0, 1}.
  /// \param[in] alpha     Cyclic shift (i.e., parameter \f$\alpha\f$).
  virtual void generate(span<cf_t> sequence, unsigned u, unsigned v, float alpha) const = 0;
};

} // namespace srsgnb
