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

#include "srsran/adt/complex.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/support/re_buffer.h"
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

  /// \brief Maximum number of OFDM symbols containing DM-RS on a PDSCH allocation
  ///
  /// It corresponds to a single symbol DM-RS with three additional positions, or to a double symbol DM-RS with one
  /// additional position, as per TS38.211 Section 7.4.1.1.2. and TS38.214 Section 5.6.1.2.
  static constexpr unsigned MAX_DMRS_SYMBOLS = 4;

  /// Defines generation parameters.
  struct params_t {
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

  /// \brief Applies the corresponding set of CDM codes to the DM-RS sequence.
  ///
  /// \param[out] sequence The DM-RS sequence after the application of CDM.
  /// \param[in] base_sequence The original DM-RS sequence.
  /// \param[in] dmrs_port DM-RS port number, determining the CDM code to apply.
  /// \param[in] symbol OFDM symbol corresponding to the input DM-RS sequence.
  /// \param[in] config PDSCH DM-RS configuration parameters.
  static void apply_cdm(span<cf_t>       sequence,
                        span<const cf_t> base_sequence,
                        unsigned         dmrs_port,
                        unsigned         symbol,
                        const config_t&  config);

  /// Temporal resource element storage.
  static_re_buffer<MAX_PORTS, MAX_DMRS_PER_SYMBOL * MAX_DMRS_SYMBOLS> temp_re;

public:
  dmrs_pdsch_processor_impl(std::unique_ptr<pseudo_random_generator> pseudo_random_generator) :
    prg(std::move(pseudo_random_generator))
  {
    srsran_assert(prg, "Invalid PRG.");
  }

  // See interface for documentation.
  void map(resource_grid_mapper& mapper, const config_t& config) override;
};

} // namespace srsran
