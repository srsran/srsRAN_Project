/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pusch_processor_impl.h"
#include "srsgnb/ran/pusch/ulsch_info.h"
#include "srsgnb/ran/sch_dmrs_power.h"

using namespace srsgnb;

pusch_processor_impl::pusch_processor_impl(pusch_processor_configuration& config) :
  estimator(std::move(config.estimator)),
  demodulator(std::move(config.demodulator)),
  demultiplex(std::move(config.demultiplex)),
  decoder(std::move(config.decoder)),
  uci_dec(std::move(config.uci_dec)),
  ch_estimate(config.ce_dims)
{
  srsgnb_assert(estimator, "Invalid estimator.");
  srsgnb_assert(demodulator, "Invalid demodulator.");
  srsgnb_assert(demultiplex, "Invalid demultiplex.");
  srsgnb_assert(decoder, "Invalid decoder.");
  srsgnb_assert(uci_dec, "Invalid UCI decoder.");
}

pusch_processor_result pusch_processor_impl::process(span<uint8_t>                 data,
                                                     rx_softbuffer&                softbuffer,
                                                     const resource_grid_reader&   grid,
                                                     const pusch_processor::pdu_t& pdu)
{
  pusch_processor_result result;

  // Number of RB used by this transmission.
  unsigned nof_rb = pdu.freq_alloc.get_nof_rb();

  // Calculate the total number of DM-RS per PRB.
  unsigned nof_dmrs_per_prb = pdu.dmrs.nof_dmrs_per_rb() * pdu.nof_cdm_groups_without_data *
                              std::count(pdu.dmrs_symbol_mask.begin(), pdu.dmrs_symbol_mask.end(), true);

  // Calculate the number of data RE per PRB.
  unsigned nof_re_per_prb = NRE * pdu.nof_symbols - nof_dmrs_per_prb;

  // Calculate the number of PUSCH symbols.
  unsigned nof_pusch_re = nof_rb * nof_re_per_prb;

  // Calculate number of LLR.
  unsigned nof_codeword_llr = nof_pusch_re * get_bits_per_symbol(pdu.mcs_descr.modulation) * pdu.nof_tx_layers;

  // Get RB mask relative to Point A. It assumes PUSCH is never interleaved.
  bounded_bitset<MAX_RB> rb_mask = pdu.freq_alloc.get_prb_mask(pdu.bwp_start_rb, pdu.bwp_size_rb);

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

  // Fill result channel state information.
  result.csi = ch_estimate.get_channel_state_information();

  // Get UL-SCH information.
  ulsch_configuration ulsch_config;
  ulsch_config.tbs                   = data.size() * 8;
  ulsch_config.mcs_descr             = pdu.mcs_descr;
  ulsch_config.nof_harq_ack_bits     = pdu.uci.nof_harq_ack;
  ulsch_config.nof_csi_part1_bits    = pdu.uci.nof_csi_part1;
  ulsch_config.nof_csi_part2_bits    = pdu.uci.nof_csi_part2;
  ulsch_config.alpha_scaling         = pdu.uci.alpha_scaling;
  ulsch_config.beta_offset_harq_ack  = pdu.uci.beta_offset_harq_ack;
  ulsch_config.beta_offset_csi_part1 = pdu.uci.beta_offset_csi_part1;
  ulsch_config.beta_offset_csi_part2 = pdu.uci.beta_offset_csi_part2;
  ulsch_config.nof_rb                = nof_rb;
  ulsch_config.start_symbol_index    = pdu.start_symbol_index;
  ulsch_config.nof_symbols           = pdu.nof_symbols;
  ulsch_config.dmrs_type             = pdu.dmrs == dmrs_type::TYPE1 ? dmrs_config_type::type1 : dmrs_config_type::type2;
  ulsch_config.dmrs_symbol_mask =
      bounded_bitset<MAX_NSYMB_PER_SLOT>(pdu.dmrs_symbol_mask.begin(), pdu.dmrs_symbol_mask.end());
  ulsch_config.nof_cdm_groups_without_data = pdu.nof_cdm_groups_without_data;
  ulsch_config.nof_layers                  = pdu.nof_tx_layers;
  ulsch_information info                   = get_ulsch_information(ulsch_config);

  // Prepare demultiplex message information.
  ulsch_demultiplex::message_information demux_msg_info;
  demux_msg_info.nof_harq_ack_bits      = pdu.uci.nof_harq_ack;
  demux_msg_info.nof_enc_harq_ack_bits  = info.nof_harq_ack_bits;
  demux_msg_info.nof_csi_part1_bits     = pdu.uci.nof_csi_part1;
  demux_msg_info.nof_enc_csi_part1_bits = info.nof_csi_part1_bits;
  demux_msg_info.nof_csi_part2_bits     = pdu.uci.nof_csi_part2;
  demux_msg_info.nof_enc_csi_part2_bits = info.nof_csi_part2_bits;

  // Prepare demultiplex configuration.
  ulsch_demultiplex::configuration demux_config;
  demux_config.modulation                  = pdu.mcs_descr.modulation;
  demux_config.nof_layers                  = pdu.nof_tx_layers;
  demux_config.nof_prb                     = nof_rb;
  demux_config.start_symbol_index          = pdu.start_symbol_index;
  demux_config.nof_symbols                 = pdu.nof_symbols;
  demux_config.nof_harq_ack_rvd            = info.nof_harq_ack_rvd;
  demux_config.dmrs                        = pdu.dmrs;
  demux_config.dmrs_symbol_mask            = ulsch_config.dmrs_symbol_mask;
  demux_config.nof_cdm_groups_without_data = ulsch_config.nof_cdm_groups_without_data;

  // Demodulate.
  pusch_demodulator::configuration demod_config;
  demod_config.rnti                        = pdu.rnti;
  demod_config.rb_mask                     = rb_mask;
  demod_config.modulation                  = pdu.mcs_descr.modulation;
  demod_config.start_symbol_index          = pdu.start_symbol_index;
  demod_config.nof_symbols                 = pdu.nof_symbols;
  demod_config.dmrs_symb_pos               = pdu.dmrs_symbol_mask;
  demod_config.dmrs_config_type            = pdu.dmrs;
  demod_config.nof_cdm_groups_without_data = pdu.nof_cdm_groups_without_data;
  demod_config.n_id                        = pdu.n_id;
  demod_config.nof_tx_layers               = pdu.nof_tx_layers;
  demod_config.rx_ports                    = pdu.rx_ports;
  demod_config.placeholders                = demultiplex->get_placeholders(demux_msg_info, demux_config);
  span<log_likelihood_ratio> codeword_llr  = span<log_likelihood_ratio>(temp_codeword_llr).first(nof_codeword_llr);
  demodulator->demodulate(codeword_llr, grid, ch_estimate, demod_config);

  // Prepare buffers.
  span<log_likelihood_ratio> sch_llr      = span<log_likelihood_ratio>(temp_sch_llr).first(info.nof_ul_sch_bits);
  span<log_likelihood_ratio> harq_ack_llr = span<log_likelihood_ratio>(temp_harq_ack_llr).first(info.nof_harq_ack_bits);
  span<log_likelihood_ratio> csi_part1_llr =
      span<log_likelihood_ratio>(temp_csi_part1_llr).first(info.nof_csi_part1_bits);
  span<log_likelihood_ratio> csi_part2_llr =
      span<log_likelihood_ratio>(temp_csi_part2_llr).first(info.nof_csi_part2_bits);

  // Demultiplex UL-SCH if any of UCI field is present.
  if ((pdu.uci.nof_harq_ack > 0) || (pdu.uci.nof_csi_part1 > 0) || (pdu.uci.nof_csi_part2 > 0)) {
    // Demultiplexes UL-SCH codeword.
    demultiplex->demultiplex(sch_llr, harq_ack_llr, csi_part1_llr, csi_part2_llr, codeword_llr, demux_config);
  } else {
    // Overwrite the view of the codeword.
    sch_llr = codeword_llr;
  }

  // Prepare UCI decoder configuration.
  uci_decoder::configuration uci_dec_config;
  uci_dec_config.modulation = pdu.mcs_descr.modulation;

  // Decode HARQ-ACK.
  result.harq_ack = decode_uci_field(harq_ack_llr, pdu.uci.nof_harq_ack, uci_dec_config);

  // Decode CSI-Part1.
  result.csi_part1 = decode_uci_field(csi_part1_llr, pdu.uci.nof_csi_part1, uci_dec_config);

  // Decode HARQ-ACK.
  result.csi_part2 = decode_uci_field(csi_part2_llr, pdu.uci.nof_csi_part2, uci_dec_config);

  // Decode codeword if present.
  if (pdu.codeword) {
    // Set the data field to present.
    result.data.emplace();

    // Prepare decoder configuration.
    pusch_decoder::configuration decoder_config;
    decoder_config.segmenter_cfg.base_graph     = pdu.codeword.value().ldpc_base_graph;
    decoder_config.segmenter_cfg.rv             = pdu.codeword.value().rv;
    decoder_config.segmenter_cfg.mod            = pdu.mcs_descr.modulation;
    decoder_config.segmenter_cfg.Nref           = pdu.tbs_lbrm_bytes * 8;
    decoder_config.segmenter_cfg.nof_layers     = pdu.nof_tx_layers;
    decoder_config.segmenter_cfg.nof_ch_symbols = info.nof_ul_sch_bits / get_bits_per_symbol(pdu.mcs_descr.modulation);
    decoder_config.nof_ldpc_iterations          = 10;
    decoder_config.use_early_stop               = true;
    decoder_config.new_data                     = pdu.codeword.value().new_data;

    // Decode.
    decoder->decode(data, result.data.value(), &softbuffer, sch_llr, decoder_config);
  }

  return result;
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
