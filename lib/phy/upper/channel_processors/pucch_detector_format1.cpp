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
/// \brief PUCCH detector definition for Format 1.

#include "pucch_detector_format1.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/upper/channel_processors/pucch_detector.h"
#include "srsran/phy/upper/pucch_orthogonal_sequence.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/mean.h"

using namespace srsran;

// Pre-generated orthogonal cover code.
static const pucch_orthogonal_sequence occ;

static void validate_config(const pucch_detector::format1_configuration& config)
{
  srsran_assert(config.start_symbol_index <= 10,
                "Setting {} as the first PUCCH symbol index, but only values between 0 and 10 are valid.",
                config.start_symbol_index);
  srsran_assert((config.nof_symbols >= 4) && (config.nof_symbols <= 14),
                "Requiring {} OFDM symbols for PUCCH, but only values between 4 and 14 are valid.",
                config.nof_symbols);
  srsran_assert(
      config.nof_symbols + config.start_symbol_index <= 14,
      "The sum of the starting OFDM symbol (i.e., {}) and the number of allocated symbols (i.e., {}) cannot exceed 14.",
      config.start_symbol_index,
      config.nof_symbols);
  srsran_assert(config.starting_prb <= 274,
                "Setting {} as the PRB allocated to PUCCH, but only values between 0 and 274 are valid.",
                config.starting_prb);
  if (config.second_hop_prb.has_value()) {
    srsran_assert(config.second_hop_prb.value() <= 274,
                  "Setting {} as the PRB allocated to PUCCH after frequency hopping, but only values between 0 and 274 "
                  "are valid.",
                  config.second_hop_prb.value());
  }
  srsran_assert(config.time_domain_occ <= 6,
                "Setting {} as the time-domain OCC index, but only values between 0 and 6 are valid.",
                config.time_domain_occ);
  srsran_assert(config.n_id <= 1023,
                "Initializing the pseudorandom generator with {}, but only values between 0 and 1023 are valid.",
                config.n_id);
  srsran_assert(config.nof_harq_ack <= 2, "At most two ACK bits - requested {}.", config.nof_harq_ack);
}

// Given the detected symbol and the corresponding equivalent noise variance, demodulates the symbol into nof_bits bits.
// It also returns the detection metric used to decide whether the PUCCH was transmitted or not by threshold comparison.
static float detect_bits(span<uint8_t> out_bits, cf_t detected_symbol, float eq_noise_var)
{
  unsigned nof_bits          = out_bits.size();
  float    detection_metric  = std::real(detected_symbol) + std::imag(detected_symbol);
  unsigned bits              = (detection_metric > 0) ? 0U : 3U;
  detection_metric           = std::abs(detection_metric);
  float    detection_metric2 = std::real(detected_symbol) - std::imag(detected_symbol);
  unsigned bits2             = (detection_metric2 > 0) ? 2U : 1U;
  detection_metric2          = std::abs(detection_metric2);
  if ((nof_bits > 1) && (detection_metric2 > detection_metric)) {
    out_bits[0] = (bits2 & 1U);
    out_bits[1] = ((bits2 >> 1U) & 1U);
    return (std::norm(detected_symbol) / eq_noise_var);
  }
  out_bits[0] = (bits & 1U);
  if (nof_bits > 1) {
    out_bits[1] = ((bits >> 1U) & 1U);
  }
  return (std::norm(detected_symbol) / eq_noise_var);
}

pucch_detector::pucch_detection_result
pucch_detector_format1::detect(const resource_grid_reader&                  grid,
                               const channel_estimate&                      estimates,
                               const pucch_detector::format1_configuration& config)
{
  validate_config(config);

  // Total number of REs used for PUCCH data (recall that positive integer division implies taking the floor).
  unsigned nof_res   = (config.nof_symbols / 2) * NRE;
  unsigned nof_ports = config.ports.size();
  time_spread_sequence.resize(nof_ports, nof_res);
  ch_estimates.resize(nof_res, nof_ports, 1);
  eq_time_spread_sequence.resize(nof_res);
  eq_time_spread_noise_var.resize(nof_res);

  // Compute the number of data symbols before frequency hop.
  nof_data_symbols         = config.nof_symbols / 2;
  nof_data_symbols_pre_hop = nof_data_symbols;
  if (config.second_hop_prb.has_value()) {
    nof_data_symbols_pre_hop = config.nof_symbols / 4;
  }

  alpha_indices = span<unsigned>(alpha_buffer).first(nof_data_symbols);

  extract_data_and_estimates(
      grid, estimates, config.start_symbol_index, config.starting_prb, config.second_hop_prb, config.ports);

  span<float> noise_var_all_ports = span<float>(noise_var_buffer).first(nof_ports);
  for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
    noise_var_all_ports[i_port] = estimates.get_noise_variance(config.ports[i_port]);
  }
  equalizer->equalize(eq_time_spread_sequence,
                      eq_time_spread_noise_var,
                      time_spread_sequence,
                      ch_estimates,
                      noise_var_all_ports,
                      config.beta_pucch);

  marginalize_w_and_r_out(config);

  // Prepare UCI message output.
  // We don't set the SR bit here - this task is delegated to a higher-level function, based on the uci_status returned
  // by this detector and on the used PUCCH resource.
  // This format doesn't support CSI reports.
  pucch_uci_message::configuration pucch_uci_message_config;
  pucch_uci_message_config.nof_sr               = 0;
  pucch_uci_message_config.nof_harq_ack         = config.nof_harq_ack;
  pucch_uci_message_config.nof_csi_part1        = 0;
  pucch_uci_message_config.nof_csi_part2        = 0;
  pucch_detector::pucch_detection_result output = {pucch_uci_message(pucch_uci_message_config), 0};

  // Select view of the payload.
  span<uint8_t> bits = output.uci_message.get_harq_ack_bits();

  // Recall that, when nof_harq_ack == 0, we still need to look for the positive SR indicator (i.e., a single, 0-valued
  // transmitted bit).
  static_vector<uint8_t, 1> temp_bits(1);
  if (config.nof_harq_ack == 0) {
    bits = temp_bits;
  }

  float detection_metric = detect_bits(bits, detected_symbol, eq_noise_var);

  // Detection threshold.
  //
  // The detection metric, as computed by \c detect_bits, is assumed to be normally distributed with variance 1 and mean
  // either 0 (when there is no PUCCH) or larger than 0 (when there is a PUCCH). Therefore, one can target a constant
  // probability of false alarm of 1% by setting the detection threshold T such that Q(T) = 0.01, where the Q-function
  // is the tail distribution function of the standard normal distribution.
  constexpr float THRESHOLD = 4.0F;
  bool            is_msg_ok = (detection_metric > THRESHOLD);
  output.detection_metric   = detection_metric / THRESHOLD;

  if (!is_msg_ok) {
    output.uci_message.set_status(uci_status::invalid);
    return output;
  }

  if (config.nof_harq_ack > 0) {
    output.uci_message.set_status(uci_status::valid);
    return output;
  }

  // If we are here, there should only be a positive SR bit and it should be 0, since nothing is sent for negative
  // SR and no ACK.
  if (bits[0] == 0U) {
    output.uci_message.set_status(uci_status::valid);
    return output;
  }

  output.uci_message.set_status(uci_status::unknown);
  return output;
}

void pucch_detector_format1::extract_data_and_estimates(const resource_grid_reader&              grid,
                                                        const channel_estimate&                  estimates,
                                                        unsigned                                 first_symbol,
                                                        unsigned                                 first_prb,
                                                        std::optional<unsigned>                  second_prb,
                                                        const static_vector<uint8_t, MAX_PORTS>& antenna_ports)
{
  for (uint8_t port : antenna_ports) {
    unsigned      i_symbol       = 0;
    unsigned      skip           = 0;
    unsigned      symbol_index   = first_symbol + 1;
    span<cbf16_t> sequence_slice = time_spread_sequence.get_slice(port);
    span<cbf16_t> estimate_slice = ch_estimates.get_channel(port, 0);
    for (; i_symbol != nof_data_symbols_pre_hop; ++i_symbol, skip += NRE, symbol_index += 2) {
      // Index of the first subcarrier assigned to PUCCH, before hopping.
      unsigned      k_init         = NRE * first_prb;
      span<cbf16_t> sequence_chunk = sequence_slice.subspan(skip, NRE);
      grid.get(sequence_chunk, port, symbol_index, k_init);

      span<const cbf16_t> tmp = estimates.get_symbol_ch_estimate(symbol_index, port);
      srsvec::copy(estimate_slice.subspan(skip, NRE), tmp.subspan(k_init, NRE));
    }

    for (; i_symbol != nof_data_symbols; ++i_symbol, skip += NRE, symbol_index += 2) {
      // Index of the first subcarrier assigned to PUCCH, after hopping. Note that we only enter this loop if
      // second_prb.has_value().
      unsigned      k_init         = NRE * second_prb.value();
      span<cbf16_t> sequence_chunk = sequence_slice.subspan(skip, NRE);
      grid.get(sequence_chunk, port, symbol_index, k_init);

      span<const cbf16_t> tmp_in  = estimates.get_symbol_ch_estimate(symbol_index, port).subspan(k_init, NRE);
      span<cbf16_t>       tmp_out = estimate_slice.subspan(skip, NRE);
      srsvec::copy(tmp_out, tmp_in);
    }
  }
}

// Computest the indices of the cyclic shifts for all symbols.
static void compute_alpha_indices(span<unsigned>       indices,
                                  unsigned             start_symbol,
                                  unsigned             nof_symbols,
                                  const slot_point&    slot,
                                  const cyclic_prefix& cp,
                                  unsigned             n_id,
                                  unsigned             m0,
                                  pucch_helper&        helper)
{
  srsran_assert(indices.size() == nof_symbols / 2,
                "The number of alpha indices {} does not match with the number of allocated symbols {}.",
                indices.size(),
                nof_symbols);

  // Only every other symbol, starting from the second one, contains data.
  for (unsigned i_symbol = 1, i_alpha = 0; i_symbol < nof_symbols; i_symbol += 2, ++i_alpha) {
    // Compute alpha index.
    indices[i_alpha] = helper.get_alpha_index(slot, cp, n_id, start_symbol + i_symbol, m0, 0);
  }
}

void pucch_detector_format1::marginalize_w_and_r_out(const pucch_detector::format1_configuration& config)
{
  unsigned time_domain_occ = config.time_domain_occ;

  srsran_assert(config.group_hopping == pucch_group_hopping::NEITHER,
                "At the moment, only group the hopping type 'neither' is implemented, requesting {}",
                ((config.group_hopping == pucch_group_hopping::ENABLE) ? "enable" : "disable"));
  unsigned group_index     = config.n_id % 30;
  unsigned sequence_number = 0;

  span<const cf_t> w_star = occ.get_sequence_conj(nof_data_symbols_pre_hop, time_domain_occ);

  compute_alpha_indices(alpha_indices,
                        config.start_symbol_index,
                        config.nof_symbols,
                        config.slot,
                        config.cp,
                        config.n_id,
                        config.initial_cyclic_shift,
                        helper);

  detected_symbol = 0;
  unsigned offset = 0;
  for (unsigned i_symbol = 0; i_symbol != nof_data_symbols_pre_hop; ++i_symbol, offset += NRE) {
    span<const cf_t> seq_r = low_papr->get(group_index, sequence_number, alpha_indices[i_symbol]);
    for (unsigned i_elem = 0; i_elem != NRE; ++i_elem) {
      detected_symbol += eq_time_spread_sequence[offset + i_elem] * w_star[i_symbol] * std::conj(seq_r[i_elem]);
    }
  }

  unsigned nof_data_symbols_post_hop = nof_data_symbols - nof_data_symbols_pre_hop;
  if (nof_data_symbols_post_hop > 0) {
    w_star = occ.get_sequence_conj(nof_data_symbols_post_hop, time_domain_occ);

    for (unsigned i_symbol = 0; i_symbol != nof_data_symbols_post_hop; ++i_symbol, offset += NRE) {
      span<const cf_t> seq_r =
          low_papr->get(group_index, sequence_number, alpha_indices[i_symbol + nof_data_symbols_pre_hop]);
      for (unsigned i_elem = 0; i_elem != NRE; ++i_elem) {
        detected_symbol += eq_time_spread_sequence[offset + i_elem] * w_star[i_symbol] * std::conj(seq_r[i_elem]);
      }
    }
  }

  auto n_repetitions = static_cast<float>(eq_time_spread_sequence.size());
  detected_symbol /= n_repetitions;
  // For the noise variance, we have to compute the sum of all variances and divide by the square of their number: same
  // as computing the mean and dividing again buy their number.
  eq_noise_var = srsvec::mean(eq_time_spread_noise_var) / n_repetitions;
}
