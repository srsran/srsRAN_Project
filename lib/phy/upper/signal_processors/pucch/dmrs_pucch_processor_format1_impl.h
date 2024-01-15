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

#include "pucch_helper.h"
#include "pucch_orthogonal_sequence.h"
#include "srsran/phy/upper/sequence_generators/low_papr_sequence_collection.h"
#include "srsran/phy/upper/signal_processors/dmrs_pucch_processor.h"
#include "srsran/phy/upper/signal_processors/port_channel_estimator.h"

namespace srsran {

/// Generic implementation of a DM-RS channel estimator for PUCCH Format 1.
class dmrs_pucch_processor_format1_impl : public dmrs_pucch_processor
{
public:
  using layer_dmrs_pattern = port_channel_estimator::layer_dmrs_pattern;

  dmrs_pucch_processor_format1_impl(std::unique_ptr<pseudo_random_generator>            prg_,
                                    std::unique_ptr<const low_papr_sequence_collection> sequence_collection_,
                                    std::unique_ptr<port_channel_estimator>             ch_est_) :
    helper(std::move(prg_)), sequence_collection(std::move(sequence_collection_)), ch_estimator(std::move(ch_est_))
  {
    srsran_assert(sequence_collection, "Invalid sequence collection.");
    srsran_assert(ch_estimator, "Invalid port channel estimator.");
  }

  // See interface for documentation.
  void estimate(channel_estimate& ce_, const resource_grid_reader& grid, const config_t& config) override;

private:
  pucch_helper helper;

  /// PUCCH Format 1 implementation expects pre-generated sequence collection on instantiation.
  std::unique_ptr<const low_papr_sequence_collection> sequence_collection;
  /// Antenna port channel estimator.
  std::unique_ptr<port_channel_estimator> ch_estimator;

  /// Buffer for DM-RS symbols.
  dmrs_symbol_list temp_symbols;
  /// Pre-generated orthogonal cover code.
  const pucch_orthogonal_sequence occ;

  /// Holds DM-RS sequence generation parameters used in TS38.211 Section 6.4.1.3.1.1.
  struct sequence_generation_config {
    // Sequence group.
    unsigned u;
    // Sequence number.
    unsigned v;
    // DM-RS symbol index.
    unsigned m;
    // Total number of DM-RS symbols
    unsigned n_pucch;
  };

  /// \brief Generates a DM-RS sequence according to TS38.211 Section 6.4.1.3.1.1.
  ///
  /// \param[out] sequence     Sequence destination buffer.
  /// \param[in]  pucch_config PUCCH configuration.
  /// \param[in]  gen_config   Additional parameters required to calculate the sequences.
  /// \param[in]  symbol       OFDM symbol index.
  void generate_sequence(span<srsran::cf_t>                sequence,
                         const config_t&                   pucch_config,
                         const sequence_generation_config& gen_config,
                         unsigned                          symbol) const;

  /// \brief Generates the PUCCH DM-RS allocation pattern.
  ///
  /// Implements the PUCCH DM-RS mapping, as described in TS38.211 Section 6.4.1.3.1.2.
  /// \param[in] config Configuration parameters.
  /// \return The DM-RS allocation pattern.
  static layer_dmrs_pattern generate_dmrs_pattern(const config_t& config);
};
} // namespace srsran
