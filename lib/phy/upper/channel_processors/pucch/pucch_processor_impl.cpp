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

#include "pucch_processor_impl.h"
#include "srsran/phy/upper/pucch_formats3_4_helpers.h"
#include "srsran/ran/pucch/pucch_info.h"
#include "srsran/support/transform_optional.h"

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

pucch_processor_result pucch_processor_impl::process(const resource_grid_reader&                   grid,
                                                     const pucch_processor::format0_configuration& config)
{
  [[maybe_unused]] std::string msg;
  srsran_assert(handle_validation(msg, pdu_validator->is_valid(config)), "{}", msg);

  // Calculate actual PRB.
  std::optional<unsigned> second_hop_prb;
  if (config.second_hop_prb.has_value()) {
    second_hop_prb.emplace(*config.second_hop_prb + config.bwp_start_rb);
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

pucch_format1_map<pucch_processor_result> pucch_processor_impl::process(const resource_grid_reader&        grid,
                                                                        const format1_batch_configuration& batch_config)
{
  const format1_common_configuration& common_config = batch_config.common_config;
  format1_configuration               proc_config   = {.context              = {},
                                                       .slot                 = common_config.slot,
                                                       .bwp_size_rb          = common_config.bwp_size_rb,
                                                       .bwp_start_rb         = common_config.bwp_start_rb,
                                                       .cp                   = common_config.cp,
                                                       .starting_prb         = common_config.starting_prb,
                                                       .second_hop_prb       = common_config.second_hop_prb,
                                                       .n_id                 = common_config.n_id,
                                                       .nof_harq_ack         = 0,
                                                       .ports                = common_config.ports,
                                                       .initial_cyclic_shift = 0,
                                                       .nof_symbols          = common_config.nof_symbols,
                                                       .start_symbol_index   = common_config.start_symbol_index,
                                                       .time_domain_occ      = 0};

  pucch_format1_map<unsigned> mux_harq_size;

  for (const auto& this_pucch : batch_config.entries) {
    unsigned initial_cyclic_shift = this_pucch.initial_cyclic_shift;
    unsigned time_domain_occ      = this_pucch.time_domain_occ;
    unsigned nof_harq_ack         = this_pucch.value.nof_harq_ack;

    proc_config.context              = this_pucch.value.context;
    proc_config.initial_cyclic_shift = initial_cyclic_shift;
    proc_config.time_domain_occ      = time_domain_occ;
    proc_config.nof_harq_ack         = nof_harq_ack;

    [[maybe_unused]] std::string msg;
    srsran_assert(handle_validation(msg, pdu_validator->is_valid(proc_config)), "{}", msg);

    mux_harq_size.insert(initial_cyclic_shift, time_domain_occ, nof_harq_ack);
  }

  // Fill the detector configuration - recall that time_domain_occ, initial_cyclic_shift and nof_harq_ack are unused
  // since given by mux_harq_size.
  pucch_detector::format1_configuration detector_config = {
      .slot                 = common_config.slot,
      .cp                   = common_config.cp,
      .starting_prb         = common_config.starting_prb + common_config.bwp_start_rb,
      .second_hop_prb       = transform_optional(common_config.second_hop_prb, std::plus(), common_config.bwp_start_rb),
      .start_symbol_index   = common_config.start_symbol_index,
      .nof_symbols          = common_config.nof_symbols,
      .group_hopping        = pucch_group_hopping::NEITHER,
      .ports                = common_config.ports,
      .beta_pucch           = 1.0F,
      .time_domain_occ      = 0,
      .initial_cyclic_shift = 0,
      .n_id                 = common_config.n_id,
      .nof_harq_ack         = 0};

  const pucch_format1_map<pucch_detector::pucch_detection_result_csi>& detection_results =
      detector->detect(grid, detector_config, mux_harq_size);

  // Create the detection results for this detection batch.
  pucch_format1_map<pucch_processor_result> batch_results;
  for (const auto& this_result : detection_results) {
    batch_results.insert(this_result.initial_cyclic_shift,
                         this_result.time_domain_occ,
                         {.csi              = this_result.value.csi,
                          .message          = this_result.value.detection_result.uci_message,
                          .detection_metric = this_result.value.detection_result.detection_metric});
  }
  return batch_results;
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
  dmrs_pucch_estimator::format2_configuration estimator_config;
  estimator_config.slot               = config.slot;
  estimator_config.cp                 = config.cp;
  estimator_config.group_hopping      = pucch_group_hopping::NEITHER;
  estimator_config.start_symbol_index = config.start_symbol_index;
  estimator_config.nof_symbols        = config.nof_symbols;
  estimator_config.starting_prb       = config.bwp_start_rb + config.starting_prb;
  estimator_config.second_hop_prb     = transform_optional(config.second_hop_prb, std::plus(), config.bwp_start_rb);
  estimator_config.nof_prb            = config.nof_prb;
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
  channel_estimator->estimate(estimates, grid, estimator_config);

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

pucch_processor_result pucch_processor_impl::process(const resource_grid_reader&  grid,
                                                     const format3_configuration& config)
{
  // Check that the PUCCH Format 3 configuration is valid.
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
  dmrs_pucch_estimator::format3_configuration estimator_config;
  estimator_config.slot               = config.slot;
  estimator_config.cp                 = config.cp;
  estimator_config.group_hopping      = pucch_group_hopping::NEITHER;
  estimator_config.start_symbol_index = config.start_symbol_index;
  estimator_config.nof_symbols        = config.nof_symbols;
  estimator_config.starting_prb       = config.bwp_start_rb + config.starting_prb;
  estimator_config.second_hop_prb     = transform_optional(config.second_hop_prb, std::plus(), config.bwp_start_rb);
  estimator_config.nof_prb            = config.nof_prb;
  estimator_config.n_id               = config.n_id_hopping;
  estimator_config.ports.assign(config.ports.begin(), config.ports.end());
  estimator_config.additional_dmrs = config.additional_dmrs;

  // Prepare channel estimate.
  channel_estimate::channel_estimate_dimensions dims;
  dims.nof_prb       = config.bwp_start_rb + config.bwp_size_rb;
  dims.nof_symbols   = get_nsymb_per_slot(config.cp);
  dims.nof_rx_ports  = config.ports.size();
  dims.nof_tx_layers = pucch_constants::MAX_LAYERS;

  estimates.resize(dims);

  // Perform channel estimation.
  channel_estimator->estimate(estimates, grid, estimator_config);

  estimates.get_channel_state_information(result.csi);

  const symbol_slot_mask dmrs_symb_mask = get_pucch_formats3_4_dmrs_symbol_mask(
      config.nof_symbols, config.second_hop_prb.has_value(), config.additional_dmrs);
  const modulation_scheme mod_scheme = config.pi2_bpsk ? modulation_scheme::PI_2_BPSK : modulation_scheme::QPSK;

  span<log_likelihood_ratio> llr = span<log_likelihood_ratio>(temp_llr).first(
      NRE * config.nof_prb * (config.nof_symbols - dmrs_symb_mask.count()) * get_bits_per_symbol(mod_scheme));

  // PUCCH Format 3 demodulator configuration.
  pucch_demodulator::format3_configuration demod_config;
  demod_config.rx_ports           = config.ports;
  demod_config.first_prb          = config.bwp_start_rb + config.starting_prb;
  demod_config.second_hop_prb     = transform_optional(config.second_hop_prb, std::plus(), config.bwp_start_rb);
  demod_config.nof_prb            = config.nof_prb;
  demod_config.start_symbol_index = config.start_symbol_index;
  demod_config.nof_symbols        = config.nof_symbols;
  demod_config.rnti               = config.rnti;
  demod_config.n_id               = config.n_id_scrambling;
  demod_config.additional_dmrs    = config.additional_dmrs;
  demod_config.pi2_bpsk           = config.pi2_bpsk;

  // Perform demodulation.
  demodulator->demodulate(llr, grid, estimates, demod_config);

  // UCI decoder configuration.
  uci_decoder::configuration decoder_config;
  decoder_config.modulation = mod_scheme;

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

pucch_processor_result pucch_processor_impl::process(const resource_grid_reader&  grid,
                                                     const format4_configuration& config)
{
  // Check that the PUCCH Format 4 configuration is valid.
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
  dmrs_pucch_estimator::format4_configuration estimator_config;
  estimator_config.slot               = config.slot;
  estimator_config.cp                 = config.cp;
  estimator_config.group_hopping      = pucch_group_hopping::NEITHER;
  estimator_config.start_symbol_index = config.start_symbol_index;
  estimator_config.nof_symbols        = config.nof_symbols;
  estimator_config.starting_prb       = config.bwp_start_rb + config.starting_prb;
  estimator_config.second_hop_prb     = transform_optional(config.second_hop_prb, std::plus(), config.bwp_start_rb);
  estimator_config.n_id               = config.n_id_hopping;
  estimator_config.ports.assign(config.ports.begin(), config.ports.end());
  estimator_config.additional_dmrs = config.additional_dmrs;
  estimator_config.occ_index       = config.occ_index;

  // Prepare channel estimate.
  channel_estimate::channel_estimate_dimensions dims;
  dims.nof_prb       = config.bwp_start_rb + config.bwp_size_rb;
  dims.nof_symbols   = get_nsymb_per_slot(config.cp);
  dims.nof_rx_ports  = config.ports.size();
  dims.nof_tx_layers = pucch_constants::MAX_LAYERS;

  estimates.resize(dims);

  // Perform channel estimation.
  channel_estimator->estimate(estimates, grid, estimator_config);

  estimates.get_channel_state_information(result.csi);

  const symbol_slot_mask dmrs_symb_mask = get_pucch_formats3_4_dmrs_symbol_mask(
      config.nof_symbols, config.second_hop_prb.has_value(), config.additional_dmrs);
  const modulation_scheme mod_scheme = config.pi2_bpsk ? modulation_scheme::PI_2_BPSK : modulation_scheme::QPSK;

  span<log_likelihood_ratio> llr = span<log_likelihood_ratio>(temp_llr).first(
      NRE * (config.nof_symbols - dmrs_symb_mask.count()) * get_bits_per_symbol(mod_scheme) / config.occ_length);

  // PUCCH Format 4 demodulator configuration.
  pucch_demodulator::format4_configuration demod_config;
  demod_config.rx_ports           = config.ports;
  demod_config.first_prb          = config.bwp_start_rb + config.starting_prb;
  demod_config.second_hop_prb     = transform_optional(config.second_hop_prb, std::plus(), config.bwp_start_rb);
  demod_config.start_symbol_index = config.start_symbol_index;
  demod_config.nof_symbols        = config.nof_symbols;
  demod_config.rnti               = config.rnti;
  demod_config.n_id               = config.n_id_scrambling;
  demod_config.additional_dmrs    = config.additional_dmrs;
  demod_config.pi2_bpsk           = config.pi2_bpsk;
  demod_config.occ_index          = config.occ_index;
  demod_config.occ_length         = config.occ_length;

  // Perform demodulation.
  demodulator->demodulate(llr, grid, estimates, demod_config);

  // UCI decoder configuration.
  uci_decoder::configuration decoder_config;
  decoder_config.modulation = mod_scheme;

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
    if (config.second_hop_prb >= config.bwp_size_rb) {
      return make_unexpected(
          fmt::format("Second hop PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                      *config.second_hop_prb + 1,
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
    if (config.second_hop_prb >= config.bwp_size_rb) {
      return make_unexpected(
          fmt::format("Second hop PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                      *config.second_hop_prb + 1,
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
      nof_uci_bits > pucch_constants::FORMATS_2_3_4_MAX_UCI_NBITS) {
    return make_unexpected(
        fmt::format("UCI Payload length, i.e., {} is not supported. Payload length must be {} to {} bits.",
                    nof_uci_bits,
                    pucch_constants::FORMAT2_MIN_UCI_NBITS,
                    static_cast<unsigned>(pucch_constants::FORMATS_2_3_4_MAX_UCI_NBITS)));
  }

  return default_success_t();
}

error_type<std::string> pucch_pdu_validator_impl::is_valid(const pucch_processor::format3_configuration& config) const
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
  symbol_slot_mask dmrs_symb_mask = get_pucch_formats3_4_dmrs_symbol_mask(
      config.nof_symbols, config.second_hop_prb.has_value(), config.additional_dmrs);
  float effective_code_rate = pucch_format3_code_rate(
      config.nof_prb, config.nof_symbols - dmrs_symb_mask.count(), config.pi2_bpsk, nof_uci_bits);

  // The code rate shall not exceed the maximum.
  if (effective_code_rate > pucch_constants::MAX_CODE_RATE) {
    return make_unexpected(fmt::format("The effective code rate (i.e., {}) exceeds the maximum allowed {}.",
                                       effective_code_rate,
                                       static_cast<float>(pucch_constants::MAX_CODE_RATE)));
  }

  // UCI payload exceeds the UCI payload size boundaries.
  interval<unsigned, true> nof_uci_bits_range(pucch_constants::FORMAT4_MIN_UCI_NBITS,
                                              pucch_constants::FORMATS_2_3_4_MAX_UCI_NBITS);
  if (!nof_uci_bits_range.contains(nof_uci_bits)) {
    return make_unexpected(fmt::format(
        "UCI Payload length (i.e., {}) is outside the supported range (i.e., {}).", nof_uci_bits, nof_uci_bits_range));
  }

  // The number of allocated PRBs is outside the allowed range.
  static constexpr interval<unsigned, true> nof_prb_range(1, 16);
  if (!nof_prb_range.contains(config.nof_prb)) {
    return make_unexpected(
        fmt::format("Number of PRBs (i.e., {}) is outside the allowed range for PUCCH Format 3 (i.e., {}).",
                    config.nof_prb,
                    nof_prb_range));
  }

  return default_success_t();
}

error_type<std::string> pucch_pdu_validator_impl::is_valid(const pucch_processor::format4_configuration& config) const
{
  // The BWP size exceeds the grid dimensions.
  if ((config.bwp_start_rb + config.bwp_size_rb) > ce_dims.nof_prb) {
    return make_unexpected(
        fmt::format("BWP allocation goes up to PRB {}, exceeding the configured maximum grid RB size, i.e., {}.",
                    config.bwp_start_rb + config.bwp_size_rb,
                    ce_dims.nof_prb));
  }

  // None of the occupied PRB within the BWP can exceed the BWP dimensions.
  if (config.starting_prb + 1 > config.bwp_size_rb) {
    return make_unexpected(fmt::format("PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                                       config.starting_prb + 1,
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
  symbol_slot_mask dmrs_symb_mask = get_pucch_formats3_4_dmrs_symbol_mask(
      config.nof_symbols, config.second_hop_prb.has_value(), config.additional_dmrs);
  float effective_code_rate = pucch_format4_code_rate(
      config.occ_length, config.nof_symbols - dmrs_symb_mask.count(), config.pi2_bpsk, nof_uci_bits);

  // The code rate shall not exceed the maximum.
  if (effective_code_rate > pucch_constants::MAX_CODE_RATE) {
    return make_unexpected(fmt::format("The effective code rate (i.e., {}) exceeds the maximum allowed {}.",
                                       effective_code_rate,
                                       static_cast<float>(pucch_constants::MAX_CODE_RATE)));
  }

  // UCI payload exceeds the UCI payload size boundaries.
  interval<unsigned, true> nof_uci_bits_range(pucch_constants::FORMAT4_MIN_UCI_NBITS,
                                              pucch_constants::FORMATS_2_3_4_MAX_UCI_NBITS);
  if (!nof_uci_bits_range.contains(nof_uci_bits)) {
    return make_unexpected(fmt::format(
        "UCI Payload length (i.e., {}) is outside the supported range (i.e., {}).", nof_uci_bits, nof_uci_bits_range));
  }

  // The OCC length is invalid.
  if ((config.occ_length != 2) && (config.occ_length != 4)) {
    return make_unexpected(
        fmt::format("Invalid OCC length value (i.e., {}). Valid values are 2 and 4.", config.occ_length));
  }

  return default_success_t();
}
