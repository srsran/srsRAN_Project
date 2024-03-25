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

#pragma once

#include "srsran/phy/support/complex_exponential_table.h"
#include "srsran/phy/upper/sequence_generators/low_papr_sequence_generator.h"
#include <set>
#include <unordered_map>

namespace srsran {

/// \brief Describes a generic low PAPR sequence generation implementation.
///
/// \attention This generic class implementation is not optimized for real-time execution.
class low_papr_sequence_generator_impl : public low_papr_sequence_generator
{
public:
  /// Create a low PAPR sequence generator.
  low_papr_sequence_generator_impl();

  // See interface for documentation.
  void generate(span<cf_t> sequence, unsigned u, unsigned v, unsigned alpha_num, unsigned alpha_den) override;

private:
  /// Supported sequence sizes.
  static const std::set<unsigned> sequence_sizes;

  /// Defines the maximum number of short sequences.
  static constexpr unsigned NOF_ZC_SEQ = 30;

  /// Defines Phi values for M_sc=6 Table 5.2.2.2-1 in TS 38.211.
  static const std::array<std::array<int, 6>, NOF_ZC_SEQ> phi_M_sc_6;

  /// Defines Phi values for M_sc=12 Table 5.2.2.2-2 in TS 38.211.
  static const std::array<std::array<int, 12>, NOF_ZC_SEQ> phi_M_sc_12;

  /// Defines Phi values for M_sc=18 Table 5.2.2.2-3 in TS 38.211.
  static const std::array<std::array<int, 18>, NOF_ZC_SEQ> phi_M_sc_18;

  /// Defines Phi values for M_sc=24 Table 5.2.2.2-4 in TS 38.211.
  static const std::array<std::array<int, 24>, NOF_ZC_SEQ> phi_M_sc_24;

  /// \brief Gets argument for the sequence \f$r^{(\alpha, \beta)}_{u,v}(n)\f$ for base sequences of length 36 or
  /// larger.
  ///
  /// \param[in] u Sequence group index {0, 29}.
  /// \param[in] v Sequence base index {0, 1}.
  span<const int> r_uv_arg_mprb(unsigned u, unsigned v, unsigned M_zc);

  /// \brief Generates argument for the sequence \f$r^{(\alpha, \beta)}_{u,v}(n)\f$.
  ///
  /// \param[in] u    Sequence group index {0, 29}.
  /// \param[in] v    Sequence base index {0, 1}.
  /// \param[in] M_zc Sequence length, parameter \f$M_{ZC}\f$.
  span<const int> r_uv_arg(unsigned u, unsigned v, unsigned M_zc);

  /// Temporary storage of the argument for sequences of length 36 or larger.
  std::vector<int> temp_r_uv_arg;
  /// Temporary storage of the cyclic shift.
  std::vector<cf_t> temp_cyclic_shift;
  /// Complex exponential tables.
  std::unordered_map<unsigned, complex_exponential_table> tables;
  /// Cyclic shift table.
  complex_exponential_table cs_table;
};

} // namespace srsran
