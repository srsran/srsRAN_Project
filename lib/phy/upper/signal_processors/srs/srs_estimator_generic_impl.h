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

#include "srsran/phy/constants.h"
#include "srsran/phy/support/time_alignment_estimator/time_alignment_estimator.h"
#include "srsran/phy/upper/sequence_generators/low_papr_sequence_generator.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator.h"
#include <memory>

namespace srsran {

/// Implements a generic Sounding Reference Signal estimator.
class srs_estimator_generic_impl : public srs_estimator
{
public:
  /// Collects the component dependencies.
  struct dependencies {
    /// Sequence generator.
    std::unique_ptr<low_papr_sequence_generator> sequence_generator;
    /// Time alignment estimator.
    std::unique_ptr<time_alignment_estimator> ta_estimator;
  };

  srs_estimator_generic_impl(dependencies deps_) : deps(std::move(deps_))
  {
    srsran_assert(deps.sequence_generator, "Invalid sequence generator.");
    srsran_assert(deps.ta_estimator, "Invalid TA estimator.");
  }

  srs_estimator_result estimate(const resource_grid_reader& grid, const srs_estimator_configuration& config) override;

private:
  /// \brief Maximum sequence length in subcarriers.
  ///
  /// It is given by the maximum value of \f$m_{SRS,0}\f$ in TS38.211 Table 6.4.1.4.3-1 and a comb size of 2.
  static constexpr unsigned max_seq_length = 272 * NRE / 2;

  static constexpr unsigned max_symbol_size = MAX_RB * NRE;

  /// \brief Generates the SRS sequence allocation mask.
  ///
  /// The mask starts at the first subcarrier used for mapping the SRS sequence.
  ///
  /// \param comb_size          Comb size, parameter \f$K_{TC}\f$.
  /// \param sequence_length    Sequence length, parameter \f$M_{sc,b}^{SRS}\f$.
  /// \return The SRS sequence allocation mask.
  static bounded_bitset<max_symbol_size> generate_mask(unsigned comb_size, unsigned sequence_length);

  /// Dependencies collection.
  dependencies deps;
};

} // namespace srsran