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
/// \brief PUSCH demodulator implementation declaration.

#pragma once

#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/upper/channel_modulation/demodulation_mapper.h"
#include "srsran/phy/upper/channel_modulation/evm_calculator.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_demodulator.h"
#include "srsran/phy/upper/equalization/channel_equalizer.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"

namespace srsran {

/// PUSCH demodulator implementation.
// todo(david): Evaluate whether it is worth delegating the private methods to specialized subcomponent classes.
class pusch_demodulator_impl : public pusch_demodulator
{
public:
  /// Constructor: sets up internal components and acquires their ownership.
  pusch_demodulator_impl(std::unique_ptr<channel_equalizer>       equalizer_,
                         std::unique_ptr<demodulation_mapper>     demapper_,
                         std::unique_ptr<evm_calculator>          evm_calc_,
                         std::unique_ptr<pseudo_random_generator> descrambler_,
                         bool                                     compute_post_eq_sinr_) :
    equalizer(std::move(equalizer_)),
    demapper(std::move(demapper_)),
    evm_calc(std::move(evm_calc_)),
    descrambler(std::move(descrambler_)),
    ch_re({MAX_BLOCK_SIZE, 1}),
    eq_re({MAX_BLOCK_SIZE, 1}),
    eq_noise_vars({MAX_BLOCK_SIZE, 1}),
    ch_estimates({MAX_BLOCK_SIZE, 1, 1}),
    compute_post_eq_sinr(compute_post_eq_sinr_)
  {
    srsran_assert(equalizer, "Invalid pointer to channel_equalizer object.");
    srsran_assert(demapper, "Invalid pointer to demodulation_mapper object.");
    srsran_assert(descrambler, "Invalid pointer to pseudo_random_generator object.");
  }

  // See interface for the documentation.
  void demodulate(pusch_codeword_buffer&      data,
                  pusch_demodulator_notifier& notifier,
                  const resource_grid_reader& grid,
                  const channel_estimate&     estimates,
                  const configuration&        config) override;

private:
  /// Maximum processing block size in bits.
  static constexpr unsigned MAX_BLOCK_SIZE = 4096;
  /// Data type for representing an RE mask within an OFDM symbol.
  using re_symbol_mask_type = bounded_bitset<MAX_RB * NRE>;

  /// \brief Gets channel data Resource Elements from the resource grid.
  ///
  /// Extracts the PUSCH data RE's from the provided resource grid. The DM-RS symbols are skipped. The extracted RE's
  /// are arranged in two dimensions, i.e., resource element and receive antenna port, as the channel equalizer expects.
  ///
  /// \param[out] data_re   PUSCH channel data symbols, organized by receive antenna port.
  /// \param[in]  grid      Resource grid for the current slot.
  /// \param[in]  i_symbol  OFDM symbol index relative to the beginning of the slot.
  /// \param[in]  init_subc Initial subcarrier index relative to Point A.
  /// \param[in]  re_mask   Resource element mask, it selects the RE elements to extract.
  /// \param[in]  rx_ports  Receive ports.
  static void get_ch_data_re(channel_equalizer::re_list&              data_re,
                             const resource_grid_reader&              grid,
                             unsigned                                 i_symbol,
                             unsigned                                 init_subc,
                             const re_symbol_mask_type&               re_mask,
                             const static_vector<uint8_t, MAX_PORTS>& rx_ports)
  {
    // Extract RE for each port and symbol.
    for (unsigned i_port = 0, i_port_end = rx_ports.size(); i_port != i_port_end; ++i_port) {
      // Get a view of the port data RE.
      span<cf_t> re_port_buffer = data_re.get_view<>({i_port});

      // Copy grid data resource elements into the buffer.
      re_port_buffer = grid.get(re_port_buffer, rx_ports[i_port], i_symbol, init_subc, re_mask);

      // Verify buffer size.
      srsran_assert(
          re_port_buffer.empty(), "Invalid number of RE read from the grid. {} RE are missing.", re_port_buffer.size());
    }
  }

  /// \brief Gets channel data estimates.
  ///
  /// Extracts the channel estimation coefficients corresponding to the PUSCH data Resource Elements from the channel
  /// estimate. The DM-RS symbols are skipped. The extracted channel coefficients are arranged in three dimensions,
  /// i.e., resource element, receive port and transmit layer, as the channel equalizer expects.
  ///
  /// \param[out] data_estimates Channel estimates of the data symbols, organized by receive port and transmit layer.
  /// \param[in]  channel_estimate Channel estimation object.
  /// \param[in]  i_symbol  OFDM symbol index relative to the beginning of the slot.
  /// \param[in]  init_subc Initial subcarrier index relative to Point A.
  /// \param[in]  re_mask   Resource element mask, it selects the RE elements to extract.
  /// \param[in]  rx_ports  Receive ports.
  static void get_ch_data_estimates(channel_equalizer::ch_est_list&          data_estimates,
                                    const channel_estimate&                  channel_estimate,
                                    unsigned                                 i_symbol,
                                    unsigned                                 init_subc,
                                    const re_symbol_mask_type&               re_mask,
                                    const static_vector<uint8_t, MAX_PORTS>& rx_ports)
  {
    // Extract data RE coefficients from the channel estimation.
    for (unsigned i_layer = 0, i_layer_end = channel_estimate.size().nof_tx_layers; i_layer != i_layer_end; ++i_layer) {
      for (unsigned i_port = 0, i_port_end = rx_ports.size(); i_port != i_port_end; ++i_port) {
        // Get a view of the channel estimates buffer for a single Rx port.
        span<cf_t> ch_port_buffer = data_estimates.get_view({i_port, i_layer});

        // View of the channel estimation for an OFDM symbol.
        span<const cf_t> symbol_estimates = channel_estimate.get_symbol_ch_estimate(i_symbol, i_port, i_layer);

        // Get view of the selected area of the grid.
        symbol_estimates = symbol_estimates.subspan(init_subc, re_mask.size());

        if (re_mask.is_contiguous()) {
          int begin = re_mask.find_lowest();
          int end   = re_mask.find_highest();

          srsran_assert(begin >= 0, "Invalid mask.");
          srsran_assert(begin <= end, "Invalid mask.");

          symbol_estimates = symbol_estimates.subspan(begin, end - begin + 1);

          srsvec::copy(ch_port_buffer, symbol_estimates);
          continue;
        }

        // Skip DM-RS estimates.
        re_mask.for_each(0, re_mask.size(), [&symbol_estimates, &ch_port_buffer](unsigned i_re) {
          // Copy RE.
          ch_port_buffer.front() = symbol_estimates[i_re];

          // Advance buffer.
          ch_port_buffer = ch_port_buffer.last(ch_port_buffer.size() - 1);
        });

        // Verify buffer size.
        srsran_assert(ch_port_buffer.empty(),
                      "Invalid number of RE read from the channel estimates. {} RE are missing.",
                      ch_port_buffer.size());
      }
    }
  }

  /// Channel equalization component, also in charge of combining contributions of all receive antenna ports.
  std::unique_ptr<channel_equalizer> equalizer;
  /// Demodulation mapper component: transforms channel symbols into log-likelihood ratios (i.e., soft bits).
  std::unique_ptr<demodulation_mapper> demapper;
  /// EVM calculator. Optional, set to nullptr if not available.
  std::unique_ptr<evm_calculator> evm_calc;
  /// Descrambler component.
  std::unique_ptr<pseudo_random_generator> descrambler;

  /// Buffer used to transfer channel modulation symbols from the resource grid to the equalizer.
  dynamic_tensor<std::underlying_type_t<channel_equalizer::re_list::dims>(channel_equalizer::re_list::dims::nof_dims),
                 cf_t,
                 channel_equalizer::re_list::dims>
      ch_re;

  /// Buffer used to store channel modulation resource elements at the equalizer output.
  dynamic_tensor<std::underlying_type_t<channel_equalizer::re_list::dims>(channel_equalizer::re_list::dims::nof_dims),
                 cf_t,
                 channel_equalizer::re_list::dims>
      eq_re;

  /// Buffer used to transfer symbol noise variances at the equalizer output.
  dynamic_tensor<std::underlying_type_t<channel_equalizer::re_list::dims>(channel_equalizer::re_list::dims::nof_dims),
                 float,
                 channel_equalizer::re_list::dims>
      eq_noise_vars;

  /// Buffer used to transfer channel estimation coefficients from the channel estimate to the equalizer.
  dynamic_tensor<std::underlying_type_t<channel_equalizer::ch_est_list::dims>(
                     channel_equalizer::ch_est_list::dims::nof_dims),
                 cf_t,
                 channel_equalizer::ch_est_list::dims>
      ch_estimates;

  /// Buffer used to transfer noise variance estimates from the channel estimate to the equalizer.
  std::array<float, MAX_PORTS> noise_var_estimates;

  /// Enables post equalization SINR calculation.
  bool compute_post_eq_sinr;
};

} // namespace srsran
