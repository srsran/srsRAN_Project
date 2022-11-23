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

void pucch_processor_impl::assert_format2_config(const pucch_processor::format2_configuration& config)
{
  // UCI multiplexing is used on the PUCCH when UCI segments corresponding to:
  // - HARQ-ACK, SR, CSI Part 1 and
  // - CSI Part 2
  // are to be transmitted on the same PUCCH resource.
  srsgnb_assert((config.nof_csi_part2 == 0) ||
                    (config.nof_harq_ack == 0 && config.nof_sr == 0 && config.nof_csi_part1 == 0),
                "UCI multiplexing on PUCCH not supported.");

  // PUCCH Format 2 frequency hopping is not currently supported.
  srsgnb_assert(!config.second_hop_prb.has_value(), "PUCCH Format 2 frequency hopping not supported.");

  // Assert OFDM symbol allocation.
  srsgnb_assert((config.nof_symbols > 0) && (config.nof_symbols <= PUCCH_FORMAT2_MAX_NSYMB),
                "Invalid number of OFDM Symbols, i.e, {}. PUCCH Format 2 occupies 1 or 2 symbols.",
                config.nof_symbols);

  // Assert that the OFDM symbols are constrained to the slot dimensions.
  srsgnb_assert(
      config.start_symbol_index + config.nof_symbols - 1 < get_nsymb_per_slot(config.cp),
      "Allocated OFDM symbol i.e., {} (zero based indices)., exceeds number of symbols in the slot, i.e., {}.",
      config.start_symbol_index + config.nof_symbols - 1,
      get_nsymb_per_slot(config.cp));

  // Assert PRB allocation.
  srsgnb_assert((config.nof_prb > 0) && (config.nof_prb <= PUCCH_FORMAT2_MAX_NPRB),
                "Invalid number of PRB, i.e., {}. PUCCH Format 2 occupies 1 to {} PRB.",
                config.nof_prb,
                PUCCH_FORMAT2_MAX_NPRB);

  // Assert that the PRB set is constrained to the BWP.
  srsgnb_assert(config.starting_prb + config.nof_prb - 1 < config.bwp_size_rb,
                "PRB allocation within the BWP, i.e., {} to {} (zero based indices), exceeds BWP size, i.e., {}.",
                config.starting_prb,
                config.starting_prb + config.nof_prb - 1,
                config.bwp_size_rb);

  // Expected UCI payload length.
  unsigned uci_payload_len = config.nof_harq_ack + config.nof_sr + config.nof_csi_part1 + config.nof_csi_part2;

  // Maximum supported UCI payload length.
  static constexpr unsigned max_uci_len = pucch_processor_impl::PUCCH_FORMAT2_MAX_UCI_LEN;

  srsgnb_assert((uci_payload_len > 0) && (uci_payload_len <= max_uci_len),
                "UCI Payload length, i.e, {} is not supported. Payload length must be 1 to {} bits.",
                uci_payload_len,
                max_uci_len);
}
