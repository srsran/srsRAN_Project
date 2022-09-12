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
#include "srsgnb/ran/sch_dmrs_power.h"

using namespace srsgnb;

pusch_processor_impl::pusch_processor_impl(pusch_processor_configuration& config) :
  estimator(std::move(config.estimator)),
  demodulator(std::move(config.demodulator)),
  decoder(std::move(config.decoder)),
  ch_estimate(config.ce_dims)
{
  srsgnb_assert(estimator, "Invalid estimator.");
  srsgnb_assert(demodulator, "Invalid estimator.");
  srsgnb_assert(decoder, "Invalid estimator.");
}

pusch_processor_result pusch_processor_impl::process(span<uint8_t>                 data,
                                                     rx_softbuffer&                softbuffer,
                                                     const resource_grid_reader&   grid,
                                                     const pusch_processor::pdu_t& pdu)
{
  pusch_processor_result result;

  // Calculate the total number of DM-RS per PRB.
  unsigned nof_dmrs_per_prb = pdu.dmrs.nof_dmrs_per_rb() * pdu.nof_cdm_groups_without_data *
                              std::count(pdu.dmrs_symbol_mask.begin(), pdu.dmrs_symbol_mask.end(), true);

  // Calculate the number of data RE per PRB.
  unsigned nof_re_per_prb = NRE * pdu.nof_symbols - nof_dmrs_per_prb;

  // Calculate the number of PUSCH symbols.
  unsigned nof_pusch_symbols = pdu.freq_alloc.get_nof_rb() * nof_re_per_prb * pdu.nof_tx_layers;

  // Calculate number of LLR.
  unsigned nof_codeword_llr  = nof_pusch_symbols * get_bits_per_symbol(pdu.modulation);
  unsigned nof_harq_ack_llr  = 0;
  unsigned nof_csi_part1_llr = 0;
  unsigned nof_csi_part2_llr = 0;

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

  // Prepare LLR buffers.
  span<log_likelihood_ratio> codeword_llr  = span<log_likelihood_ratio>(temp_codeword_llr).first(nof_codeword_llr);
  span<log_likelihood_ratio> harq_ack_llr  = span<log_likelihood_ratio>(temp_harq_ack_llr).first(nof_harq_ack_llr);
  span<log_likelihood_ratio> csi_part1_llr = span<log_likelihood_ratio>(temp_csi_part1_llr).first(nof_csi_part1_llr);
  span<log_likelihood_ratio> csi_part2_llr = span<log_likelihood_ratio>(temp_csi_part2_llr).first(nof_csi_part2_llr);

  // Demodulate.
  pusch_demodulator::configuration demod_config;
  demod_config.rnti                        = pdu.rnti;
  demod_config.rb_mask                     = rb_mask;
  demod_config.modulation                  = pdu.modulation;
  demod_config.start_symbol_index          = pdu.start_symbol_index;
  demod_config.nof_symbols                 = pdu.nof_symbols;
  demod_config.dmrs_symb_pos               = pdu.dmrs_symbol_mask;
  demod_config.dmrs_config_type            = pdu.dmrs;
  demod_config.nof_cdm_groups_without_data = pdu.nof_cdm_groups_without_data;
  demod_config.n_id                        = pdu.n_id;
  demod_config.nof_tx_layers               = pdu.nof_tx_layers;
  demod_config.rx_ports                    = pdu.rx_ports;
  demodulator->demodulate(codeword_llr, harq_ack_llr, csi_part1_llr, csi_part2_llr, grid, ch_estimate, demod_config);

  // Decode codeword if present.
  if (pdu.codeword) {
    // Set the data field to present.
    result.data.emplace();

    // Prepare decoder configuration.
    pusch_decoder::configuration decoder_config;
    decoder_config.segmenter_cfg.base_graph     = pdu.codeword.value().ldpc_base_graph;
    decoder_config.segmenter_cfg.rv             = pdu.codeword.value().rv;
    decoder_config.segmenter_cfg.mod            = pdu.modulation;
    decoder_config.segmenter_cfg.Nref           = pdu.tbs_lbrm_bytes * 8;
    decoder_config.segmenter_cfg.nof_layers     = pdu.nof_tx_layers;
    decoder_config.segmenter_cfg.nof_ch_symbols = nof_pusch_symbols;
    decoder_config.nof_ldpc_iterations          = 10;
    decoder_config.use_early_stop               = true;
    decoder_config.new_data                     = pdu.codeword.value().new_data;

    // Decode.
    decoder->decode(data, result.data.value(), &softbuffer, codeword_llr, decoder_config);
  }

  // Decode UCI if present.
  if (pdu.uci) {
    // TBD.
  }

  return result;
}
