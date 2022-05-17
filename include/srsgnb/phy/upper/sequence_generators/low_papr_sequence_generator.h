/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_SEQUENCE_GENERATORS_LOW_PAPR_SEQUENCE_GENERATOR_H
#define SRSGNB_PHY_UPPER_SEQUENCE_GENERATORS_LOW_PAPR_SEQUENCE_GENERATOR_H

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"

namespace srsgnb {

/// Describes a low PAPR sequence generator interface that implements TS 38.211 section 5.2.2 for any possible length.
class low_papr_sequence_generator
{
public:
  /// Default destructor.
  virtual ~low_papr_sequence_generator() = default;

  /// \brief Generates a \f$r^{(\alpha, \beta)}_{u,v}\f$ sequence as per TS 38.211 section 5.2.2.
  /// \param[out] sequence Provides the destination buffer and the sequence length.
  /// \param[in] u Indicates the sequence group {0...29}.
  /// \param[in] v Indicates the sequence base {0,1}.
  /// \param[in] alpha Indicates the \f$\alpha\f$ parameter.
  virtual void generate(span<cf_t> sequence, unsigned u, unsigned v, float alpha) const = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_SEQUENCE_GENERATORS_LOW_PAPR_SEQUENCE_GENERATOR_H
