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

#include "srsran/phy/generic_functions/transform_precoding/transform_precoder.h"
#include "srsran/phy/support/re_buffer.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/upper/channel_modulation/demodulation_mapper.h"
#include "srsran/phy/upper/channel_modulation/evm_calculator.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_demodulator.h"
#include "srsran/phy/upper/equalization/channel_equalizer.h"
#include "srsran/phy/upper/equalization/dynamic_ch_est_list.h"
#include "srsran/phy/upper/equalization/modular_ch_est_list.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/srsvec/copy.h"

namespace srsran {

/// PUSCH demodulator implementation.
// todo(david): Evaluate whether it is worth delegating the private methods to specialized subcomponent classes.
class pusch_demodulator_impl : public pusch_demodulator
{
public:
  /// Constructor: sets up internal components and acquires their ownership.
  pusch_demodulator_impl(std::unique_ptr<channel_equalizer>       equalizer_,
                         std::unique_ptr<transform_precoder>      precoder_,
                         std::unique_ptr<demodulation_mapper>     demapper_,
                         std::unique_ptr<evm_calculator>          evm_calc_,
                         std::unique_ptr<pseudo_random_generator> descrambler_,
                         unsigned                                 max_nof_rb,
                         bool                                     compute_post_eq_sinr_) :
    equalizer(std::move(equalizer_)),
    precoder(std::move(precoder_)),
    demapper(std::move(demapper_)),
    evm_calc(std::move(evm_calc_)),
    descrambler(std::move(descrambler_)),
    ch_re_copy(MAX_PORTS, max_nof_rb * NRE),
    temp_eq_re(max_nof_rb * NRE * pusch_constants::MAX_NOF_LAYERS),
    temp_eq_noise_vars(max_nof_rb * NRE * pusch_constants::MAX_NOF_LAYERS),
    ch_estimates_copy(max_nof_rb * NRE, pusch_constants::MAX_NOF_RX_PORTS, pusch_constants::MAX_NOF_LAYERS),
    ch_estimates_view(pusch_constants::MAX_NOF_RX_PORTS, pusch_constants::MAX_NOF_LAYERS),
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
  /// Data type for representing an RE mask within an OFDM symbol.
  using re_symbol_mask_type = bounded_bitset<MAX_RB * NRE>;

  /// \brief Gets channel data Resource Elements from the resource grid.
  ///
  /// Extracts the PUSCH data RE's from the provided resource grid. The DM-RS symbols are skipped. The extracted RE's
  /// are arranged in two dimensions, i.e., resource element and receive antenna port, as the channel equalizer expects.
  ///
  /// \param[in]  grid      Resource grid for the current slot.
  /// \param[in]  i_symbol  OFDM symbol index relative to the beginning of the slot.
  /// \param[in]  re_mask   Resource element mask, it selects the RE elements to extract.
  /// \param[in]  rx_ports  Receive ports.
  /// \return A reference to the PUSCH channel data symbols.
  const re_buffer_reader<cbf16_t>& get_ch_data_re(const resource_grid_reader&              grid,
                                                  unsigned                                 i_symbol,
                                                  const re_symbol_mask_type&               re_mask,
                                                  const static_vector<uint8_t, MAX_PORTS>& rx_ports)
  {
    // Extract RE boundaries.
    unsigned nof_re = re_mask.count();
    int      begin  = re_mask.find_lowest();
    int      end    = re_mask.find_highest();
    srsran_assert(begin <= end, "Invalid mask.");

    // Check if the mask is contiguous.
    if (nof_re == static_cast<unsigned>(end + 1 - begin)) {
      // Prepare channel estimates view.
      ch_re_view.resize(rx_ports.size(), nof_re);

      // Iterate over all layers and ports.
      for (unsigned i_port = 0, i_port_end = rx_ports.size(); i_port != i_port_end; ++i_port) {
        // View of the channel estimation for an OFDM symbol.
        span<const cbf16_t> ch_data_re = grid.get_view(i_port, i_symbol);

        // Set the view in the channel estimates.
        ch_re_view.set_slice(i_port, ch_data_re.subspan(begin, nof_re));
      }
      return ch_re_view;
    }

    // Prepare channel estimates copy destination.
    ch_re_copy.resize(rx_ports.size(), nof_re);

    // Extract RE for each port and symbol.
    for (unsigned i_port = 0, i_port_end = rx_ports.size(); i_port != i_port_end; ++i_port) {
      // Get a view of the port data RE.
      span<cbf16_t> re_port_buffer = ch_re_copy.get_slice(i_port);

      // Copy grid data resource elements into the buffer.
      re_port_buffer = grid.get(re_port_buffer, rx_ports[i_port], i_symbol, 0, re_mask);

      // Verify buffer size.
      srsran_assert(
          re_port_buffer.empty(), "Invalid number of RE read from the grid. {} RE are missing.", re_port_buffer.size());
    }

    return ch_re_copy;
  }

  /// \brief Gets channel data estimates.
  ///
  /// Extracts the channel estimation coefficients corresponding to the PUSCH data Resource Elements from the channel
  /// estimate. The DM-RS symbols are skipped. The extracted channel coefficients are arranged in three dimensions,
  /// i.e., resource element, receive port and transmit layer, as the channel equalizer expects.
  ///
  /// \param[in]  channel_estimate Channel estimation object.
  /// \param[in]  i_symbol      OFDM symbol index relative to the beginning of the slot.
  /// \param[in]  init_subc     Initial subcarrier index relative to Point A.
  /// \param[in]  nof_tx_layers Number of layers.
  /// \param[in]  re_mask       Resource element mask, it selects the RE elements to extract.
  /// \param[in]  rx_ports      Receive ports list.
  /// \return A reference to the PUSCH channel data estimates.
  const channel_equalizer::ch_est_list& get_ch_data_estimates(const channel_estimate&                  channel_estimate,
                                                              unsigned                                 i_symbol,
                                                              unsigned                                 nof_tx_layers,
                                                              const re_symbol_mask_type&               re_mask,
                                                              const static_vector<uint8_t, MAX_PORTS>& rx_ports)
  {
    // Extract RE boundaries.
    unsigned nof_re = re_mask.count();
    int      begin  = re_mask.find_lowest();
    int      end    = re_mask.find_highest();
    srsran_assert(begin <= end, "Invalid mask.");

    // Check if the mask is contiguous.
    if (nof_re == static_cast<unsigned>(end + 1 - begin)) {
      // Prepare channel estimates view.
      ch_estimates_view.resize(nof_re, rx_ports.size(), nof_tx_layers);

      // Iterate over all layers and ports.
      for (unsigned i_layer = 0, i_layer_end = nof_tx_layers; i_layer != i_layer_end; ++i_layer) {
        for (unsigned i_port = 0, i_port_end = rx_ports.size(); i_port != i_port_end; ++i_port) {
          // View of the channel estimation for an OFDM symbol.
          span<const cbf16_t> symbol_estimates = channel_estimate.get_symbol_ch_estimate(i_symbol, i_port, i_layer);

          // Set the view in the channel estimates.
          ch_estimates_view.set_channel(symbol_estimates.subspan(begin, nof_re), i_port, i_layer);
        }
      }

      // Return the reference to the view buffer.
      return ch_estimates_view;
    }

    ch_estimates_copy.resize(nof_re, rx_ports.size(), nof_tx_layers);

    // Extract data RE coefficients from the channel estimation.
    for (unsigned i_layer = 0, i_layer_end = nof_tx_layers; i_layer != i_layer_end; ++i_layer) {
      for (unsigned i_port = 0, i_port_end = rx_ports.size(); i_port != i_port_end; ++i_port) {
        // Get a view of the channel estimates buffer for a single Rx port.
        span<cbf16_t> ch_port_buffer = ch_estimates_copy.get_channel(i_port, i_layer);

        // View of the channel estimation for an OFDM symbol.
        span<const cbf16_t> symbol_estimates = channel_estimate.get_symbol_ch_estimate(i_symbol, i_port, i_layer);

        // Get view of the selected area of the grid.
        symbol_estimates = symbol_estimates.first(re_mask.size());

        // Skip DM-RS estimates.
        re_mask.for_each(0, re_mask.size(), [&symbol_estimates, &ch_port_buffer](unsigned i_re) {
          // Copy RE.
          ch_port_buffer.front() = to_cf(symbol_estimates[i_re]);

          // Advance buffer.
          ch_port_buffer = ch_port_buffer.last(ch_port_buffer.size() - 1);
        });

        // Verify buffer size.
        srsran_assert(ch_port_buffer.empty(),
                      "Invalid number of RE read from the channel estimates. {} RE are missing.",
                      ch_port_buffer.size());
      }
    }

    return ch_estimates_copy;
  }

  /// Channel equalization component, also in charge of combining contributions of all receive antenna ports.
  std::unique_ptr<channel_equalizer> equalizer;
  /// Transform precoder.
  std::unique_ptr<transform_precoder> precoder;
  /// Demodulation mapper component: transforms channel symbols into log-likelihood ratios (i.e., soft bits).
  std::unique_ptr<demodulation_mapper> demapper;
  /// EVM calculator. Optional, set to nullptr if not available.
  std::unique_ptr<evm_calculator> evm_calc;
  /// Descrambler component.
  std::unique_ptr<pseudo_random_generator> descrambler;
  /// Copy buffer used to transfer channel modulation symbols from the resource grid to the equalizer.
  dynamic_re_buffer<cbf16_t> ch_re_copy;
  /// View buffer used to transfer channel modulation symbols from the resource grid to the equalizer.
  modular_re_buffer_reader<cbf16_t, MAX_PORTS> ch_re_view;
  /// Buffer used to store channel modulation resource elements at the equalizer output.
  std::vector<cf_t> temp_eq_re;
  /// Buffer used to transfer symbol noise variances at the equalizer output.
  std::vector<float> temp_eq_noise_vars;
  /// Copy buffer used to transfer channel estimation coefficients from the channel estimate to the equalizer.
  dynamic_ch_est_list ch_estimates_copy;
  /// View buffer used to transfer channel estimation coefficients from the channel estimate to the equalizer.
  modular_ch_est_list<pusch_constants::MAX_NOF_RX_PORTS * pusch_constants::MAX_NOF_LAYERS> ch_estimates_view;
  /// Buffer used to transfer noise variance estimates from the channel estimate to the equalizer.
  std::array<float, MAX_PORTS> noise_var_estimates;

  /// Enables post equalization SINR calculation.
  bool compute_post_eq_sinr;
};

} // namespace srsran
