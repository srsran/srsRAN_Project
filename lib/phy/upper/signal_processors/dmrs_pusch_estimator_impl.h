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

/// \file
/// \brief PUSCH channel estimator definition.

#pragma once

#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/phy/upper/signal_processors/dmrs_pusch_estimator.h"
#include "srsran/phy/upper/signal_processors/port_channel_estimator.h"

namespace srsran {

class dmrs_pusch_estimator_impl : public dmrs_pusch_estimator
{
public:
  using layer_dmrs_pattern = port_channel_estimator::layer_dmrs_pattern;

  /// Constructor - sets the channel estimator.
  explicit dmrs_pusch_estimator_impl(std::unique_ptr<pseudo_random_generator> prg_,
                                     std::unique_ptr<port_channel_estimator>  ch_est) :
    prg(std::move(prg_)), ch_estimator(std::move(ch_est))
  {
    srsran_assert(prg, "Invalid PRG.");
    srsran_assert(ch_estimator, "Invalid port channel estimator.");
  }

  // See interface for the documentation.
  void estimate(channel_estimate& estimate, const resource_grid_reader& grid, const configuration& config) override;

private:
  /// Parameters for PUSCH DM-RS.
  struct parameters {
    bounded_bitset<NRE>  re_pattern;
    std::array<float, 2> w_f;
    std::array<float, 2> w_t;
  };

  /// Parameters for PUSCH DM-RS configuration type 1 as per TS 38.211 Table 6.4.1.1.3-1.
  static const std::array<parameters, 8> params_type1;

  /// Parameters for PUSCH DM-RS configuration type 2 as per TS 38.211 Table 6.4.1.1.3-2.
  static const std::array<parameters, 12> params_type2;

  /// Maximum supported number of transmission layers.
  static constexpr unsigned MAX_TX_LAYERS = pusch_constants::MAX_NOF_LAYERS;
  /// DMRS for PUSCH reference point \f$k\f$ relative to Point A.
  static constexpr unsigned DMRS_REF_POINT_K_TO_POINT_A = 0;

  /// Pseudo-random generator.
  std::unique_ptr<pseudo_random_generator> prg;
  /// Antenna port channel estimator.
  std::unique_ptr<port_channel_estimator> ch_estimator;
  /// Buffer for DM-RS symbols.
  dmrs_symbol_list temp_symbols;
  /// Buffer for DM-RS symbol coordinates.
  std::array<layer_dmrs_pattern, MAX_TX_LAYERS> temp_pattern;

  /// \brief Generates the sequence described in TS 38.211 section 6.4.1.1.1, considering the only values required
  /// in TS38.211 section 6.4.1.1.2.
  ///
  /// \param[out] sequence Sequence destination.
  /// \param[in] symbol    Symbol index within the slot.
  /// \param[in] config    Configuration parameters.
  void sequence_generation(span<cf_t> sequence, unsigned symbol, const configuration& config) const;

  /// \brief Generates the PUSCH DM-RS symbols for one transmission port.
  ///
  /// Implements the PUSCH DM-RS generation, precoding and mapping procedures described in TS38.211 Section 6.4.1.1.
  /// \param[out] symbols Lists of generated DM-RS symbols, one per transmission layer.
  /// \param[out] pattern Lists of DM-RS patterns representing the REs the DM-RS symbols should be mapped to, one per
  ///                     transmission layer.
  /// \param[in]  cfg     Configuration parameters.
  void generate(dmrs_symbol_list& symbols, span<layer_dmrs_pattern> mask, const configuration& cfg);
};

} // namespace srsran
