/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief PUSCH demodulator implementation declaration.

#pragma once

#include "srsgnb/phy/upper/channel_modulation/demodulation_mapper.h"
#include "srsgnb/phy/upper/channel_processors/pusch_demodulator.h"
#include "srsgnb/phy/upper/equalization/channel_equalizer.h"
#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"

namespace srsgnb {

/// PUSCH demodulator implementation.
// todo(david): Evaluate whether it is worth delegating the private methods to specialized subcomponent classes.
class pusch_demodulator_impl : public pusch_demodulator
{
public:
  /// Constructor: sets up internal components and acquires their ownership.
  pusch_demodulator_impl(std::unique_ptr<channel_equalizer>       equalizer_,
                         std::unique_ptr<demodulation_mapper>     demapper_,
                         std::unique_ptr<pseudo_random_generator> descrambler_) :
    equalizer(std::move(equalizer_)),
    demapper(std::move(demapper_)),
    descrambler(std::move(descrambler_)),
    ch_re({MAX_RB * NRE * MAX_NSYMB_PER_SLOT, MAX_PORTS}),
    eq_re({MAX_RB * NRE * MAX_NSYMB_PER_SLOT, pusch_constants::MAX_NOF_LAYERS}),
    eq_noise_vars({MAX_RB * NRE * MAX_NSYMB_PER_SLOT, pusch_constants::MAX_NOF_LAYERS})
  {
    srsgnb_assert(equalizer, "Invalid pointer to channel_equalizer object.");
    srsgnb_assert(demapper, "Invalid pointer to demodulation_mapper object.");
    srsgnb_assert(descrambler, "Invalid pointer to pseudo_random_generator object.");
  }

  // See interface for the documentation.
  void demodulate(span<log_likelihood_ratio>  data,
                  const resource_grid_reader& grid,
                  const channel_estimate&     estimates,
                  const configuration&        config) override;

private:
  /// \brief Gets channel data Resource Elements from the resource grid.
  ///
  /// Extracts the PUSCH data RE's from the provided resource grid. The DM-RS symbols are skipped. The extracted RE's
  /// are arranged in two dimensions, i.e., resource element and receive antenna port, as the channel equalizer expects.
  ///
  /// \param[out] data_re PUSCH channel data symbols, organized by receive antenna port.
  /// \param[in]  grid    Resource grid for the current slot.
  /// \param[in]  config  Configuration parameters.
  static void
  get_ch_data_re(channel_equalizer::re_list& data_re, const resource_grid_reader& grid, const configuration& config)
  {
    srsgnb_assert(config.dmrs_config_type == dmrs_type::TYPE1 && config.nof_cdm_groups_without_data == 2,
                  "Only DM-RS type 1 with a number of CDM groups equal to 2 is implemented.");

    // Prepare RE mask.
    bounded_bitset<NRE> prb_re_mask(NRE);
    prb_re_mask.fill(0, NRE, true);
    bounded_bitset<MAX_RB* NRE> re_mask = config.rb_mask.kronecker_product<NRE>(prb_re_mask);

    // Number of subcarriers.
    unsigned nof_subcs = config.rb_mask.count() * NRE;

    // Extract RE for each port and symbol.
    for (unsigned i_port = 0, i_port_end = config.rx_ports.size(); i_port != i_port_end; ++i_port) {
      // Get a view of the port data RE.
      span<cf_t> re_port_buffer = data_re.get_view<>({i_port});

      for (unsigned i_symbol = 0; i_symbol != config.nof_symbols; ++i_symbol) {
        // Skip data carrying DM-RS.
        if (config.dmrs_symb_pos[i_symbol + config.start_symbol_index]) {
          continue;
        }

        // Copy grid data resource elements into the buffer.
        span<cf_t> unread_re =
            grid.get(re_port_buffer.first(nof_subcs), i_port, i_symbol + config.start_symbol_index, 0, re_mask);

        srsgnb_assert(
            unread_re.empty(), "Invalid number of RE read from the grid. {} RE have not been read", unread_re.size());

        // Advance buffers.
        re_port_buffer = re_port_buffer.subspan(nof_subcs, re_port_buffer.size() - nof_subcs);
      }
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
  /// \param[in]  config  Configuration parameters.
  static void get_ch_data_estimates(channel_equalizer::ch_est_list& data_estimates,
                                    const channel_estimate&         channel_estimate,
                                    const configuration&            config)
  {
    srsgnb_assert(config.dmrs_config_type == dmrs_type::TYPE1 && config.nof_cdm_groups_without_data == 2,
                  "Only DM-RS type 1 with a number of CDM groups equal to 2 is implemented.");

    // Number of subcarriers.
    unsigned nof_subcs = config.rb_mask.size() * NRE;

    // Extract data RE coefficients from the channel estimation.
    for (unsigned i_layer = 0, i_layer_end = config.nof_tx_layers; i_layer != i_layer_end; ++i_layer) {
      for (unsigned i_port = 0, i_port_end = config.rx_ports.size(); i_port != i_port_end; ++i_port) {
        // Get a view of the channel estimates buffer for a single Rx port.
        span<cf_t> ch_port_buffer = data_estimates.get_view<>({i_port, i_layer});

        for (unsigned i_symbol     = config.start_symbol_index,
                      i_symbol_end = config.start_symbol_index + config.nof_symbols;
             i_symbol != i_symbol_end;
             ++i_symbol) {
          // Skip DM-RS estimates.
          if (config.dmrs_symb_pos[i_symbol]) {
            continue;
          }

          // View of the channel estimation for an OFDM symbol.
          span<const cf_t> symbol_estimates = channel_estimate.get_symbol_ch_estimate(i_symbol, i_port, i_layer);

          srsgnb_assert(symbol_estimates.size() == nof_subcs,
                        "Channel estimate symbol size (i.e. {}) does not match expected symbol size (i.e. {})",
                        symbol_estimates.size(),
                        nof_subcs);

          // Copy channel data estimates into the buffer.
          config.rb_mask.for_each(0, config.rb_mask.size(), [&symbol_estimates, &ch_port_buffer](unsigned i_prb) {
            // View over the estimates corresponding to a Resource Block.
            span<const cf_t> rb_estimates = symbol_estimates.subspan(i_prb * NRE, NRE);
            // Copy the estimates into the buffer.
            srsvec::copy(ch_port_buffer.first(NRE), rb_estimates);
            // Advance buffer.
            ch_port_buffer = ch_port_buffer.last(ch_port_buffer.size() - NRE);
          });
        }

        // Assert that the expected number of channel coefficients are read.
        srsgnb_assert(ch_port_buffer.size() == 0, "Incorrect number of extracted channel estimation coefficients.");
      }
    }
  }

  /// \brief Reverses the scrambling of the received soft bits described by TS38.211 Section 6.3.1.1.
  /// \param[out] out   Result of the process of reversing the scrambling.
  /// \param[in] in     Input soft bits to descramble.
  /// \param[in] config PUSCH demodulator configuration.
  void descramble(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in, const configuration& config);

  /// Channel equalization component, also in charge of combining contributions of all receive antenna ports.
  std::unique_ptr<channel_equalizer> equalizer;
  /// Demodulation mapper component: transforms channel symbols into log-likelihood ratios (i.e., soft bits).
  std::unique_ptr<demodulation_mapper> demapper;
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
  std::array<float, pusch_constants::MAX_NOF_LAYERS> noise_var_estimates;
};

} // namespace srsgnb
