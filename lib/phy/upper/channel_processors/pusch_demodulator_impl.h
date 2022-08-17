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
  pusch_demodulator_impl(std::unique_ptr<channel_equalizer>       eq,
                         std::unique_ptr<demodulation_mapper>     demap,
                         std::unique_ptr<pseudo_random_generator> dscr) :
    equalizer(std::move(eq)), demapper(std::move(demap)), descrambler(std::move(dscr))
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
  /// \brief Gets channel symbols from the resource grid.
  ///
  /// \param[out] symbols PUSCH channel symbols, organized by receive antenna port.
  /// \param[in]  grid    Resource grid for the current slot.
  /// \param[in]  config  Configuration parameters.
  void get_ch_symbols(re_measurement<cf_t>& symbols, const resource_grid_reader& grid, const configuration& config)
  {
    // For now, do nothing.
  }

  /// \brief Removes elements corresponding to DM-RS symbols and reserved positions.
  ///
  /// From the equalizer output, removes all estimated modulation symbols and noise variances corresponding to DM-RS
  /// positions. Also, performs layer demapping, thus returning two one-dimensional sequences.
  ///
  /// \param[out] symbols_out     Layer-demapped sequence of estimated modulation symbols.
  /// \param[out] noise_vars_out  Layer-demapped sequence of equivalent symbol noise variances.
  /// \param[in]  symbols_in      Equalized modulation and DM-RS symbols, grouped by layer.
  /// \param[in]  noise_vars_in   Equivalent symbol noise variances, grouped by layer.
  /// \param[in]  config          Configuration parameters.
  void remove_dmrs(static_vector<cf_t, MAX_NOF_DATA_LLR>&  symbols_out,
                   static_vector<float, MAX_NOF_DATA_LLR>& noise_vars_out,
                   const re_measurement<cf_t>&             symbols_in,
                   const re_measurement<float>&            noise_vars_in,
                   const configuration&                    config)
  {
    // For now, do nothing.
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
  re_measurement<cf_t> ch_symbols;
  /// Buffer used to store channel modulation symbols at the equalizer output.
  re_measurement<cf_t> mod_symbols_eq;
  /// Buffer used to transfer symbol noise variances at the equalizer output.
  re_measurement<float> noise_vars_eq;
  /// Buffer used to transfer channel modulation symbols to the demodulator.
  static_vector<cf_t, MAX_NOF_DATA_LLR> mod_symbols_data;
  /// Buffer used to transfer symbol noise variances to the demodulator.
  static_vector<float, MAX_NOF_DATA_LLR> noise_vars_data;
};

} // namespace srsgnb
