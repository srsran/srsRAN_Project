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
                  span<log_likelihood_ratio>  harq_ack,
                  span<log_likelihood_ratio>  csi_part1,
                  span<log_likelihood_ratio>  csi_part2,
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
    std::array<bool, MAX_RB* NRE> tmp_re_mask = {};
    span<bool>                    re_mask     = span<bool>(tmp_re_mask).first(config.rb_mask.size() * NRE);
    config.rb_mask.for_each(0, config.rb_mask.size(), [&](unsigned i_prb) {
      span<bool> re_view = re_mask.subspan(i_prb * NRE, NRE);
      std::fill(re_view.begin(), re_view.end(), true);
    });

    // Number of subcarriers.
    unsigned nof_subcs = config.rb_mask.size() * NRE;

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

  /// \brief Gets channel data estimates from the channel estimation.
  ///
  /// Extracts the channel estimation coefficients corresponding to the PUSCH data Resource Elements from the channel
  /// estimate. The DM-RS symbols are skipped. The extracted channel coefficients are arranged in two dimensions, i.e.,
  /// resource element and receive antenna port, as the channel equalizer expects.
  ///
  /// \param[out] data_estimates PUSCH channel estimates of the data symbols, organized by receive antenna port.
  /// \param[in]  channel_estimate channel estimation object.
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
        // Get a view of the port data estimates.
        span<cf_t> ch_port_buffer = data_estimates.get_view<>({i_port, i_layer});

        // Skip DM-RS estimates.
        for (unsigned i_symbol = 0; i_symbol != config.nof_symbols; ++i_symbol) {
          if (config.dmrs_symb_pos[i_symbol + config.start_symbol_index]) {
            continue;
          }

          // Copy channel data estimates into the buffer.
          span<const cf_t> symbol_estimates = channel_estimate.get_symbol_ch_estimate(i_symbol, i_port, i_layer);

          srsgnb_assert(symbol_estimates.size() == nof_subcs,
                        "Channel estimate symbol size (i.e. {}) does not match expected symbol size (i.e. {})",
                        symbol_estimates.size(),
                        nof_subcs);

          srsvec::copy(ch_port_buffer.first(nof_subcs), symbol_estimates);

          // Advance buffers.
          ch_port_buffer = ch_port_buffer.subspan(nof_subcs, ch_port_buffer.size() - nof_subcs);
        }
      }
    }
  }

  /// \brief Extracts HARQ-ACK soft bits from the sequence of multiplexed data and control bits.
  ///
  /// See TS38.212 Section 6.2.7.
  /// \param[out]    harq_ack Sequence of HARQ-ACK soft bits.
  /// \param[in/out] data     Sequence of data soft bits (possibly multiplexed with control ones).
  /// \param[in]     config   Configuration parameters.
  /// \remark The elements of \c data corresponding to HARQ-ACK bits will be set to zero (i.e., fully indeterminate data
  /// soft bit).
  void
  extract_harq_ack(span<log_likelihood_ratio> harq_ack, span<log_likelihood_ratio> data, const configuration& config)
  {
    // For now, do nothing.
  }

  /// \brief Extracts CSI Part1 soft bits from the sequence of multiplexed data and control bits.
  ///
  /// See TS38.212 Section 6.2.7.
  /// \param[out]    csi_part1 Sequence of CSI Part1 soft bits.
  /// \param[in/out] data      Sequence of data soft bits (possibly multiplexed with control ones).
  /// \param[in]     config    Configuration parameters.
  /// \remark The elements of \c data corresponding to CSI Part1 bits will be set to zero (i.e., fully indeterminate
  /// data soft bit).
  void extract_csi_part1(span<log_likelihood_ratio>& csi_part1,
                         span<log_likelihood_ratio>& data,
                         const configuration&        config)
  {
    // For now, do nothing.
  }

  /// \brief Extracts CSI Part2 soft bits from the sequence of multiplexed data and control bits.
  ///
  /// See TS38.212 Section 6.2.7.
  /// \param[out]    csi_part1 Sequence of CSI Part2 soft bits.
  /// \param[in/out] data      Sequence of data soft bits (possibly multiplexed with control ones).
  /// \param[in]     config    Configuration parameters.
  /// \remark The elements of \c data corresponding to CSI Part2 bits will be set to zero (i.e., fully indeterminate
  /// data soft bit).
  void
  extract_csi_part2(span<log_likelihood_ratio> csi_part2, span<log_likelihood_ratio> data, const configuration& config)
  {
    // For now, do nothing.
  }

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
