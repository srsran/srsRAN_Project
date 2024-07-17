/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Generation of PUCCH Format 1 orthogonal sequences.

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/support/math_utils.h"
#include "srsran/support/srsran_assert.h"
#include <array>

namespace srsran {

/// Generator of orthogonal sequences \e w for PUCCH Format 1.
class pucch_orthogonal_sequence
{
private:
  /// Alias for sequence table type.
  using w_array =
      std::array<std::array<std::array<cf_t, pucch_constants::FORMAT1_N_MAX>, pucch_constants::FORMAT1_N_MAX>,
                 pucch_constants::FORMAT1_N_MAX>;

  /// \brief Coefficients \f$\phi(m)\f$ for sequence generation.
  ///
  /// See TS38.211 Table 6.3.2.4.1-2: Orthogonal sequences for PUCCH Format 1.
  static constexpr std::array<
      std::array<std::array<unsigned, pucch_constants::FORMAT1_N_MAX>, pucch_constants::FORMAT1_N_MAX>,
      pucch_constants::FORMAT1_N_MAX>
      pucch_format1_phi = {
          {{{{0}, {}, {}, {}, {}, {}, {}}},
           {{{0, 0}, {0, 1}, {}, {}, {}, {}, {}}},
           {{{0, 0, 0}, {0, 1, 2}, {0, 2, 1}, {}, {}, {}, {}}},
           {{{0, 0, 0, 0}, {0, 2, 0, 2}, {0, 0, 2, 2}, {0, 2, 2, 0}, {}, {}, {}}},
           {{{0, 0, 0, 0, 0}, {0, 1, 2, 3, 4}, {0, 2, 4, 1, 3}, {0, 3, 1, 4, 2}, {0, 4, 3, 2, 1}, {}, {}}},
           {{{0, 0, 0, 0, 0, 0},
             {0, 1, 2, 3, 4, 5},
             {0, 2, 4, 0, 2, 4},
             {0, 3, 0, 3, 0, 3},
             {0, 4, 2, 0, 4, 2},
             {0, 5, 4, 3, 2, 1},
             {}}},
           {{{0, 0, 0, 0, 0, 0, 0},
             {0, 1, 2, 3, 4, 5, 6},
             {0, 2, 4, 6, 1, 3, 5},
             {0, 3, 6, 2, 5, 1, 4},
             {0, 4, 1, 5, 2, 6, 3},
             {0, 5, 3, 1, 6, 4, 2},
             {0, 6, 5, 4, 3, 2, 1}}}}};

  /// Table with the actual sequences \e w.
  w_array orthogonal_sequence;

public:
  /// Constructor: builds the sequences \e w from the coefficients in \ref pucch_format1_phi.
  pucch_orthogonal_sequence()
  {
    for (unsigned n_pucch = 0, max_n_pucch = pucch_constants::FORMAT1_N_MAX; n_pucch != max_n_pucch; ++n_pucch) {
      for (unsigned i_seq = 0; i_seq != pucch_constants::FORMAT1_N_MAX; ++i_seq) {
        for (unsigned i_term = 0; i_term != pucch_constants::FORMAT1_N_MAX; ++i_term) {
          auto rho                                    = static_cast<float>(pucch_format1_phi[n_pucch][i_seq][i_term]);
          orthogonal_sequence[n_pucch][i_seq][i_term] = std::polar(1.0F, TWOPI * rho / static_cast<float>(n_pucch + 1));
        }
      }
    }
  }

  /// \brief Sequence value calculator.
  ///
  /// \param[in] n_pucch   Length of the PUCCH Format 1 sequence
  ///                      \f$N_{\text{SF},m'}^{\text{PUCCH},1} \in \{1, \dots, 7\}\f$.
  /// \param[in] i         Sequence index \f$i \in \{0, \dots, N_{\text{SF},m'}^{\text{PUCCH},1} - 1\}\f$.
  /// \param[in] m         Value index inside the sequence
  ///                      \f$m \in \{0, \dots, N_{\text{SF},m'}^{\text{PUCCH},1} - 1\}\f$.
  /// \returns The requested value inside the given sequence.
  /// \warning An assertion is thrown if the indices do not match the limits above.
  cf_t get_sequence_value(unsigned n_pucch, unsigned i, unsigned m) const
  {
    srsran_assert(n_pucch >= 1 && n_pucch <= pucch_constants::FORMAT1_N_MAX, "Invalid n_pucch");
    srsran_assert(i < pucch_constants::FORMAT1_N_MAX, "Invalid i");
    srsran_assert(m < pucch_constants::FORMAT1_N_MAX, "Invalid m");

    return orthogonal_sequence[n_pucch - 1][i][m];
  }
};

} // namespace srsran
