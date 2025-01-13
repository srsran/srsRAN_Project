/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
/// \brief Generation of PUCCH orthogonal sequences.

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/phy/constants.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/support/math/math_utils.h"
#include "srsran/support/srsran_assert.h"
#include <array>

namespace srsran {

/// Generator of orthogonal sequences \e w for PUCCH Format 1.
class pucch_orthogonal_sequence_format1
{
public:
  /// Constructor: builds the sequences \e w from the coefficients in \ref pucch_format1_phi.
  pucch_orthogonal_sequence_format1()
  {
    for (unsigned n_pucch = 0, max_n_pucch = pucch_constants::FORMAT1_N_MAX; n_pucch != max_n_pucch; ++n_pucch) {
      for (unsigned i_seq = 0; i_seq != pucch_constants::FORMAT1_N_MAX; ++i_seq) {
        for (unsigned i_term = 0; i_term != pucch_constants::FORMAT1_N_MAX; ++i_term) {
          auto rho                                    = static_cast<float>(pucch_format1_phi[n_pucch][i_seq][i_term]);
          orthogonal_sequence[n_pucch][i_seq][i_term] = std::polar(1.0F, TWOPI * rho / static_cast<float>(n_pucch + 1));
          orthogonal_sequence_conj[n_pucch][i_seq][i_term] = std::conj(orthogonal_sequence[n_pucch][i_seq][i_term]);
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
    srsran_assert(n_pucch >= 1 && n_pucch <= pucch_constants::FORMAT1_N_MAX,
                  "Invalid n_pucch {}: valid values from 1 to {}.",
                  n_pucch,
                  pucch_constants::FORMAT1_N_MAX);
    srsran_assert(i < n_pucch, "Invalid sequence index i = {}, valid values from 0 to {}.", i, n_pucch - 1);
    srsran_assert(m < n_pucch, "Invalid value index m = {}, valid values from 0 to {}.", m, n_pucch - 1);

    return orthogonal_sequence[n_pucch - 1][i][m];
  }

  /// \brief Gets an entire PUCCH Format 1 conjugated orthogonal sequence.
  ///
  /// \param[in] n_pucch   Length of the PUCCH Format 1 sequence
  ///                      \f$N_{\text{SF},m'}^{\text{PUCCH},1} \in \{1, \dots, 7\}\f$.
  /// \param[in] i         Sequence index \f$i \in \{0, \dots, N_{\text{SF},m'}^{\text{PUCCH},1} - 1\}\f$.
  /// \returns The requested conjugated sequence.
  /// \warning An assertion is thrown if the inputs do not match the limits above.
  span<const cf_t> get_sequence_conj(unsigned n_pucch, unsigned i) const
  {
    srsran_assert(n_pucch >= 1 && n_pucch <= pucch_constants::FORMAT1_N_MAX,
                  "Invalid n_pucch {}: valid values from 1 to {}.",
                  n_pucch,
                  pucch_constants::FORMAT1_N_MAX);
    srsran_assert(i < n_pucch, "Invalid sequence index i = {}, valid values from 0 to {}.", i, n_pucch - 1);

    return span<const cf_t>(orthogonal_sequence_conj[n_pucch - 1][i]).first(n_pucch);
  }

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
  /// Table with the conjugated sequences \e w.
  w_array orthogonal_sequence_conj;
};

/// Generator of orthogonal sequences \e w for PUCCH Format 4.
class pucch_orthogonal_sequence_format4
{
public:
  /// \brief Gets an entire PUCCH Format 4 orthogonal sequence.
  ///
  /// \param[in] n_pucch   Length of the PUCCH Format 4 sequence
  ///                      \f$N_{\text{SF},m'}^{\text{PUCCH},4} \in \{2, 4\}\f$.
  /// \param[in] i         Sequence index \f$i \in \{0, \dots, N_{\text{SF},m'}^{\text{PUCCH},4} - 1\}\f$.
  /// \returns The requested sequence.
  /// \warning An assertion is thrown if the inputs do not match the limits above.
  static span<const cf_t> get_sequence(unsigned n_pucch, unsigned i)
  {
    srsran_assert((n_pucch == 2) || (n_pucch == 4), "Invalid n_pucch {}: valid values 2 or 4.", n_pucch);
    srsran_assert(i < n_pucch, "Invalid sequence index i = {}, valid values from 0 to {}.", i, n_pucch - 1);

    if (n_pucch == 2) {
      return pucch_format4_length2[i];
    }
    return pucch_format4_length4[i];
  }

private:
  static constexpr auto one     = cf_t(1.0F, 0.0F);
  static constexpr auto neg_one = cf_t(-1.0F, 0.0F);
  static constexpr auto j       = cf_t(0.0F, 1.0F);
  static constexpr auto neg_j   = cf_t(0.0F, -1.0F);

  static constexpr std::array<std::array<cf_t, NRE>, 2> pucch_format4_length2 = {{
      {one, one, one, one, one, one, one, one, one, one, one, one},
      {one, one, one, one, one, one, neg_one, neg_one, neg_one, neg_one, neg_one, neg_one},
  }};

  static constexpr std::array<std::array<cf_t, NRE>, 4> pucch_format4_length4 = {{
      {one, one, one, one, one, one, one, one, one, one, one, one},
      {one, one, one, neg_j, neg_j, neg_j, neg_one, neg_one, neg_one, j, j, j},
      {one, one, one, neg_one, neg_one, neg_one, one, one, one, neg_one, neg_one, neg_one},
      {one, one, one, j, j, j, neg_one, neg_one, neg_one, neg_j, neg_j, neg_j},
  }};
};

} // namespace srsran
