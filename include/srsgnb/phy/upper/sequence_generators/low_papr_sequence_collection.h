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

#include "srsgnb/phy/upper/sequence_generators/low_papr_sequence_generator.h"
#include "srsgnb/support/math_utils.h"
#include "srsgnb/support/srsran_assert.h"
#include "srsgnb/phy/constants.h"
#include <map>

namespace srsgnb {

/// \brief Describes a low PAPR sequence collection as per TS 38.211 section 5.2.2.
///
/// This class describes a collection of \f$r^{(\alpha, \beta)}_{u,v}\f$ sequences. The collection of sequences is
/// generated at construction time. The constructor is not suitable for real-time operation while the sequence retrieval
/// through method get() is suitable for real-time execution.
///
class low_papr_sequence_collection
{
private:
  /// Stores generated signals indexed by \c u, \c v and \f$\alpha\f$ index.
  std::map<std::tuple<unsigned, unsigned, unsigned>, std::vector<cf_t> > pregen_signals;

public:
  /// Defines the number of sequence groups (u).
  static constexpr unsigned NOF_GROUPS = 30;

  /// Defines the number of sequence bases (v).
  static constexpr unsigned MAX_NOF_BASES = 2;

  /// \brief Initialize the sequence generator with the length and an alpha list.
  ///
  /// This method generates a collection of low PAPR sequences \f$r^{(\alpha, \beta)}_{u,v}\f$ as per TS 38.211
  /// section 5.2.2 that can be later accessed by the method get(). The sequence length is
  /// \f$M_{ZC}=mN^{RB}_{sc}/2^\delta\f$.
  ///
  /// \param[in] generator Provides the low PAPR sequence generator.
  /// \param[in] m Indicates the parameter \f$m\f$.
  /// \param[in] delta Indicates the parameter \f$\delta\f$.
  /// \param[in] alphas Provides a list with the possible \f$\alpha\f$.
  low_papr_sequence_collection(const low_papr_sequence_generator& generator,
                               unsigned                           m,
                               unsigned                           delta,
                               span<const float>                  alphas)
  {
    // Calculate sequence length.
    unsigned M_zc = (NRE * m) / pow2(delta);

    // Calculate number of bases.
    unsigned nof_bases = MAX_NOF_BASES;
    if ((NRE / 2) <= M_zc && M_zc <= (5 * NRE)) {
      nof_bases = 1;
    }

    // Create sequence collection for all possible of u, v and alpha index values.
    for (unsigned u = 0; u != NOF_GROUPS; ++u) {
      for (unsigned v = 0; v != nof_bases; ++v) {
        for (unsigned alpha_idx = 0; alpha_idx != alphas.size(); ++alpha_idx) {
          // Allocate signal storage.
          pregen_signals[{u, v, alpha_idx}] = std::vector<cf_t>(M_zc);

          // Generate a sequence for each alpha.
          generator.generate(pregen_signals[{u, v, alpha_idx}], u, v, alphas[alpha_idx]);
        }
      }
    }
  }

  /// \brief Get the sequence \f$r^{(\alpha, \beta)}_{u,v}\f$ corresponding to the given indices.
  ///
  /// \param[in] u Indicates the sequence group {0...29}.
  /// \param[in] v Indicates the sequence base {0,1}.
  /// \param[in] alpha_idx Indicates an \f$\alpha\f$ index from the ones provided in the constructor.
  /// \return A view of the generated signal.
  /// \attention Requesting an invalid \c u, \c v and \c alpha_idx terminates the execution.
  span<const cf_t> get(unsigned u, unsigned v, unsigned alpha_idx) const
  {
    std::tuple<unsigned, unsigned, unsigned> key{u, v, alpha_idx};

    // Make sure the sequence exists.
    srsran_assert(pregen_signals.count(key),
                  "The sequence collection was not initialized with u=%d, v=%d and alpha_idx=%d",
                  u,
                  v,
                  alpha_idx);

    // Return the vector view of the sequence.
    return pregen_signals.at(key);
  };
};

} // namespace srsgnb
