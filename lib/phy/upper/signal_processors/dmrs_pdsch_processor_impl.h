/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/adt/complex.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/phy/upper/signal_processors/dmrs_pdsch_processor.h"
#include "srsran/support/math_utils.h"

namespace srsran {

/// Defines a DMRS for PDSCH generic implementation.
class dmrs_pdsch_processor_impl : public dmrs_pdsch_processor
{
private:
  /// Defines the maximum number of DMRS per RB.
  static constexpr unsigned MAX_DRMS_PER_RB =
      std::max(dmrs_type(dmrs_type::TYPE1).nof_dmrs_per_rb(), dmrs_type(dmrs_type::TYPE2).nof_dmrs_per_rb());

  /// Define the maximum number of DMRS per symbol.
  static constexpr unsigned MAX_DMRS_PER_SYMBOL = MAX_RB * MAX_DRMS_PER_RB;

  /// Defines generation parameters.
  struct params_t {
    unsigned             delta;
    std::array<float, 2> w_f;
    std::array<float, 2> w_t;
  };

  /// Provides TS 38.211 Table 7.4.1.1.2-1: Parameters for PDSCH DM-RS configuration type 1.
  static const std::array<params_t, 8> params_type1;

  /// Provides TS 38.211 Table 7.4.1.1.2-2: Parameters for PDSCH DM-RS configuration type 2.
  static const std::array<params_t, 12> params_type2;

  /// Pseudo-random sequence generator instance.
  std::unique_ptr<pseudo_random_generator> prg;

  /// \brief Implements TS 38.211 section 7.4.1.1.1 Sequence generation.
  ///
  /// This method generates the sequence described in TS 38.211 section 7.4.1.1.1, considering the only values required
  /// in TS 38.211 section 7.4.1.1.2.
  ///
  /// \param[out] sequence Provides the sequence destination.
  /// \param[in] symbol Denotes the symbol index.
  /// \param[in] config Provides the required parameters to calculate the sequences.
  void sequence_generation(span<cf_t> sequence, unsigned symbol, const config_t& config) const;

  /// \brief Implements TS 38.211 section 7.4.1.1.2 Mapping to physical resources.
  ///
  /// This method implements the signal mapping as described in TS 38.211 section 7.4.1.1.2. In addition, it applies
  /// precoding if configured.
  ///
  /// \param[out] grid        Grid destination to map the signal.
  /// \param[in] sequence     Generated sequence for the given symbol.
  /// \param[in] rg_subc_mask Subcarriers where \c sequence is mapped onto, as a boolean mask.
  /// \param[in] symbol       Symbol index.
  /// \param[in] config       Configuration parameters.
  /// \note The method expects \c base_mask to start with \c NRE entries of zero-padding (i.e., set to \c false).
  void mapping(resource_grid_writer&               grid,
               span<const cf_t>                    sequence,
               const bounded_bitset<MAX_RB * NRE>& rg_subc_mask,
               unsigned                            symbol,
               const config_t&                     config);

  /// Temporal resource element storage, just in case weights or precoding is applied for each port.
  std::array<std::array<cf_t, MAX_DMRS_PER_SYMBOL>, MAX_PORTS> static_temp_re;

public:
  dmrs_pdsch_processor_impl(std::unique_ptr<pseudo_random_generator> pseudo_random_generator) :
    prg(std::move(pseudo_random_generator))
  {
    srsran_assert(prg, "Invalid PRG.");
  }

  // See interface for documentation.
  void map(resource_grid_writer& grid, const config_t& config) override;
};

} // namespace srsran
