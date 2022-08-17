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

  // Calculate number of LLR.
  unsigned nof_codeword_llr =
      pdu.freq_alloc.get_nof_rb() * pdu.nof_symbols * pdu.ports.size() * get_bits_per_symbol(pdu.codeword->modulation);
  unsigned nof_harq_ack_llr  = 0;
  unsigned nof_csi_part1_llr = 0;
  unsigned nof_csi_part2_llr = 0;

  // Estimate channel.
  dmrs_pusch_estimator::configuration ch_est_config;
  ch_est_config.slot = pdu.slot;
  estimator->estimate(ch_estimate, grid, ch_est_config);

  // Fill result channel state information.
  result.csi = ch_estimate.get_channel_state_information();

  // Prepare LLR buffers.
  span<log_likelihood_ratio> codeword_llr  = span<log_likelihood_ratio>(temp_codeword_llr).first(nof_codeword_llr);
  span<log_likelihood_ratio> harq_ack_llr  = span<log_likelihood_ratio>(temp_harq_ack_llr).first(nof_harq_ack_llr);
  span<log_likelihood_ratio> csi_part1_llr = span<log_likelihood_ratio>(temp_csi_part1_llr).first(nof_csi_part1_llr);
  span<log_likelihood_ratio> csi_part2_llr = span<log_likelihood_ratio>(temp_csi_part2_llr).first(nof_csi_part2_llr);

  // Prepare demodulator configuration.
  pusch_demodulator::configuration demod_config;
  demod_config.rnti = pdu.rnti;

  // Demodulate.
  demodulator->demodulate(codeword_llr, harq_ack_llr, csi_part1_llr, csi_part2_llr, grid, ch_estimate, demod_config);

  // Process codeword.
  if (pdu.codeword) {
    // Set the data field to present.
    result.data.emplace();

    // Prepare decoder configuration.
    pusch_decoder::configuration decoder_config;
    decoder_config.new_data = pdu.codeword->new_data;

    // Decode.
    decoder->decode(data, result.data.value(), &softbuffer, codeword_llr, decoder_config);
  }

  // Process UCI.
  if (pdu.uci) {
    // TBD.
  }

  return result;
}
