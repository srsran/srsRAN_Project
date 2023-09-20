/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "pucch_processor_impl.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/ran/pucch/pucch_info.h"

using namespace srsran;

pucch_processor_result pucch_processor_impl::process(const resource_grid_reader&  grid,
                                                     const format1_configuration& config)
{
  // Check that the PUCCH Format 1 configuration is valid.
  assert_format1_config(config);

  // Prepare channel estimation.
  dmrs_pucch_processor::config_t estimator_config;
  estimator_config.format               = pucch_format::FORMAT_1;
  estimator_config.slot                 = config.slot;
  estimator_config.cp                   = config.cp;
  estimator_config.start_symbol_index   = config.start_symbol_index;
  estimator_config.nof_symbols          = config.nof_symbols;
  estimator_config.starting_prb         = config.starting_prb + config.bwp_start_rb;
  estimator_config.intra_slot_hopping   = config.second_hop_prb.has_value();
  estimator_config.second_hop_prb       = config.second_hop_prb.has_value()
                                              ? (config.second_hop_prb.value() + config.bwp_start_rb)
                                              : estimator_config.starting_prb;
  estimator_config.initial_cyclic_shift = config.initial_cyclic_shift;
  estimator_config.time_domain_occ      = config.time_domain_occ;
  estimator_config.n_id                 = config.n_id;
  estimator_config.ports.assign(config.ports.begin(), config.ports.end());

  // Unused channel estimator parameters for this format.
  estimator_config.group_hopping   = pucch_group_hopping::NEITHER;
  estimator_config.nof_prb         = 0;
  estimator_config.n_id_0          = 0;
  estimator_config.additional_dmrs = false;

  // Prepare channel estimate.
  channel_estimate::channel_estimate_dimensions dims;
  dims.nof_prb       = config.bwp_start_rb + config.bwp_size_rb;
  dims.nof_symbols   = get_nsymb_per_slot(config.cp);
  dims.nof_rx_ports  = config.ports.size();
  dims.nof_tx_layers = pucch_constants::MAX_LAYERS;

  estimates.resize(dims);

  // Perform channel estimation.
  channel_estimator_format_1->estimate(estimates, grid, estimator_config);

  // Prepare detector configuration.
  pucch_detector::format1_configuration detector_config;
  detector_config.slot         = config.slot;
  detector_config.cp           = config.cp;
  detector_config.starting_prb = config.starting_prb + config.bwp_start_rb;
  if (config.second_hop_prb.has_value()) {
    detector_config.second_hop_prb.emplace(config.second_hop_prb.value() + config.bwp_start_rb);
  }
  detector_config.start_symbol_index   = config.start_symbol_index;
  detector_config.nof_symbols          = config.nof_symbols;
  detector_config.group_hopping        = pucch_group_hopping::NEITHER;
  detector_config.port                 = config.ports.front();
  detector_config.beta_pucch           = 1.0F;
  detector_config.time_domain_occ      = config.time_domain_occ;
  detector_config.initial_cyclic_shift = config.initial_cyclic_shift;
  detector_config.n_id                 = config.n_id;
  detector_config.nof_harq_ack         = config.nof_harq_ack;

  // Actual message detection.
  pucch_detector::pucch_detection_result detection_result = detector->detect(grid, estimates, detector_config);

  // Prepare result.
  pucch_processor_result result;
  estimates.get_channel_state_information(result.csi);
  result.message          = detection_result.uci_message;
  result.detection_metric = detection_result.detection_metric;

  return result;
}

pucch_processor_result pucch_processor_impl::process(const resource_grid_reader&  grid,
                                                     const format2_configuration& config)
{
  // Check that the PUCCH Format 2 configuration is valid.
  assert_format2_config(config);

  pucch_processor_result result;

  // PUCCH UCI message configuration.
  pucch_uci_message::configuration pucch_uci_message_config = {};
  pucch_uci_message_config.nof_sr                           = config.nof_sr;
  pucch_uci_message_config.nof_harq_ack                     = config.nof_harq_ack;
  pucch_uci_message_config.nof_csi_part1                    = config.nof_csi_part1;
  pucch_uci_message_config.nof_csi_part2                    = config.nof_csi_part2;

  result.message = pucch_uci_message(pucch_uci_message_config);

  // Channel estimator configuration.
  dmrs_pucch_processor::config_t estimator_config = {};

  estimator_config.format             = pucch_format::FORMAT_2;
  estimator_config.slot               = config.slot;
  estimator_config.cp                 = config.cp;
  estimator_config.start_symbol_index = config.start_symbol_index;
  estimator_config.nof_symbols        = config.nof_symbols;
  estimator_config.starting_prb       = config.bwp_start_rb + config.starting_prb;
  estimator_config.nof_prb            = config.nof_prb;
  estimator_config.n_id               = config.n_id;
  estimator_config.n_id_0             = config.n_id_0;
  estimator_config.ports.assign(config.ports.begin(), config.ports.end());

  // Prepare channel estimate.
  channel_estimate::channel_estimate_dimensions dims;
  dims.nof_prb       = config.bwp_start_rb + config.bwp_size_rb;
  dims.nof_symbols   = get_nsymb_per_slot(config.cp);
  dims.nof_rx_ports  = config.ports.size();
  dims.nof_tx_layers = pucch_constants::MAX_LAYERS;

  estimates.resize(dims);

  // Perform channel estimation.
  channel_estimator_format_2->estimate(estimates, grid, estimator_config);

  estimates.get_channel_state_information(result.csi);

  span<log_likelihood_ratio> llr =
      span<log_likelihood_ratio>(temp_llr).first(pucch_constants::FORMAT2_NOF_DATA_SC * config.nof_prb *
                                                 config.nof_symbols * get_bits_per_symbol(modulation_scheme::QPSK));

  // PUCCH Format 2 demodulator configuration.
  pucch_demodulator::format2_configuration demod_config = {};

  demod_config.rx_ports           = config.ports;
  demod_config.first_prb          = config.bwp_start_rb + config.starting_prb;
  demod_config.nof_prb            = config.nof_prb;
  demod_config.start_symbol_index = config.start_symbol_index;
  demod_config.nof_symbols        = config.nof_symbols;
  demod_config.rnti               = config.rnti;
  demod_config.n_id               = config.n_id;

  // Perform demodulation.
  demodulator->demodulate(llr, grid, estimates, demod_config);

  // UCI decoder configuration.
  uci_decoder::configuration decoder_config = {};
  decoder_config.modulation                 = modulation_scheme::QPSK;

  // Decode UCI payload.
  result.message.set_status(decoder->decode(result.message.get_full_payload(), llr, decoder_config));

  // Expected UCI payload length in number of bits.
  unsigned expected_nof_uci_bits = config.nof_harq_ack + config.nof_sr + config.nof_csi_part1 + config.nof_csi_part2;

  // Assert that the decoded UCI payload has the expected number of bits.
  srsran_assert(result.message.get_full_payload().size() == expected_nof_uci_bits,
                "Decoded UCI payload length, i.e., {}, does not match expected number of UCI bits, i.e., {}.",
                result.message.get_full_payload().size(),
                expected_nof_uci_bits);

  return result;
}

void pucch_processor_impl::assert_format1_config(const pucch_processor::format1_configuration& config)
{
  // Assert BWP allocation.
  srsran_assert(config.bwp_start_rb + config.bwp_size_rb <= max_sizes.nof_prb,
                "BWP allocation goes up to PRB {}, exceeding the configured maximum grid RB size, i.e., {}.",
                config.bwp_start_rb + config.bwp_size_rb,
                max_sizes.nof_prb);

  // Assert that the PRB allocation is constrained to the BWP. Recall that PUCCH Format 1 occupies a single PRB.
  srsran_assert(config.starting_prb < config.bwp_size_rb,
                "PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                config.starting_prb + 1,
                config.bwp_size_rb);

  if (config.second_hop_prb.has_value()) {
    srsran_assert(config.second_hop_prb.value() < config.bwp_size_rb,
                  "Second hop PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                  config.second_hop_prb.value() + 1,
                  config.bwp_size_rb);
  }

  // Assert that the OFDM symbols are constrained to the slot dimensions given by the CP.
  srsran_assert(config.start_symbol_index + config.nof_symbols <= get_nsymb_per_slot(config.cp),
                "OFDM symbol allocation goes up to symbol {}, exceeding the number of symbols in the given slot with "
                "{} CP, i.e., {}.",
                config.start_symbol_index + config.nof_symbols,
                config.cp.to_string(),
                get_nsymb_per_slot(config.cp));

  // Assert that the OFDM symbols are within the configured maximum slot dimensions.
  srsran_assert(
      config.start_symbol_index + config.nof_symbols <= max_sizes.nof_symbols,
      "OFDM symbol allocation goes up to symbol {}, exceeding the configured maximum number of slot symbols, i.e., {}.",
      config.start_symbol_index + config.nof_symbols,
      max_sizes.nof_symbols);

  // Assert the number of receive ports.
  srsran_assert(!config.ports.empty(), "The number of receive ports cannot be zero.");

  srsran_assert(
      config.ports.size() <= max_sizes.nof_rx_ports,
      "The number of receive ports, i.e. {}, exceeds the configured maximum number of receive ports, i.e., {}.",
      config.ports.size(),
      max_sizes.nof_rx_ports);
}

void pucch_processor_impl::assert_format2_config(const pucch_processor::format2_configuration& config)
{
  // Assert BWP allocation.
  srsran_assert(config.bwp_start_rb + config.bwp_size_rb <= max_sizes.nof_prb,
                "BWP allocation goes up to PRB {}, exceeding the configured maximum grid RB size, i.e., {}.",
                config.bwp_start_rb + config.bwp_size_rb,
                max_sizes.nof_prb);

  // Assert that the PRB set is constrained to the BWP.
  srsran_assert(config.starting_prb + config.nof_prb <= config.bwp_size_rb,
                "PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                config.starting_prb + config.nof_prb,
                config.bwp_size_rb);

  // Assert that the OFDM symbols are constrained to the slot dimensions given by the CP.
  srsran_assert(config.start_symbol_index + config.nof_symbols <= get_nsymb_per_slot(config.cp),
                "OFDM symbol allocation goes up to symbol {}, exceeding the number of symbols in the given slot with "
                "{} CP, i.e., {}.",
                config.start_symbol_index + config.nof_symbols,
                config.cp.to_string(),
                get_nsymb_per_slot(config.cp));

  // Assert that the OFDM symbols are within the configured maximum slot dimensions.
  srsran_assert(
      config.start_symbol_index + config.nof_symbols <= max_sizes.nof_symbols,
      "OFDM symbol allocation goes up to symbol {}, exceeding the configured maximum number of slot symbols, i.e., {}.",
      config.start_symbol_index + config.nof_symbols,
      max_sizes.nof_symbols);

  // Assert the number of receive ports.
  srsran_assert(!config.ports.empty(), "The number of receive ports cannot be zero.");

  srsran_assert(
      config.ports.size() <= max_sizes.nof_rx_ports,
      "The number of receive ports, i.e. {}, exceeds the configured maximum number of receive ports, i.e., {}.",
      config.ports.size(),
      max_sizes.nof_rx_ports);

  // CSI is not currently supported.
  srsran_assert(config.nof_csi_part2 == 0, "CSI Part 2 is not currently supported.");

  // PUCCH Format 2 frequency hopping is not currently supported.
  srsran_assert(!config.second_hop_prb.has_value(), "PUCCH Format 2 frequency hopping not supported.");

  // Expected UCI payload length.
  unsigned uci_payload_len = config.nof_harq_ack + config.nof_sr + config.nof_csi_part1 + config.nof_csi_part2;

  // Calculate effective code rate.
  float effective_code_rate = pucch_format2_code_rate(config.nof_prb, config.nof_symbols, uci_payload_len);
  srsran_assert(effective_code_rate <= pucch_constants::MAX_CODE_RATE,
                "The effective code rate (i.e., {}) exceeds the maximum allowed {}.",
                effective_code_rate,
                static_cast<float>(pucch_constants::MAX_CODE_RATE));

  srsran_assert((uci_payload_len >= pucch_constants::FORMAT2_MIN_UCI_NBITS) &&
                    (uci_payload_len <= FORMAT2_MAX_UCI_NBITS),
                "UCI Payload length, i.e., {} is not supported. Payload length must be {} to {} bits.",
                uci_payload_len,
                pucch_constants::FORMAT2_MIN_UCI_NBITS,
                static_cast<unsigned>(FORMAT2_MAX_UCI_NBITS));
}

bool pucch_pdu_validator_impl::is_valid(const pucch_processor::format1_configuration& config) const
{
  // The BWP size exceeds the grid dimensions.
  if ((config.bwp_start_rb + config.bwp_size_rb) > ce_dims.nof_prb) {
    return false;
  }

  // PRB allocation goes beyond the BWP. Recall that PUCCH Format 1 occupies a single PRB.
  if (config.starting_prb >= config.bwp_size_rb) {
    return false;
  }

  // Second hop PRB allocation goes beyond the BWP.
  if (config.second_hop_prb.has_value()) {
    if (config.second_hop_prb.value() >= config.bwp_size_rb) {
      return false;
    }
  }

  // None of the occupied symbols can exceed the configured maximum slot size, or the slot size given by the CP.
  if ((config.start_symbol_index + config.nof_symbols > get_nsymb_per_slot(config.cp)) ||
      (config.start_symbol_index + config.nof_symbols > ce_dims.nof_symbols)) {
    return false;
  }

  // The number of receive ports is either zero or exceeds the configured maximum number of receive ports.
  if (config.ports.empty() || (config.ports.size() > ce_dims.nof_rx_ports)) {
    return false;
  }

  return true;
}

bool pucch_pdu_validator_impl::is_valid(const pucch_processor::format2_configuration& config) const
{
  // Count total number of payload bits.
  unsigned nof_uci_bits = config.nof_harq_ack + config.nof_sr + config.nof_csi_part1 + config.nof_csi_part2;

  // Calculate effective code rate.
  float effective_code_rate = pucch_format2_code_rate(config.nof_prb, config.nof_symbols, nof_uci_bits);

  // The BWP size exceeds the grid dimensions.
  if ((config.bwp_start_rb + config.bwp_size_rb) > ce_dims.nof_prb) {
    return false;
  }

  // None of the occupied PRB within the BWP can exceed the BWP dimensions.
  if (config.starting_prb + config.nof_prb > config.bwp_size_rb) {
    return false;
  }

  // None of the occupied symbols can exceed the configured maximum slot size, or the slot size given by the CP.
  if ((config.start_symbol_index + config.nof_symbols > get_nsymb_per_slot(config.cp)) ||
      (config.start_symbol_index + config.nof_symbols > ce_dims.nof_symbols)) {
    return false;
  }

  // The number of receive ports is either zero or exceeds the configured maximum number of receive ports.
  if (config.ports.empty() || (config.ports.size() > ce_dims.nof_rx_ports)) {
    return false;
  }

  // CSI Part 2 is not supported.
  if (config.nof_csi_part2 != 0) {
    return false;
  }

  // Intra-slot frequency hopping is not supported.
  if (config.second_hop_prb.has_value()) {
    return false;
  }

  // The code rate shall not exceed the maximum.
  if (effective_code_rate > pucch_constants::MAX_CODE_RATE) {
    return false;
  }

  // UCI payload exceeds the UCI payload size boundaries.
  if (nof_uci_bits < pucch_constants::FORMAT2_MIN_UCI_NBITS ||
      nof_uci_bits > pucch_processor_impl::FORMAT2_MAX_UCI_NBITS) {
    return false;
  }

  return true;
}
