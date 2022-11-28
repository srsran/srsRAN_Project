/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pucch_processor_impl.h"

using namespace srsgnb;

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
  estimator_config.ports                = config.ports;

  // Unused channel estimator parameters for this format.
  estimator_config.group_hopping   = pucch_group_hopping::NEITHER;
  estimator_config.nof_prb         = 0;
  estimator_config.n_id_0          = 0;
  estimator_config.additional_dmrs = false;

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
  pucch_uci_message message = detector->detect(grid, estimates, detector_config);

  // Prepare result.
  pucch_processor_result result;
  result.csi     = estimates.get_channel_state_information();
  result.message = message;

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
  estimator_config.ports              = config.ports;

  // Perform channel estimation.
  channel_estimator_format_2->estimate(estimates, grid, estimator_config);

  result.csi = estimates.get_channel_state_information();

  span<log_likelihood_ratio> llr = span<log_likelihood_ratio>(temp_llr).first(
      PUCCH_FORMAT2_NOF_DATA_SC * config.nof_prb * config.nof_symbols * get_bits_per_symbol(modulation_scheme::QPSK));

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
  srsgnb_assert(result.message.get_full_payload().size() == expected_nof_uci_bits,
                "Decoded UCI payload length, i.e., {}, does not match expected number of UCI bits, i.e., {}.",
                result.message.get_full_payload().size(),
                expected_nof_uci_bits);

  return result;
}

void pucch_processor_impl::assert_format1_config(const pucch_processor::format1_configuration& config)
{
  // Extended CP is only for 60 kHz subcarrier spacing, see TS38.211 Section 4.2.
  srsgnb_assert((config.cp != cyclic_prefix::EXTENDED) || (config.slot.numerology() == 2),
                "Extended CP is only supported for 60 kHz SCS.");

  // Assert BWP allocation.
  srsgnb_assert(config.bwp_size_rb != 0, "BWP size cannot be zero.");

  srsgnb_assert(config.bwp_start_rb + config.bwp_size_rb <= max_sizes.nof_prb,
                "BWP allocation goes up to PRB {}, exceeding the configured maximum grid RB size, i.e., {}.",
                config.bwp_start_rb + config.bwp_size_rb,
                max_sizes.nof_prb);

  // Assert that the PRB allocation is constrained to the BWP.
  srsgnb_assert(config.starting_prb < config.bwp_size_rb,
                "PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                config.starting_prb + 1,
                config.bwp_size_rb);

  if (config.second_hop_prb.has_value()) {
    srsgnb_assert(config.second_hop_prb.value() < config.bwp_size_rb,
                  "Second hop PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                  config.second_hop_prb.value() + 1,
                  config.bwp_size_rb);
  }

  // Assert OFDM symbol allocation.
  srsgnb_assert((config.nof_symbols >= PUCCH_FORMAT1_MIN_NSYMB) && (config.nof_symbols <= PUCCH_FORMAT1_MAX_NSYMB),
                "Invalid number of OFDM Symbols, i.e., {}. PUCCH Format 1 occupies {} to {} symbols.",
                config.nof_symbols,
                PUCCH_FORMAT1_MIN_NSYMB,
                PUCCH_FORMAT1_MAX_NSYMB);

  // Assert that the OFDM symbols are constrained to the slot dimensions given by the CP.
  srsgnb_assert(config.start_symbol_index + config.nof_symbols <= get_nsymb_per_slot(config.cp),
                "OFDM symbol allocation goes up to symbol {}, exceeding the number of symbols in the slot given {} "
                "CP, i.e., {}.",
                config.start_symbol_index + config.nof_symbols,
                config.cp.to_string(),
                get_nsymb_per_slot(config.cp));

  // Assert that the OFDM symbols are within the configured maximum slot dimensions.
  srsgnb_assert(
      config.start_symbol_index + config.nof_symbols <= max_sizes.nof_symbols,
      "OFDM symbol allocation goes up to symbol {}, exceeding the configured maximum number of slot symbols, i.e., {}.",
      config.start_symbol_index + config.nof_symbols,
      max_sizes.nof_symbols);

  // Assert the number of receive ports.
  srsgnb_assert(!config.ports.empty(), "number of receive ports cannot be zero.");

  srsgnb_assert(config.ports.size() <= max_sizes.nof_rx_ports,
                "Number of receive ports, i.e. {}, exceeds the configured maximum number of receive ports, i.e., {}.",
                config.ports.size(),
                max_sizes.nof_rx_ports);

  // Assert the time domain OCC index.
  srsgnb_assert(config.time_domain_occ <= pucch_pdu_validator_impl::FORMAT1_MAX_OCC_IDX,
                "The time domain OCC index, i.e., {}, exceeds its maximum value of {}.",
                config.time_domain_occ,
                static_cast<unsigned>(pucch_pdu_validator_impl::FORMAT1_MAX_OCC_IDX));

  // Assert the initial cyclic shift.
  srsgnb_assert(config.initial_cyclic_shift <= pucch_pdu_validator_impl::FORMAT1_MAX_CYCLIC_SHIFT,
                "The initial cyclic shift, i.e., {}, exceeds its maximum value of {}.",
                config.initial_cyclic_shift,
                static_cast<unsigned>(pucch_pdu_validator_impl::FORMAT1_MAX_CYCLIC_SHIFT));

  // Assert the NID.
  srsgnb_assert(config.n_id <= pucch_pdu_validator_impl::FORMAT1_MAX_NID,
                "The NID value, i.e., {}, exceeds its maximum value of {}.",
                config.n_id,
                static_cast<unsigned>(pucch_pdu_validator_impl::FORMAT1_MAX_NID));

  // Assert the number of HARQ-ACK.
  srsgnb_assert(config.nof_harq_ack <= PUCCH_FORMAT1_MAX_HARQ_ACK,
                "The number of requested HARQ-ACK, i.e., {}, exceeds the PUCCH Format 1 number of HARQ-ACK, i.e., {}.",
                config.nof_harq_ack,
                PUCCH_FORMAT1_MAX_HARQ_ACK);
}

void pucch_processor_impl::assert_format2_config(const pucch_processor::format2_configuration& config)
{
  // Extended CP is only for 60 kHz subcarrier spacing, see TS38.211 Section 4.2.
  srsgnb_assert((config.cp != cyclic_prefix::EXTENDED) || (config.slot.numerology() == 2),
                "Extended CP is only supported for 60 kHz SCS.");

  // Assert BWP allocation.
  srsgnb_assert(config.bwp_size_rb != 0, "BWP size cannot be zero.");

  srsgnb_assert(config.bwp_start_rb + config.bwp_size_rb <= max_sizes.nof_prb,
                "BWP allocation goes up to PRB {}, exceeding the configured maximum grid RB size, i.e., {}.",
                config.bwp_start_rb + config.bwp_size_rb,
                max_sizes.nof_prb);

  // Assert PRB allocation.
  srsgnb_assert((config.nof_prb > 0) && (config.nof_prb <= PUCCH_FORMAT2_MAX_NPRB),
                "Invalid number of PRB, i.e., {}. PUCCH Format 2 occupies 1 to {} PRB.",
                config.nof_prb,
                PUCCH_FORMAT2_MAX_NPRB);

  // Assert that the PRB set is constrained to the BWP.
  srsgnb_assert(config.starting_prb + config.nof_prb <= config.bwp_size_rb,
                "PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                config.starting_prb + config.nof_prb,
                config.bwp_size_rb);

  // Assert OFDM symbol allocation.
  srsgnb_assert((config.nof_symbols > 0) && (config.nof_symbols <= PUCCH_FORMAT2_MAX_NSYMB),
                "Invalid number of OFDM Symbols, i.e., {}. PUCCH Format 2 occupies 1 to {} symbols.",
                config.nof_symbols,
                PUCCH_FORMAT2_MAX_NSYMB);

  // Assert that the OFDM symbols are constrained to the slot dimensions given by the CP.
  srsgnb_assert(config.start_symbol_index + config.nof_symbols <= get_nsymb_per_slot(config.cp),
                "OFDM symbol allocation goes up to symbol {}, exceeding the number of symbols in the slot given {} "
                "CP, i.e., {}.",
                config.start_symbol_index + config.nof_symbols,
                config.cp.to_string(),
                get_nsymb_per_slot(config.cp));

  // Assert that the OFDM symbols are within the configured maximum slot dimensions.
  srsgnb_assert(
      config.start_symbol_index + config.nof_symbols <= max_sizes.nof_symbols,
      "OFDM symbol allocation goes up to symbol {}, exceeding the configured maximum number of slot symbols, i.e., {}.",
      config.start_symbol_index + config.nof_symbols,
      max_sizes.nof_symbols);

  // Assert the number of receive ports.
  srsgnb_assert(!config.ports.empty(), "number of receive ports cannot be zero.");

  srsgnb_assert(config.ports.size() <= max_sizes.nof_rx_ports,
                "Number of receive ports, i.e. {}, exceeds the configured maximum number of receive ports, i.e., {}.",
                config.ports.size(),
                max_sizes.nof_rx_ports);

  // CSI is not currently supported.
  srsgnb_assert(config.nof_csi_part1 == 0, "CSI Part 1 is not currently supported.");
  srsgnb_assert(config.nof_csi_part2 == 0, "CSI Part 2 is not currently supported.");

  // PUCCH Format 2 frequency hopping is not currently supported.
  srsgnb_assert(!config.second_hop_prb.has_value(), "PUCCH Format 2 frequency hopping not supported.");

  // Expected UCI payload length.
  unsigned uci_payload_len = config.nof_harq_ack + config.nof_sr + config.nof_csi_part1 + config.nof_csi_part2;

  srsgnb_assert((uci_payload_len >= PUCCH_FORMAT2_MIN_UCI_NBITS) &&
                    (uci_payload_len <= pucch_pdu_validator_impl::FORMAT2_MAX_UCI_NBITS),
                "UCI Payload length, i.e., {} is not supported. Payload length must be {} to {} bits.",
                uci_payload_len,
                PUCCH_FORMAT2_MIN_UCI_NBITS,
                static_cast<unsigned>(pucch_pdu_validator_impl::FORMAT2_MAX_UCI_NBITS));

  // Assert the NID.
  srsgnb_assert(config.n_id <= pucch_pdu_validator_impl::FORMAT2_MAX_NID,
                "The NID value, i.e., {}, exceeds its maximum value of {}.",
                config.n_id,
                static_cast<unsigned>(pucch_pdu_validator_impl::FORMAT2_MAX_NID));

  // Assert the NID_0.
  srsgnb_assert(config.n_id_0 <= pucch_pdu_validator_impl::FORMAT2_MAX_NID_0,
                "The NID_0 value, i.e., {}, exceeds its maximum value of {}.",
                config.n_id_0,
                static_cast<unsigned>(pucch_pdu_validator_impl::FORMAT2_MAX_NID_0));
}

bool pucch_pdu_validator_impl::is_valid(const pucch_processor::format1_configuration& config) const
{
  // The configuration structure is not properly initialized.
  if ((config.bwp_size_rb == 0) || (config.nof_symbols == 0) || config.ports.empty()) {
    return false;
  }

  // Extended CP is only for 60 kHz subcarrier spacing, see TS38.211 Section 4.2.
  if ((config.cp == cyclic_prefix::EXTENDED) && (config.slot.numerology() != 2)) {
    return false;
  }

  // The BWP size exceeds the dimensions.
  if ((config.bwp_start_rb + config.bwp_size_rb) > ce_dims.nof_prb) {
    return false;
  }

  // PRB allocation goes beyond the BWP.
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

  // Number of OFDM symbols lies outside the PUCCH Format 1 range.
  if ((config.nof_symbols < PUCCH_FORMAT1_MIN_NSYMB) || (config.nof_symbols > PUCCH_FORMAT1_MAX_NSYMB)) {
    return false;
  }

  // Time domain OCC index exceeds its maximum value.
  if (config.time_domain_occ > FORMAT1_MAX_OCC_IDX) {
    return false;
  }

  // Initial cyclic shift exceeds its maximum value.
  if (config.initial_cyclic_shift > FORMAT1_MAX_CYCLIC_SHIFT) {
    return false;
  }

  // NID exceeds its maximum value.
  if (config.n_id > FORMAT1_MAX_NID) {
    return false;
  }

  // The number of receive ports exceeds the configured maximum number of receive ports.
  if (config.ports.size() > ce_dims.nof_rx_ports) {
    return false;
  }

  // The number of HARQ-ACK exceeds the maximum HARQ-ACK carried by PUCCH Format 1.
  if (config.nof_harq_ack > PUCCH_FORMAT1_MAX_HARQ_ACK) {
    return false;
  }

  return true;
}

bool pucch_pdu_validator_impl::is_valid(const pucch_processor::format2_configuration& config) const
{
  unsigned nof_uci_bits = config.nof_harq_ack + config.nof_sr + config.nof_csi_part1 + config.nof_csi_part2;

  // The configuration structure is not properly initialized.
  if ((config.bwp_size_rb == 0) || (config.nof_prb == 0) || (config.nof_symbols == 0) || (nof_uci_bits == 0) ||
      (config.rnti == 0) || config.ports.empty()) {
    return false;
  }

  // Extended CP is only for 60 kHz subcarrier spacing, see TS38.211 Section 4.2.
  if ((config.cp == cyclic_prefix::EXTENDED) && (config.slot.numerology() != 2)) {
    return false;
  }

  // The BWP size exceeds the dimensions.
  if ((config.bwp_start_rb + config.bwp_size_rb) > ce_dims.nof_prb) {
    return false;
  }

  // The number of PRB allocated to PUCCH Format 2 exceeds the maximum allocation dimensions.
  if (config.nof_prb > PUCCH_FORMAT2_MAX_NPRB) {
    return false;
  }

  // None of the occupied PRB within the BWP can exceed the BWP dimensions.
  if (config.starting_prb + config.nof_prb > config.bwp_size_rb) {
    return false;
  }

  // The number of OFDM symbols allocated to PUCCH Format 2 exceeds the maximum allocation dimensions.
  if (config.nof_symbols > PUCCH_FORMAT2_MAX_NSYMB) {
    return false;
  }

  // None of the occupied symbols can exceed the configured maximum slot size, or the slot size given by the CP.
  if ((config.start_symbol_index + config.nof_symbols > get_nsymb_per_slot(config.cp)) ||
      (config.start_symbol_index + config.nof_symbols > ce_dims.nof_symbols)) {
    return false;
  }

  // The number of receive ports exceeds the configured maximum number of receive ports.
  if (config.ports.size() > ce_dims.nof_rx_ports) {
    return false;
  }

  // CSI is not supported.
  if ((config.nof_csi_part1 != 0) || (config.nof_csi_part2 != 0)) {
    return false;
  }

  // Intra-slot frequency hopping is not supported.
  if (config.second_hop_prb.has_value()) {
    return false;
  }

  // UCI payload exceeds the UCI payload size boundaries.
  if (nof_uci_bits < PUCCH_FORMAT2_MIN_UCI_NBITS || nof_uci_bits > FORMAT2_MAX_UCI_NBITS) {
    return false;
  }

  // NID exceeds its maximum value.
  if (config.n_id > FORMAT2_MAX_NID) {
    return false;
  }

  // NID_0 exceeds its maximum value.
  if (config.n_id_0 > FORMAT2_MAX_NID_0) {
    return false;
  }

  return true;
}
