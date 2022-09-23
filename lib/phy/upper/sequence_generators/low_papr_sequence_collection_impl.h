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

#include "srsgnb/adt/tensor.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/phy/upper/sequence_generators/low_papr_sequence_collection.h"
#include "srsgnb/phy/upper/sequence_generators/low_papr_sequence_generator.h"

namespace srsgnb {

/// Pre-generates and collects low PAPR sequences.
class low_papr_sequence_collection_impl : public low_papr_sequence_collection
{
public:
  /// \brief Constructor: Initializes the sequence generator with the given list of \f$\alpha\f$ indices and the proper
  /// length.
  ///
  /// This method generates a collection of low PAPR sequences \f$r^{(\alpha, \delta)}_{u,v}\f$ as per TS38.211
  /// Section 5.2.2 that can be later accessed by the method get(). The sequence length is
  /// \f$M_{ZC}=m \cdot N^{RB}_{sc}/2^\delta\f$.
  ///
  /// \param[in] generator Low PAPR sequence generator.
  /// \param[in] m         Parameter \f$m\f$.
  /// \param[in] delta     Parameter \f$\delta\f$.
  /// \param[in] alphas    List with the possible \f$\alpha\f$.
  low_papr_sequence_collection_impl(const low_papr_sequence_generator& generator,
                                    unsigned                           m,
                                    unsigned                           delta,
                                    span<const float>                  alphas);

  // See interface for documentation.
  span<const cf_t> get(unsigned u, unsigned v, unsigned alpha_idx) const override;

private:
  /// Stores generated signals indexed by sample, group \f$u\f$, base sequence \f$v\f$ and parameter \f$\alpha\f$
  /// indexes.
  dynamic_tensor<4, cf_t> pregen_signals;
};

} // namespace srsgnb
