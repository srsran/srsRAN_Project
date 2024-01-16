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
/// \brief PUCCH detector declaration.

#pragma once

#include "srsran/phy/upper/channel_processors/pucch_detector.h"
#include "srsran/phy/upper/equalization/channel_equalizer.h"
#include "srsran/phy/upper/sequence_generators/low_papr_sequence_collection.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"

namespace srsran {

/// PUCCH detector implementation for Formats 0 and 1.
class pucch_detector_impl : public pucch_detector
{
public:
  /// \brief Maximum number of REs allocated to PUCCH Format 1.
  ///
  /// The allocated resources are at most one PRB over all OFDM symbols.
  static constexpr unsigned MAX_ALLOCATED_RE_F1 = NRE * MAX_NSYMB_PER_SLOT;

  /// Maximum number of OFDM symbols allocated for PUCCH Format 1 data.
  static constexpr unsigned MAX_N_DATA_SYMBOLS = 7;

  /// \brief Constructor: provides access to a collection of low-PAPR sequences and a pseudorandom sequence generator.
  /// \param[in] lpsc Pointer to a collection of low-PAPR sequences.
  /// \param[in] prg  Pointer to a pseudorandom sequence generator.
  /// \remark The low-PAPR collection should correspond to the cyclic shifts \f$\{\alpha : \alpha = 2 \pi
  /// \frac{n}{N_{\textup{sc}}^{\textup{RB}}}, \quad n = 0, \dots, N_{\textup{sc}}^{\textup{RB}}-1\}\f$, where
  /// \f$N_{\textup{sc}}^{\textup{RB}} = 12\f$ is the number of subcarriers in a resource block.
  pucch_detector_impl(std::unique_ptr<low_papr_sequence_collection> lpsc,
                      std::unique_ptr<pseudo_random_generator>      prg,
                      std::unique_ptr<channel_equalizer>            eqzr) :
    low_papr(std::move(lpsc)), pseudo_random(std::move(prg)), equalizer(std::move(eqzr))
  {
  }

  // See interface for documentation.
  std::pair<pucch_uci_message, channel_state_information> detect(const resource_grid_reader& /*grid*/,
                                                                 const format0_configuration& /*config*/) override
  {
    srsran_assert(false, "PUCCH Format 0 not implemented yet.");
    return {};
  }

  // See interface for documentation.
  pucch_detection_result detect(const resource_grid_reader&  grid,
                                const channel_estimate&      estimates,
                                const format1_configuration& config) override;

private:
  /// \brief Extracts PUCCH data and channel coefficients.
  ///
  /// This method extracts the PUCCH data REs from the resource grid as well as the corresponding channel estimates.
  void extract_data_and_estimates(const resource_grid_reader& grid,
                                  const channel_estimate&     estimates,
                                  unsigned                    first_symbol,
                                  unsigned                    first_prb,
                                  optional<unsigned>          second_prb,
                                  unsigned                    port);

  /// \brief Marginalizes the spreading sequences out.
  ///
  /// A PUCCH Format 1 consists of a single modulation symbol spread over all time and frequency allocated resources.
  /// This method combines all the replicas into a single estimate of the modulation symbol and it computes the
  /// equivalent noise variance. The PUCCH configuration is needed to build the proper spreading sequences.
  void marginalize_w_and_r_out(const format1_configuration& config);

  /// Collection of low-PAPR sequences.
  std::unique_ptr<low_papr_sequence_collection> low_papr;
  /// Pseudorandom sequence generator.
  std::unique_ptr<pseudo_random_generator> pseudo_random;
  /// Channel equalizer.
  std::unique_ptr<channel_equalizer> equalizer;
  /// \brief Tensor for storing the spread data sequence.
  /// \remark Only half of the allocated symbols contain data, the other half being used for DM-RS.
  static_tensor<std::underlying_type_t<channel_equalizer::re_list::dims>(channel_equalizer::re_list::dims::nof_dims),
                cf_t,
                MAX_ALLOCATED_RE_F1 / 2,
                channel_equalizer::re_list::dims>
      time_spread_sequence;
  /// \brief Tensor for storing the channel estimates corresponding to the spread data sequence.
  /// \remark Only half of the allocated symbols contain data, the other half being used for DM-RS.
  static_tensor<std::underlying_type_t<channel_equalizer::ch_est_list::dims>(
                    channel_equalizer::ch_est_list::dims::nof_dims),
                cf_t,
                MAX_ALLOCATED_RE_F1 / 2,
                channel_equalizer::ch_est_list::dims>
      ch_estimates;
  /// \brief Buffer for storing the spread data sequence after equalization.
  /// \remark Only half of the allocated symbols contain data, the other half being used for DM-RS.
  static_tensor<std::underlying_type_t<channel_equalizer::re_list::dims>(channel_equalizer::re_list::dims::nof_dims),
                cf_t,
                MAX_ALLOCATED_RE_F1 / 2,
                channel_equalizer::re_list::dims>
      eq_time_spread_sequence;
  /// \brief Buffer for storing the equivalent channel noise variances corresponding to the equalized spread data
  /// sequence.
  /// \remark Only half of the allocated symbols contain data, the other half being used for DM-RS.
  static_tensor<std::underlying_type_t<channel_equalizer::re_list::dims>(channel_equalizer::re_list::dims::nof_dims),
                float,
                MAX_ALLOCATED_RE_F1 / 2,
                channel_equalizer::re_list::dims>
      eq_time_spread_noise_var;
  /// Buffer for storing alpha indices.
  std::array<unsigned, MAX_NSYMB_PER_SLOT / 2> alpha_buffer;
  /// View of the alpha indices buffer.
  span<unsigned> alpha_indices;
  /// Detected symbol.
  cf_t detected_symbol = 0;
  /// Equivalent noise variance of detected symbol.
  float eq_noise_var = 0;
  /// Total number of data symbols.
  unsigned nof_data_symbols = 0;
  /// Number of data symbols before frequency hopping.
  unsigned nof_data_symbols_pre_hop = 0;
};

} // namespace srsran
