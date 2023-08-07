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

#include "pusch_processor_impl.h"
#include "srsran/phy/upper/unique_rx_softbuffer.h"
#include "srsran/ran/pusch/ulsch_info.h"
#include "srsran/ran/sch_dmrs_power.h"

using namespace srsran;

bool pusch_processor_validator_impl::is_valid(const pusch_processor::pdu_t& pdu) const
{
  unsigned nof_symbols_slot = get_nsymb_per_slot(pdu.cp);

  // The BWP size exceeds the grid size.
  if ((pdu.bwp_start_rb + pdu.bwp_size_rb) > ce_dims.nof_prb) {
    return false;
  }

  // The implementation only works with a single transmit layer.
  if (pdu.nof_tx_layers > ce_dims.nof_tx_layers) {
    return false;
  }

  // The number of receive ports cannot exceed the maximum dimensions.
  if (pdu.rx_ports.size() > ce_dims.nof_rx_ports) {
    return false;
  }

  // The frequency allocation is not compatible with the BWP parameters.
  if (!pdu.freq_alloc.is_bwp_valid(pdu.bwp_start_rb, pdu.bwp_size_rb)) {
    return false;
  }

  // Currently, none of the UCI field sizes can exceed 11 bit.
  static constexpr unsigned max_uci_len = 11;
  if ((pdu.uci.nof_harq_ack > max_uci_len) || (pdu.uci.nof_csi_part1 > max_uci_len)) {
    return false;
  }

  // CSI Part 2 multiplexing is not supported.
  if (pdu.uci.nof_csi_part2 != 0) {
    return false;
  }

  // The number of symbols carrying DM-RS must be greater than zero.
  if (pdu.dmrs_symbol_mask.size() != nof_symbols_slot) {
    return false;
  }

  // The number of symbols carrying DM-RS must be greater than zero.
  if (pdu.dmrs_symbol_mask.none()) {
    return false;
  }

  // The index of the first OFDM symbol carrying DM-RS shall be equal to or greater than the first symbol allocated to
  // transmission.
  int first_dmrs_symbol_index = pdu.dmrs_symbol_mask.find_lowest(true);
  if (static_cast<unsigned>(first_dmrs_symbol_index) < pdu.start_symbol_index) {
    return false;
  }

  // The index of the last OFDM symbol carrying DM-RS shall not be larger than the last symbol allocated to
  // transmission.
  int last_dmrs_symbol_index = pdu.dmrs_symbol_mask.find_highest(true);
  if (static_cast<unsigned>(last_dmrs_symbol_index) >= (pdu.start_symbol_index + pdu.nof_symbols)) {
    return false;
  }

  // None of the occupied symbols must exceed the slot size.
  if (nof_symbols_slot < (pdu.start_symbol_index + pdu.nof_symbols)) {
    return false;
  }

  // Only DM-RS Type 1 is supported.
  if (pdu.dmrs != dmrs_type::TYPE1) {
    return false;
  }

  // Only two CDM groups without data is supported.
  if (pdu.nof_cdm_groups_without_data != 2) {
    return false;
  }

  return true;
}

pusch_processor_impl::pusch_processor_impl(pusch_processor_configuration& config) :
  estimator(std::move(config.estimator)),
  demodulator(std::move(config.demodulator)),
  demultiplex(std::move(config.demultiplex)),
  decoder(std::move(config.decoder)),
  uci_dec(std::move(config.uci_dec)),
  ch_estimate(config.ce_dims),
  dec_nof_iterations(config.dec_nof_iterations),
  dec_enable_early_stop(config.dec_enable_early_stop)
{
  srsran_assert(estimator, "Invalid estimator.");
  srsran_assert(demodulator, "Invalid demodulator.");
  srsran_assert(demultiplex, "Invalid demultiplex.");
  srsran_assert(decoder, "Invalid decoder.");
  srsran_assert(uci_dec, "Invalid UCI decoder.");
  srsran_assert(dec_nof_iterations != 0, "The decoder number of iterations must be non-zero.");
}

void pusch_processor_impl::process(span<uint8_t>                    data,
                                   rx_softbuffer&                   softbuffer,
                                   pusch_processor_result_notifier& notifier,
                                   const resource_grid_reader&      grid,
                                   const pusch_processor::pdu_t&    pdu)
{
  assert_pdu(pdu);

  // Number of RB used by this transmission.
  unsigned nof_rb = pdu.freq_alloc.get_nof_rb();

  // Calculate the total number of DM-RS per PRB.
  unsigned nof_dmrs_per_prb =
      pdu.dmrs.nof_dmrs_per_rb() * pdu.nof_cdm_groups_without_data * pdu.dmrs_symbol_mask.count();

  // Calculate the number of data RE per PRB.
  unsigned nof_re_per_prb = NRE * pdu.nof_symbols - nof_dmrs_per_prb;

  // Calculate the number of PUSCH symbols.
  unsigned nof_pusch_re = nof_rb * nof_re_per_prb;

  // Calculate number of LLR.
  unsigned nof_codeword_llr = nof_pusch_re * get_bits_per_symbol(pdu.mcs_descr.modulation) * pdu.nof_tx_layers;

  // Get RB mask relative to Point A. It assumes PUSCH is never interleaved.
  bounded_bitset<MAX_RB> rb_mask = pdu.freq_alloc.get_prb_mask(pdu.bwp_start_rb, pdu.bwp_size_rb);

  // Get UL-SCH information.
  ulsch_configuration ulsch_config;
  ulsch_config.tbs                   = units::bytes(data.size()).to_bits();
  ulsch_config.mcs_descr             = pdu.mcs_descr;
  ulsch_config.nof_harq_ack_bits     = units::bits(pdu.uci.nof_harq_ack);
  ulsch_config.nof_csi_part1_bits    = units::bits(pdu.uci.nof_csi_part1);
  ulsch_config.nof_csi_part2_bits    = units::bits(pdu.uci.nof_csi_part2);
  ulsch_config.alpha_scaling         = pdu.uci.alpha_scaling;
  ulsch_config.beta_offset_harq_ack  = pdu.uci.beta_offset_harq_ack;
  ulsch_config.beta_offset_csi_part1 = pdu.uci.beta_offset_csi_part1;
  ulsch_config.beta_offset_csi_part2 = pdu.uci.beta_offset_csi_part2;
  ulsch_config.nof_rb                = nof_rb;
  ulsch_config.start_symbol_index    = pdu.start_symbol_index;
  ulsch_config.nof_symbols           = pdu.nof_symbols;
  ulsch_config.dmrs_type             = pdu.dmrs == dmrs_type::TYPE1 ? dmrs_config_type::type1 : dmrs_config_type::type2;
  ulsch_config.dmrs_symbol_mask      = pdu.dmrs_symbol_mask;
  ulsch_config.nof_cdm_groups_without_data = pdu.nof_cdm_groups_without_data;
  ulsch_config.nof_layers                  = pdu.nof_tx_layers;
  ulsch_information info                   = get_ulsch_information(ulsch_config);

  // Estimate channel.
  dmrs_pusch_estimator::configuration ch_est_config;
  ch_est_config.slot          = pdu.slot;
  ch_est_config.type          = pdu.dmrs;
  ch_est_config.scrambling_id = pdu.scrambling_id;
  ch_est_config.n_scid        = pdu.n_scid;
  ch_est_config.scaling       = convert_dB_to_amplitude(-get_sch_to_dmrs_ratio_dB(pdu.nof_cdm_groups_without_data));
  ch_est_config.c_prefix      = pdu.cp;
  ch_est_config.symbols_mask  = pdu.dmrs_symbol_mask;
  ch_est_config.rb_mask       = rb_mask;
  ch_est_config.first_symbol  = pdu.start_symbol_index;
  ch_est_config.nof_symbols   = pdu.nof_symbols;
  ch_est_config.nof_tx_layers = pdu.nof_tx_layers;
  ch_est_config.rx_ports.assign(pdu.rx_ports.begin(), pdu.rx_ports.end());
  estimator->estimate(ch_estimate, grid, ch_est_config);

  // Prepare demultiplex message information.
  ulsch_demultiplex::message_information demux_msg_info;
  demux_msg_info.nof_harq_ack_bits      = pdu.uci.nof_harq_ack;
  demux_msg_info.nof_enc_harq_ack_bits  = info.nof_harq_ack_bits.value();
  demux_msg_info.nof_csi_part1_bits     = pdu.uci.nof_csi_part1;
  demux_msg_info.nof_enc_csi_part1_bits = info.nof_csi_part1_bits.value();
  demux_msg_info.nof_csi_part2_bits     = pdu.uci.nof_csi_part2;
  demux_msg_info.nof_enc_csi_part2_bits = info.nof_csi_part2_bits.value();

  // Prepare demultiplex configuration.
  ulsch_demultiplex::configuration demux_config;
  demux_config.modulation                  = pdu.mcs_descr.modulation;
  demux_config.nof_layers                  = pdu.nof_tx_layers;
  demux_config.nof_prb                     = nof_rb;
  demux_config.start_symbol_index          = pdu.start_symbol_index;
  demux_config.nof_symbols                 = pdu.nof_symbols;
  demux_config.nof_harq_ack_rvd            = info.nof_harq_ack_rvd.value();
  demux_config.dmrs                        = pdu.dmrs;
  demux_config.dmrs_symbol_mask            = ulsch_config.dmrs_symbol_mask;
  demux_config.nof_cdm_groups_without_data = ulsch_config.nof_cdm_groups_without_data;

  // Convert DM-RS symbol mask to array.
  std::array<bool, MAX_NSYMB_PER_SLOT> dmrs_symbol_mask = {};
  pdu.dmrs_symbol_mask.for_each(
      0, pdu.dmrs_symbol_mask.size(), [&dmrs_symbol_mask](unsigned i_symb) { dmrs_symbol_mask[i_symb] = true; });

  // Demodulate.
  pusch_demodulator::configuration demod_config;
  demod_config.rnti                        = pdu.rnti;
  demod_config.rb_mask                     = rb_mask;
  demod_config.modulation                  = pdu.mcs_descr.modulation;
  demod_config.start_symbol_index          = pdu.start_symbol_index;
  demod_config.nof_symbols                 = pdu.nof_symbols;
  demod_config.dmrs_symb_pos               = dmrs_symbol_mask;
  demod_config.dmrs_config_type            = pdu.dmrs;
  demod_config.nof_cdm_groups_without_data = pdu.nof_cdm_groups_without_data;
  demod_config.n_id                        = pdu.n_id;
  demod_config.nof_tx_layers               = pdu.nof_tx_layers;
  demod_config.rx_ports                    = pdu.rx_ports;
  demod_config.placeholders                = demultiplex->get_placeholders(demux_msg_info, demux_config);
  span<log_likelihood_ratio> codeword_llr  = span<log_likelihood_ratio>(temp_codeword_llr).first(nof_codeword_llr);
  pusch_demodulator::demodulation_status demod_status =
      demodulator->demodulate(codeword_llr, grid, ch_estimate, demod_config);

  // Process channel state information.
  {
    // Extract channel state information.
    channel_state_information csi = ch_estimate.get_channel_state_information();

    // Current SINR estimation is not accurate enough for the purpose of adaptive MCS.
    // Temporarily use an EVM-to-SINR conversion function.
    if (demod_status.evm.has_value()) {
      csi.sinr_dB = -20 * log10f(demod_status.evm.value()) - 3.7;
    }

    // Notify the completion of the channel state information measurement.
    notifier.on_csi(csi);
  }

  // Prepare buffers.
  span<log_likelihood_ratio> sch_llr = span<log_likelihood_ratio>(temp_sch_llr).first(info.nof_ul_sch_bits.value());

  // Process UCI if HARQ-ACK or CSI reports are present.
  if ((pdu.uci.nof_harq_ack > 0) || (pdu.uci.nof_csi_part1 > 0)) {
    span<log_likelihood_ratio> harq_ack_llr =
        span<log_likelihood_ratio>(temp_harq_ack_llr).first(info.nof_harq_ack_bits.value());
    span<log_likelihood_ratio> csi_part1_llr =
        span<log_likelihood_ratio>(temp_csi_part1_llr).first(info.nof_csi_part1_bits.value());

    // Depending on CSI Part 2 report.
    if (pdu.uci.nof_csi_part2 > 0) {
      // Demultiplex HARQ-ACK and CSI Part 1.
      demultiplex->demultiplex_csi_part1(csi_part1_llr, codeword_llr, info.nof_harq_ack_bits.value(), demux_config);
    } else {
      // Demultiplex SCH data, HARQ-ACK and CSI Part 1.
      demultiplex->demultiplex(sch_llr, harq_ack_llr, csi_part1_llr, {}, codeword_llr, demux_config);
    }

    // Prepare UCI decoder configuration.
    uci_decoder::configuration uci_dec_config;
    uci_dec_config.modulation = pdu.mcs_descr.modulation;

    // Prepare UCI results.
    pusch_processor_result_control result_uci;
    result_uci.evm = demod_status.evm;

    // Decode HARQ-ACK.
    result_uci.harq_ack = decode_uci_field(harq_ack_llr, pdu.uci.nof_harq_ack, uci_dec_config);

    // Decode CSI Part 1.
    result_uci.csi_part1 = decode_uci_field(csi_part1_llr, pdu.uci.nof_csi_part1, uci_dec_config);

    // If CSI Part 2 is enabled.
    if (pdu.uci.nof_csi_part2 > 0) {
      // Calculate the number of CSI Part 2 payload bits.
      unsigned nof_csi_part2 = pdu.uci.nof_csi_part2;

      // Calculate the number of CSI Part 2 encoded bits.
      unsigned nof_enc_csi_part2 = info.nof_csi_part2_bits.value();

      // Prepare view of CSI Part 2 report LLRs.
      span<log_likelihood_ratio> csi_part2_llr =
          span<log_likelihood_ratio>(temp_csi_part2_llr).first(nof_enc_csi_part2);

      // Demultiplex SCH data and CSI Part 2 bits.
      demultiplex->demultiplex_sch_harq_ack_and_csi_part2(
          sch_llr, harq_ack_llr, csi_part2_llr, codeword_llr, csi_part1_llr.size(), demux_config);

      // Decode CSI Part 2.
      result_uci.csi_part2 = decode_uci_field(csi_part2_llr, nof_csi_part2, uci_dec_config);
    } else {
      // Otherwise, clear CSI Part 2 result.
      result_uci.csi_part2.status = uci_status::unknown;
      result_uci.csi_part2.payload.clear();
    }

    // Report UCI if at least one field is present.
    notifier.on_uci(result_uci);
  } else {
    // Overwrite the view of the codeword to avoid copying SCH data.
    sch_llr = codeword_llr;
  }

  // Decode codeword if present.
  if (pdu.codeword) {
    // Prepare data result.
    pusch_processor_result_data result_data;
    result_data.evm = demod_status.evm;

    // Prepare decoder configuration.
    pusch_decoder::configuration decoder_config;
    decoder_config.segmenter_cfg.base_graph = pdu.codeword.value().ldpc_base_graph;
    decoder_config.segmenter_cfg.rv         = pdu.codeword.value().rv;
    decoder_config.segmenter_cfg.mod        = pdu.mcs_descr.modulation;
    decoder_config.segmenter_cfg.Nref       = pdu.tbs_lbrm_bytes * 8;
    decoder_config.segmenter_cfg.nof_layers = pdu.nof_tx_layers;
    decoder_config.segmenter_cfg.nof_ch_symbols =
        info.nof_ul_sch_bits.value() / get_bits_per_symbol(pdu.mcs_descr.modulation);
    decoder_config.nof_ldpc_iterations = dec_nof_iterations;
    decoder_config.use_early_stop      = dec_enable_early_stop;
    decoder_config.new_data            = pdu.codeword.value().new_data;

    // Decode.
    decoder->decode(data, result_data.data, &softbuffer, sch_llr, decoder_config);

    // Notify the outcome of the data decoding.
    notifier.on_sch(result_data);
  }
}

void pusch_processor_impl::assert_pdu(const pusch_processor::pdu_t& pdu) const
{
  // Make sure the configuration is supported.
  srsran_assert((pdu.bwp_start_rb + pdu.bwp_size_rb) <= ch_estimate.size().nof_prb,
                "The sum of the BWP start (i.e., {}) and size (i.e., {}) exceeds the maximum grid size (i.e., {} PRB).",
                pdu.bwp_start_rb,
                pdu.bwp_size_rb,
                ch_estimate.size().nof_prb);
  srsran_assert(pdu.dmrs == dmrs_type::TYPE1, "Only DM-RS Type 1 is currently supported.");
  srsran_assert(pdu.nof_cdm_groups_without_data == 2, "Only two CDM groups without data are currently supported.");
  srsran_assert(
      pdu.nof_tx_layers <= ch_estimate.size().nof_tx_layers,
      "The number of transmit layers (i.e., {}) exceeds the maximum number of transmission layers (i.e., {}).",
      pdu.nof_tx_layers,
      ch_estimate.size().nof_tx_layers);
  srsran_assert(pdu.rx_ports.size() <= ch_estimate.size().nof_rx_ports,
                "The number of receive ports (i.e., {}) exceeds the maximum number of receive ports (i.e., {}).",
                pdu.rx_ports.size(),
                ch_estimate.size().nof_rx_ports);

  static constexpr unsigned max_uci_field_len = 11;
  srsran_assert(pdu.uci.nof_harq_ack <= max_uci_field_len,
                "HARQ-ACK UCI field length (i.e., {}) exceeds the maximum supported length (i.e., {})",
                pdu.uci.nof_harq_ack,
                max_uci_field_len);

  srsran_assert(pdu.uci.nof_csi_part1 <= max_uci_field_len,
                "CSI Part 1 UCI field length (i.e., {}) exceeds the maximum supported length (i.e., {})",
                pdu.uci.nof_csi_part1,
                max_uci_field_len);
}

pusch_uci_field pusch_processor_impl::decode_uci_field(span<const log_likelihood_ratio>  llr,
                                                       unsigned                          nof_bits,
                                                       const uci_decoder::configuration& uci_dec_config)
{
  pusch_uci_field result;

  // If the UCI field is not present.
  if (nof_bits == 0) {
    result.payload.clear();
    result.status = uci_status::unknown;
    return result;
  }

  // Prepare payload.
  result.payload.resize(nof_bits);

  // Actual decoding.
  result.status = uci_dec->decode(result.payload, llr, uci_dec_config);

  return result;
}
