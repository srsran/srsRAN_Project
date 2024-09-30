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

#include "pucch_processor_impl.h"
#include "srsran/adt/expected.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/ran/pucch/pucch_info.h"
#include "srsran/support/transform_optional.h"
#include "fmt/core.h"
#include <functional>
#include <optional>

using namespace srsran;

/// Looks at the output of the validator and, if unsuccessful, fills msg with the error message.
/// This is used to call the validator inside the process methods only if asserts are active.
[[maybe_unused]] static bool handle_validation(std::string& msg, const error_type<std::string>& err)
{
  bool is_success = err.has_value();
  if (!is_success) {
    msg = err.error();
  }
  return is_success;
}

pucch_processor_result pucch_processor_impl::process(const resource_grid_reader&                   grid,
                                                     const pucch_processor::format0_configuration& config)
{
  [[maybe_unused]] std::string msg;
  srsran_assert(handle_validation(msg, pdu_validator->is_valid(config)), "{}", msg);

  // Calculate actual PRB.
  std::optional<unsigned> second_hop_prb;
  if (config.second_hop_prb.has_value()) {
    second_hop_prb.emplace(config.second_hop_prb.value() + config.bwp_start_rb);
  }

  pucch_detector::format0_configuration detector_config;
  detector_config.slot                                                     = config.slot;
  detector_config.starting_prb                                             = config.starting_prb + config.bwp_start_rb;
  detector_config.second_hop_prb                                           = second_hop_prb;
  detector_config.start_symbol_index                                       = config.start_symbol_index;
  detector_config.nof_symbols                                              = config.nof_symbols;
  detector_config.initial_cyclic_shift                                     = config.initial_cyclic_shift;
  detector_config.n_id                                                     = config.n_id;
  detector_config.nof_harq_ack                                             = config.nof_harq_ack;
  detector_config.sr_opportunity                                           = config.sr_opportunity;
  detector_config.ports                                                    = config.ports;
  std::pair<pucch_uci_message, channel_state_information> detection_result = detector->detect(grid, detector_config);

  pucch_processor_result result;
  result.message = detection_result.first;
  result.csi     = detection_result.second;

  return result;
}

pucch_processor_result pucch_processor_impl::process(const resource_grid_reader&  grid,
                                                     const format1_configuration& config)
{
  [[maybe_unused]] std::string msg;
  srsran_assert(handle_validation(msg, pdu_validator->is_valid(config)), "{}", msg);

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
  detector_config.ports                = config.ports;
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

  // Time alignment measurements are unreliable for PUCCH Format 1.
  result.csi.reset_time_alignment();

  return result;
}

pucch_processor_result pucch_processor_impl::process(const resource_grid_reader&  grid,
                                                     const format2_configuration& config)
{
  // Check that the PUCCH Format 2 configuration is valid.
  [[maybe_unused]] std::string msg;
  srsran_assert(handle_validation(msg, pdu_validator->is_valid(config)), "{}", msg);

  pucch_processor_result result;

  // PUCCH UCI message configuration.
  pucch_uci_message::configuration pucch_uci_message_config;
  pucch_uci_message_config.nof_sr        = config.nof_sr;
  pucch_uci_message_config.nof_harq_ack  = config.nof_harq_ack;
  pucch_uci_message_config.nof_csi_part1 = config.nof_csi_part1;
  pucch_uci_message_config.nof_csi_part2 = config.nof_csi_part2;

  result.message = pucch_uci_message(pucch_uci_message_config);

  // Channel estimator configuration.
  dmrs_pucch_processor::config_t estimator_config;
  estimator_config.format             = pucch_format::FORMAT_2;
  estimator_config.slot               = config.slot;
  estimator_config.cp                 = config.cp;
  estimator_config.group_hopping      = pucch_group_hopping::NEITHER;
  estimator_config.start_symbol_index = config.start_symbol_index;
  estimator_config.nof_symbols        = config.nof_symbols;
  estimator_config.starting_prb       = config.bwp_start_rb + config.starting_prb;
  estimator_config.intra_slot_hopping = config.second_hop_prb.has_value();
  estimator_config.second_hop_prb     = evaluate_or(config.second_hop_prb, 0U, std::plus(), config.bwp_start_rb);

  estimator_config.nof_prb              = config.nof_prb;
  estimator_config.initial_cyclic_shift = 0;
  estimator_config.time_domain_occ      = 0;
  estimator_config.additional_dmrs      = false;
  estimator_config.n_id                 = config.n_id;
  estimator_config.n_id_0               = config.n_id_0;
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
  pucch_demodulator::format2_configuration demod_config;
  demod_config.rx_ports           = config.ports;
  demod_config.first_prb          = config.bwp_start_rb + config.starting_prb;
  demod_config.second_hop_prb     = transform_optional(config.second_hop_prb, std::plus(), config.bwp_start_rb);
  demod_config.nof_prb            = config.nof_prb;
  demod_config.start_symbol_index = config.start_symbol_index;
  demod_config.nof_symbols        = config.nof_symbols;
  demod_config.rnti               = config.rnti;
  demod_config.n_id               = config.n_id;

  // Perform demodulation.
  demodulator->demodulate(llr, grid, estimates, demod_config);

  // UCI decoder configuration.
  uci_decoder::configuration decoder_config;
  decoder_config.modulation = modulation_scheme::QPSK;

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

error_type<std::string> pucch_pdu_validator_impl::is_valid(const pucch_processor::format0_configuration& config) const
{
  // BWP PRB shall not exceed the maximum.
  if (config.bwp_start_rb + config.bwp_size_rb > MAX_RB) {
    return make_unexpected(
        fmt::format("BWP allocation goes up to PRB {}, exceeding the configured maximum grid RB size, i.e., {}.",
                    config.bwp_start_rb + config.bwp_size_rb,
                    MAX_RB));
  }

  // PRB allocation goes beyond the BWP. Recall that PUCCH Format 0 occupies a single PRB.
  if (config.starting_prb >= config.bwp_size_rb) {
    return make_unexpected(fmt::format("PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                                       config.starting_prb + 1,
                                       config.bwp_size_rb));
  }

  // Second hop PRB allocation goes beyond the BWP.
  if (config.second_hop_prb.has_value()) {
    if (config.second_hop_prb.value() >= config.bwp_size_rb) {
      return make_unexpected(
          fmt::format("Second hop PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                      config.second_hop_prb.value() + 1,
                      config.bwp_size_rb));
    }
  }

  // The number of symbols shall be in the range.
  if (!pucch_constants::format0_nof_symbols_range.contains(config.nof_symbols)) {
    return make_unexpected(fmt::format("Number of symbols (i.e., {}) is out of the range {}.",
                                       config.nof_symbols,
                                       pucch_constants::format0_nof_symbols_range));
  }

  // None of the occupied symbols can exceed the configured maximum slot size, or the slot size given by the CP.
  if (config.start_symbol_index + config.nof_symbols > get_nsymb_per_slot(config.cp)) {
    return make_unexpected(fmt::format(
        "OFDM symbol allocation goes up to symbol {}, exceeding the number of symbols in the given slot with "
        "{} CP, i.e., {}.",
        config.start_symbol_index + config.nof_symbols,
        config.cp.to_string(),
        get_nsymb_per_slot(config.cp)));
  }

  // Initial cyclic shift must be in range.
  if (!pucch_constants::format0_initial_cyclic_shift_range.contains(config.initial_cyclic_shift)) {
    return make_unexpected(fmt::format("The initial cyclic shift (i.e., {}) is out of the range {}.",
                                       config.initial_cyclic_shift,
                                       pucch_constants::format0_initial_cyclic_shift_range));
  }

  // Hopping identifier must be in range.
  if (!pucch_constants::n_id_range.contains(config.n_id)) {
    return make_unexpected(fmt::format("The sequence hopping identifier (i.e., {}) is out of the range {}.",
                                       config.n_id,
                                       pucch_constants::n_id_range));
  }

  // No payload detected.
  if ((config.nof_harq_ack == 0) && !config.sr_opportunity) {
    return make_unexpected(fmt::format("No payload."));
  }

  // Number of HARQ-ACK exceeds maximum.
  if (!pucch_constants::format0_nof_harq_ack_range.contains(config.nof_harq_ack)) {
    return make_unexpected(fmt::format("The number of HARQ-ACK bits (i.e., {}) is out of the range {}.",
                                       config.nof_harq_ack,
                                       pucch_constants::format0_nof_harq_ack_range));
  }

  // The number of receive ports must not be empty.
  if (config.ports.empty()) {
    return make_unexpected(fmt::format("The number of receive ports cannot be zero."));
  }

  return default_success_t();
}

error_type<std::string> pucch_pdu_validator_impl::is_valid(const pucch_processor::format1_configuration& config) const
{
  // The BWP size exceeds the grid dimensions.
  if ((config.bwp_start_rb + config.bwp_size_rb) > ce_dims.nof_prb) {
    return make_unexpected(
        fmt::format("BWP allocation goes up to PRB {}, exceeding the configured maximum grid RB size, i.e., {}.",
                    config.bwp_start_rb + config.bwp_size_rb,
                    ce_dims.nof_prb));
  }

  // PRB allocation goes beyond the BWP. Recall that PUCCH Format 1 occupies a single PRB.
  if (config.starting_prb >= config.bwp_size_rb) {
    return make_unexpected(fmt::format("PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                                       config.starting_prb + 1,
                                       config.bwp_size_rb));
  }

  // Second hop PRB allocation goes beyond the BWP.
  if (config.second_hop_prb.has_value()) {
    if (config.second_hop_prb.value() >= config.bwp_size_rb) {
      return make_unexpected(
          fmt::format("Second hop PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                      config.second_hop_prb.value() + 1,
                      config.bwp_size_rb));
    }
  }

  // None of the occupied symbols can exceed the configured maximum slot size, or the slot size given by the CP.
  if (config.start_symbol_index + config.nof_symbols > get_nsymb_per_slot(config.cp)) {
    return make_unexpected(fmt::format(
        "OFDM symbol allocation goes up to symbol {}, exceeding the number of symbols in the given slot with "
        "{} CP, i.e., {}.",
        config.start_symbol_index + config.nof_symbols,
        config.cp.to_string(),
        get_nsymb_per_slot(config.cp)));
  }
  if (config.start_symbol_index + config.nof_symbols > ce_dims.nof_symbols) {
    return make_unexpected(fmt::format("OFDM symbol allocation goes up to symbol {}, exceeding the configured maximum "
                                       "number of slot symbols, i.e., {}.",
                                       config.start_symbol_index + config.nof_symbols,
                                       ce_dims.nof_symbols));
  }

  // The number of receive ports is either zero or exceeds the configured maximum number of receive ports.
  if (config.ports.empty()) {
    return make_unexpected(fmt::format("The number of receive ports cannot be zero."));
  }
  if (config.ports.size() > ce_dims.nof_rx_ports) {
    return make_unexpected(fmt::format(
        "The number of receive ports, i.e. {}, exceeds the configured maximum number of receive ports, i.e., {}.",
        config.ports.size(),
        ce_dims.nof_rx_ports));
  }

  return default_success_t();
}

error_type<std::string> pucch_pdu_validator_impl::is_valid(const pucch_processor::format2_configuration& config) const
{
  // The BWP size exceeds the grid dimensions.
  if ((config.bwp_start_rb + config.bwp_size_rb) > ce_dims.nof_prb) {
    return make_unexpected(
        fmt::format("BWP allocation goes up to PRB {}, exceeding the configured maximum grid RB size, i.e., {}.",
                    config.bwp_start_rb + config.bwp_size_rb,
                    ce_dims.nof_prb));
  }

  // None of the occupied PRB within the BWP can exceed the BWP dimensions.
  if (config.starting_prb + config.nof_prb > config.bwp_size_rb) {
    return make_unexpected(fmt::format("PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                                       config.starting_prb + config.nof_prb,
                                       config.bwp_size_rb));
  }

  // None of the occupied symbols can exceed the configured maximum slot size, or the slot size given by the CP.
  if (config.start_symbol_index + config.nof_symbols > get_nsymb_per_slot(config.cp)) {
    return make_unexpected(fmt::format(
        "OFDM symbol allocation goes up to symbol {}, exceeding the number of symbols in the given slot with "
        "{} CP, i.e., {}.",
        config.start_symbol_index + config.nof_symbols,
        config.cp.to_string(),
        get_nsymb_per_slot(config.cp)));
  }

  if (config.start_symbol_index + config.nof_symbols > ce_dims.nof_symbols) {
    return make_unexpected(fmt::format("OFDM symbol allocation goes up to symbol {}, exceeding the configured maximum "
                                       "number of slot symbols, i.e., {}.",
                                       config.start_symbol_index + config.nof_symbols,
                                       ce_dims.nof_symbols));
  }

  // The number of receive ports is either zero or exceeds the configured maximum number of receive ports.
  if (config.ports.empty()) {
    return make_unexpected(fmt::format("The number of receive ports cannot be zero."));
  }

  if (config.ports.size() > ce_dims.nof_rx_ports) {
    return make_unexpected(fmt::format(
        "The number of receive ports, i.e. {}, exceeds the configured maximum number of receive ports, i.e., {}.",
        config.ports.size(),
        ce_dims.nof_rx_ports));
  }

  // CSI Part 2 is not supported.
  if (config.nof_csi_part2 != 0) {
    return make_unexpected(fmt::format("CSI Part 2 is not currently supported."));
  }

  // Count total number of payload bits.
  unsigned nof_uci_bits = config.nof_harq_ack + config.nof_sr + config.nof_csi_part1 + config.nof_csi_part2;

  // Calculate effective code rate.
  float effective_code_rate = pucch_format2_code_rate(config.nof_prb, config.nof_symbols, nof_uci_bits);

  // The code rate shall not exceed the maximum.
  if (effective_code_rate > pucch_constants::MAX_CODE_RATE) {
    return make_unexpected(fmt::format("The effective code rate (i.e., {}) exceeds the maximum allowed {}.",
                                       effective_code_rate,
                                       static_cast<float>(pucch_constants::MAX_CODE_RATE)));
  }

  // UCI payload exceeds the UCI payload size boundaries.
  if (nof_uci_bits < pucch_constants::FORMAT2_MIN_UCI_NBITS ||
      nof_uci_bits > pucch_processor_impl::FORMAT2_MAX_UCI_NBITS) {
    return make_unexpected(
        fmt::format("UCI Payload length, i.e., {} is not supported. Payload length must be {} to {} bits.",
                    nof_uci_bits,
                    pucch_constants::FORMAT2_MIN_UCI_NBITS,
                    static_cast<unsigned>(pucch_processor_impl::FORMAT2_MAX_UCI_NBITS)));
  }

  return default_success_t();
}
