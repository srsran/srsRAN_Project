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

#include "srs_estimator_generic_impl.h"
#include "srs_validator_generic_impl.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/expected.h"
#include "srsran/adt/static_vector.h"
#include "srsran/adt/tensor.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_configuration.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_result.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/srs/srs_constants.h"
#include "srsran/ran/srs/srs_information.h"
#include "srsran/srsvec/add.h"
#include "srsran/srsvec/dot_prod.h"
#include "srsran/srsvec/mean.h"
#include "srsran/srsvec/prod.h"
#include "srsran/srsvec/sc_prod.h"
#include "srsran/srsvec/subtract.h"

using namespace srsran;

/// \brief Looks at the output of the validator and, if unsuccessful, fills \c msg with the error message.
///
/// This is used to call the validator inside the process methods only if asserts are active.
[[maybe_unused]] static bool handle_validation(std::string& msg, const error_type<std::string>& err)
{
  bool is_success = err.has_value();
  if (!is_success) {
    msg = err.error();
  }
  return is_success;
}

void srs_estimator_generic_impl::compensate_phase_shift(span<cf_t> mean_lse,
                                                        float      phase_shift_subcarrier,
                                                        float      phase_shift_offset)
{
  unsigned sequence_length = mean_lse.size();

  // Generate phase indices.
  span<unsigned> phase_indices = span<unsigned>(temp_phase).first(sequence_length);
  std::generate(
      phase_indices.begin(), phase_indices.end(), [phase_shift_subcarrier, phase_shift_offset, n = 0]() mutable {
        return static_cast<int>(std::round(static_cast<float>(cexp_table_size) *
                                           (static_cast<float>(n++) * phase_shift_subcarrier + phase_shift_offset) /
                                           TWOPI));
      });

  // Generate complex exponential.
  span<cf_t> cexp = span<cf_t>(temp_cexp).first(sequence_length);
  cexp_table.generate(cexp, phase_indices);

  // Compensate phase shift.
  srsvec::prod(mean_lse, cexp, mean_lse);
}

srs_estimator_result srs_estimator_generic_impl::estimate(const resource_grid_reader&        grid,
                                                          const srs_estimator_configuration& config)
{
  // Makes sure the PDU is valid.
  [[maybe_unused]] std::string msg;
  srsran_assert(handle_validation(msg, srs_validator_generic_impl(max_nof_prb).is_valid(config)), "{}", msg);

  unsigned nof_rx_ports         = config.ports.size();
  auto     nof_antenna_ports    = static_cast<unsigned>(config.resource.nof_antenna_ports);
  auto     nof_symbols          = static_cast<unsigned>(config.resource.nof_symbols);
  unsigned nof_symbols_per_slot = get_nsymb_per_slot(cyclic_prefix::NORMAL);
  srsran_assert(config.resource.start_symbol.value() + nof_symbols <= nof_symbols_per_slot,
                "The start symbol index (i.e., {}) plus the number of symbols (i.e., {}) exceeds the number of symbols "
                "per slot (i.e., {})",
                config.resource.start_symbol,
                nof_symbols,
                nof_symbols_per_slot);

  // Extract subcarrier spacing.
  subcarrier_spacing scs = to_subcarrier_spacing(config.slot.numerology());

  // Extract comb size.
  auto comb_size = static_cast<unsigned>(config.resource.comb_size);

  srs_information common_info = get_srs_information(config.resource, 0);

  // Sequence length is common for all ports and symbols.
  unsigned sequence_length = common_info.sequence_length;

  // Maximum measurable delay due to cyclic shift.
  double max_ta = 1.0 / static_cast<double>(common_info.n_cs_max * scs_to_khz(scs) * 1000 * comb_size);

  // Prepare results.
  srs_estimator_result result;
  result.time_alignment.time_alignment = 0;
  result.time_alignment.resolution     = 0;
  result.time_alignment.min            = std::numeric_limits<double>::min();
  result.time_alignment.max            = std::numeric_limits<double>::max();
  result.channel_matrix                = srs_channel_matrix(nof_rx_ports, nof_antenna_ports);

  // Temporary LSE.
  static_tensor<3, cf_t, max_seq_length * srs_constants::max_nof_rx_ports * srs_constants::max_nof_tx_ports> temp_lse(
      {sequence_length, nof_rx_ports, nof_antenna_ports});

  // All sequences of pilots.
  static_tensor<2, cf_t, max_seq_length * srs_constants::max_nof_tx_ports> all_sequences(
      {sequence_length, nof_antenna_ports});

  // Auxiliary buffer for noise computation.
  static_tensor<3, cf_t, 2 * max_seq_length * srs_constants::max_nof_rx_ports> temp_noise(
      {sequence_length, 2, nof_rx_ports});
  srsvec::zero(temp_noise.get_data());

  srs_information info_port0         = get_srs_information(config.resource, /*antenna_port*/ 0);
  bool            interleaved_pilots = (nof_antenna_ports == 4) && (info_port0.n_cs >= info_port0.n_cs_max / 2);

  float epre = 0;
  // Iterate transmit ports.
  for (unsigned i_antenna_port = 0; i_antenna_port != nof_antenna_ports; ++i_antenna_port) {
    // Obtain SRS information for a given SRS antenna port.
    srs_information info = get_srs_information(config.resource, i_antenna_port);

    // Generate sequence and store them in all_sequences.
    span<cf_t> sequence = all_sequences.get_view({i_antenna_port});
    deps.sequence_generator->generate(sequence, info.sequence_group, info.sequence_number, info.n_cs, info.n_cs_max);

    // For the current Tx antenna, keep track of all the LSEs at all Rx ports.
    modular_re_buffer_reader<cf_t, srs_constants::max_nof_rx_ports> port_lse(nof_rx_ports, sequence_length);

    // Iterate receive ports.
    for (unsigned i_rx_port_index = 0; i_rx_port_index != nof_rx_ports; ++i_rx_port_index) {
      unsigned i_rx_port = config.ports[i_rx_port_index];

      // View to the mean LSE for a port combination.
      span<cf_t> mean_lse = temp_lse.get_view({i_rx_port_index, i_antenna_port});
      // View for noise computation: with interleaved pilots, we need to keep track of two different sets of REs - those
      // for odd-indexed ports and those for even-indexed ports.
      span<cf_t> noise_help = temp_noise.get_view({(interleaved_pilots) ? i_antenna_port % 2 : 0U, i_rx_port_index});

      // Extract sequence for all symbols and average LSE.
      for (unsigned i_symbol     = config.resource.start_symbol.value(),
                    i_symbol_end = config.resource.start_symbol.value() + nof_symbols;
           i_symbol != i_symbol_end;
           ++i_symbol) {
        // Extract received sequence.
        static_vector<cf_t, max_seq_length> rx_sequence(info.sequence_length);
        grid.get(rx_sequence, i_rx_port, i_symbol, info.mapping_initial_subcarrier, info.comb_size);

        // Since the same SRS sequence is sent over all symbols, it makes sense to average out the noise. When pilots
        // are interleaved, we need to keep track of two different sets of REs.
        if ((i_antenna_port == 0) || (interleaved_pilots && (i_antenna_port == 1))) {
          srsvec::add(noise_help, rx_sequence, noise_help);
          epre += srsvec::average_power(rx_sequence);
        }

        // Avoid accumulation for the first symbol containing SRS.
        if (i_symbol == config.resource.start_symbol.value()) {
          srsvec::copy(mean_lse, rx_sequence);
        } else {
          srsvec::add(mean_lse, rx_sequence, mean_lse);
        }
      }

      // Calculate LSE.
      srsvec::prod_conj(mean_lse, mean_lse, sequence);

      // Scale accumulated LSE.
      if (nof_symbols > 1) {
        srsvec::sc_prod(mean_lse, mean_lse, 1.0 / static_cast<float>(nof_symbols));
      }

      port_lse.set_slice(i_rx_port_index, mean_lse);
    }

    // Estimate TA. Note that, since port_lse still contains the contributions of the other Tx ports (which cancel out
    // only when averaging across subcarriers), the channel impulse response of the channel will show a number of
    // replicas. However, since the TA estimator picks the peak closest to the origin (i.e., the one corresponding to
    // the first replica), the estimation is still valid.
    time_alignment_measurement ta_meas = deps.ta_estimator->estimate(port_lse, info.comb_size, scs, max_ta);

    // Combine time alignment measurements.
    result.time_alignment.time_alignment += ta_meas.time_alignment;
    result.time_alignment.min        = std::max(result.time_alignment.min, ta_meas.min);
    result.time_alignment.max        = std::min(result.time_alignment.max, ta_meas.max);
    result.time_alignment.resolution = std::max(result.time_alignment.resolution, ta_meas.resolution);
  }

  // Average time alignment across all paths.
  result.time_alignment.time_alignment /= nof_antenna_ports;

  float noise_var = 0;
  float rsrp      = 0;
  // Compensate time alignment and estimate channel coefficients.
  for (unsigned i_rx_port = 0; i_rx_port != nof_rx_ports; ++i_rx_port) {
    for (unsigned i_antenna_port = 0; i_antenna_port != nof_antenna_ports; ++i_antenna_port) {
      // View to the mean LSE for a port combination.
      span<cf_t> mean_lse = temp_lse.get_view({i_rx_port, i_antenna_port});

      // Get sequence information.
      srs_information info = get_srs_information(config.resource, i_antenna_port);

      // Calculate subcarrier phase shift in radians.
      auto phase_shift_subcarrier =
          static_cast<float>(TWOPI * result.time_alignment.time_alignment * scs_to_khz(scs) * 1000 * comb_size);

      // Calculate the initial phase shift in radians.
      float phase_shift_offset =
          phase_shift_subcarrier * static_cast<float>(info.mapping_initial_subcarrier) / static_cast<float>(comb_size);

      // Compensate phase shift.
      compensate_phase_shift(mean_lse, phase_shift_subcarrier, phase_shift_offset);

      // Calculate channel wideband coefficient.
      cf_t coefficient = srsvec::mean(mean_lse);
      result.channel_matrix.set_coefficient(coefficient, i_rx_port, i_antenna_port);
      rsrp += std::norm(coefficient);

      // View for noise computation: with interleaved pilots, we need to keep track of two different sets of REs -
      // those for odd-indexed ports and those for even-indexed ports.
      span<cf_t> noise_help = temp_noise.get_view({(interleaved_pilots) ? i_antenna_port % 2 : 0U, i_rx_port});

      if ((i_antenna_port == 0) || (interleaved_pilots && (i_antenna_port == 1))) {
        compensate_phase_shift(noise_help, phase_shift_subcarrier, phase_shift_offset);
      }

      // We recover the signal by multiplying the SRS sequence by the channel coefficient and we remove it from
      // noise_help. Recall that the latter contains the contribution of all symbols, so the reconstructed symbol must
      // also be counted nof_symbols times.
      static_vector<cf_t, max_seq_length> recovered_signal(noise_help.size());
      srsvec::sc_prod(
          recovered_signal, all_sequences.get_view({i_antenna_port}), static_cast<float>(nof_symbols) * coefficient);
      srsvec::subtract(noise_help, noise_help, recovered_signal);
    }
    span<cf_t> noise_help = temp_noise.get_view({0U, i_rx_port});
    noise_var += srsvec::average_power(noise_help) * noise_help.size();

    if (interleaved_pilots) {
      noise_help = temp_noise.get_view({1U, i_rx_port});
      noise_var += srsvec::average_power(noise_help) * noise_help.size();
    }
  }
  // At this point, noise_var contains the sum of all the squared errors between the received signal and the
  // reconstructed one. For each Rx port, the number of degrees of freedom used to estimate the channel coefficients
  // is usually equal nof_antenna_ports, but when pilots are interleaved, in which case it's 2. Also, when
  // interleaving pilots, we look at double the samples.
  unsigned nof_estimates     = (interleaved_pilots ? 2 : nof_antenna_ports);
  unsigned correction_factor = (interleaved_pilots ? 2 : 1);
  noise_var /= static_cast<float>((nof_symbols * sequence_length - nof_estimates) * correction_factor * nof_rx_ports);

  // Normalize the wideband channel matrix with respect to the noise standard deviation, so that the Frobenius norm
  // square will give us a rough estimate of the SNR. Avoid huge coefficients if the noise variance is too low
  // (keep SNR <= 40 dB).
  float noise_std = std::max(std::sqrt(noise_var), std::sqrt(rsrp) * 0.01F);
  result.channel_matrix *= 1 / noise_std;

  epre /= static_cast<float>(nof_symbols * correction_factor * nof_rx_ports);
  rsrp /= static_cast<float>(nof_antenna_ports * nof_rx_ports);

  // Set noise variance, EPRE and RSRP.
  result.noise_variance = noise_var;
  result.epre_dB        = convert_power_to_dB(epre);
  result.rsrp_dB        = convert_power_to_dB(rsrp);

  return result;
}
