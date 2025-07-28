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

#pragma once

#include "srsran/phy/upper/pucch_helper.h"
#include "srsran/phy/upper/sequence_generators/low_papr_sequence_generator.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/phy/upper/signal_processors/dmrs_pucch_estimator.h"
#include "srsran/phy/upper/signal_processors/port_channel_estimator.h"

namespace srsran {

/// Generic implementation of a DM-RS channel estimator for PUCCH Formats 3 and 4.
class dmrs_pucch_estimator_formats3_4
{
public:
  using layer_dmrs_pattern = port_channel_estimator::layer_dmrs_pattern;

  dmrs_pucch_estimator_formats3_4(std::unique_ptr<pseudo_random_generator>     prg_,
                                  std::unique_ptr<low_papr_sequence_generator> low_papr_generator_,
                                  std::unique_ptr<port_channel_estimator>      ch_est_) :
    helper(std::move(prg_)), low_papr_generator(std::move(low_papr_generator_)), ch_estimator(std::move(ch_est_))
  {
    srsran_assert(low_papr_generator, "Invalid sequence generator.");
    srsran_assert(ch_estimator, "Invalid port channel estimator.");
  }

  /// Estimates the channel of a PUCCH Format 3 transmission. See \ref dmrs_pucch_estimator for more details.
  void estimate(channel_estimate&                                  ch_estimate,
                const resource_grid_reader&                        grid,
                const dmrs_pucch_estimator::format3_configuration& config);

  /// Estimates the channel of a PUCCH Format 4 transmission. See \ref dmrs_pucch_estimator for more details.
  void estimate(channel_estimate&                                  ch_estimate,
                const resource_grid_reader&                        grid,
                const dmrs_pucch_estimator::format4_configuration& config);

private:
  /// PUCCH helper functions.
  pucch_helper helper;

  /// PUCCH Formats 3 and 4 implementation expects pre-generated sequence collection on instantiation.
  std::unique_ptr<low_papr_sequence_generator> low_papr_generator;
  /// Antenna port channel estimator.
  std::unique_ptr<port_channel_estimator> ch_estimator;

  /// Buffer for DM-RS symbols.
  dmrs_symbol_list temp_symbols;

  /// Common configuration parameters for PUCCH Formats 3 and 4.
  struct estimate_config {
    /// Slot timing and numerology.
    slot_point slot;
    /// Cyclic Prefix.
    cyclic_prefix cp;
    /// Group and sequence hopping configuration.
    pucch_group_hopping group_hopping;
    /// Parameter \f$n_{ID}\f$ in TS38.211 Section 6.3.2.2.2.
    unsigned n_id;
    /// Number of PRBs.
    unsigned nof_prb;
    /// Number of OFDM symbols assigned to resource.
    unsigned nof_symbols;
    /// Start PRB index.
    unsigned starting_prb;
    /// Index of first PRB of the second hop if intra-slot frequency hopping is enabled, empty otherwise.
    std::optional<unsigned> second_hop_prb;
    /// Start symbol index.
    unsigned start_symbol_index;
    /// Additional DM-RS flag.
    bool additional_dmrs;
    /// Orthogonal Cover Code.
    unsigned occ_index;
    /// Port indices the PUCCH transmission is mapped onto.
    const static_vector<uint8_t, DMRS_MAX_NPORTS>& ports;
  };

  /// Performs the actual channel estimation of a PUCCH Format 3 or Format 4 transmission.
  void estimate(channel_estimate& estimate, const resource_grid_reader& grid, const estimate_config& config);

  /// \brief Generates a DM-RS sequence according to TS38.211 Section 6.4.1.3.2.1.
  ///
  /// \param[out] sequence Sequence destination.
  /// \param[in]  config   Required parameters to calculate the sequences.
  /// \param[in]  u        Sequence group.
  /// \param[in]  v        Sequence number.
  /// \param[in]  symbol   Symbol index.
  void generate_sequence(span<cf_t> sequence, const estimate_config& config, unsigned u, unsigned v, unsigned symbol);

  /// \brief Generates the PUCCH DM-RS allocation pattern.
  ///
  /// Implements the PUCCH DM-RS mapping, as described in TS38.211 Section 6.4.1.3.3.2.
  /// \param[in] config Number of PRBs.
  ///
  /// \return The DM-RS allocation pattern.
  static layer_dmrs_pattern generate_dmrs_pattern(const estimate_config& config);
};

} // namespace srsran
