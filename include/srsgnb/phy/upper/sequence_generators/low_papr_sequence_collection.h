/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

/// \brief Describes a low PAPR sequence collection as per TS38.211 Section 5.2.2.
///
/// This class describes a collection of \f$r^{(\alpha, \delta)}_{u,v}(n)\f$ sequences. The collection of sequences is
/// generated at construction time. The constructor is not suitable for real-time operation while the sequence retrieval
/// through method get() is suitable for real-time execution.
///
class low_papr_sequence_collection
{
public:
  /// Defines the number of sequence groups (\f$u\f$).
  static constexpr unsigned NOF_GROUPS = 30;

  /// Defines the number of sequence bases (\f$v\f$).
  static constexpr unsigned MAX_NOF_BASES = 2;

  /// Default destructor.
  virtual ~low_papr_sequence_collection() = default;

  /// \brief Get the sequence \f$r^{(\alpha, \delta)}_{u,v}(n)\f$ corresponding to the given indices.
  ///
  /// \param[in] u         Sequence group \f$u\f$ {0, ..., 29}.
  /// \param[in] v         Sequence base \f$v\f$ {0, 1}.
  /// \param[in] alpha_idx Corresponding index of parameter \f$\alpha\f$.
  /// \return A read-only view of the generated signal.
  /// \remark An assertion is triggered if any of \c u, \c v and \c alpha_idx is invalid.
  virtual span<const cf_t> get(unsigned u, unsigned v, unsigned alpha_idx) const = 0;
};

} // namespace srsgnb
