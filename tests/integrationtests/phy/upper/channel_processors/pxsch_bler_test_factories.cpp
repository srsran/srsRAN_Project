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

#include "pxsch_bler_test_factories.h"

using namespace srsran;

std::shared_ptr<pdsch_processor_factory> srsran::create_sw_pdsch_processor_factory(task_executor& executor,
                                                                                   unsigned       max_nof_threads)
{
  std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
  report_fatal_error_if_not(crc_calc_factory, "Failed to create CRC calculator factory.");

  std::shared_ptr<ldpc_encoder_factory> ldpc_encoder_factory = create_ldpc_encoder_factory_sw("auto");
  report_fatal_error_if_not(ldpc_encoder_factory, "Failed to create factory.");

  std::shared_ptr<ldpc_rate_matcher_factory> ldpc_rate_matcher_factory = create_ldpc_rate_matcher_factory_sw();
  report_fatal_error_if_not(ldpc_rate_matcher_factory, "Failed to create factory.");

  std::shared_ptr<ldpc_segmenter_tx_factory> segmenter_factory = create_ldpc_segmenter_tx_factory_sw(crc_calc_factory);
  report_fatal_error_if_not(segmenter_factory, "Failed to create factory.");

  std::shared_ptr<pseudo_random_generator_factory> pseudo_random_gen_factory =
      create_pseudo_random_generator_sw_factory();
  report_fatal_error_if_not(pseudo_random_gen_factory, "Failed to create factory.");

  std::shared_ptr<channel_modulation_factory> channel_mod_factory = create_channel_modulation_sw_factory();
  report_fatal_error_if_not(channel_mod_factory, "Failed to create factory.");

  std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_pdsch_proc_factory =
      create_dmrs_pdsch_processor_factory_sw(pseudo_random_gen_factory);
  report_fatal_error_if_not(dmrs_pdsch_proc_factory, "Failed to create factory.");

  return create_pdsch_concurrent_processor_factory_sw(crc_calc_factory,
                                                      ldpc_encoder_factory,
                                                      ldpc_rate_matcher_factory,
                                                      pseudo_random_gen_factory,
                                                      channel_mod_factory,
                                                      dmrs_pdsch_proc_factory,
                                                      executor,
                                                      max_nof_threads);
}

std::shared_ptr<pusch_processor_factory> srsran::create_sw_pusch_processor_factory(task_executor& executor,
                                                                                   unsigned       max_nof_threads,
                                                                                   unsigned       nof_ldpc_iterations,
                                                                                   bool           dec_enable_early_stop)
{
  std::shared_ptr<dft_processor_factory> dft_proc_factory = create_dft_processor_factory_fftw_slow();
  report_fatal_error_if_not(dft_proc_factory, "Failed to create factory.");

  std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
  report_fatal_error_if_not(crc_calc_factory, "Failed to create factory.");

  std::shared_ptr<ldpc_decoder_factory> ldpc_decoder_factory = create_ldpc_decoder_factory_sw("auto");
  report_fatal_error_if_not(ldpc_decoder_factory, "Failed to create factory.");

  std::shared_ptr<ldpc_rate_dematcher_factory> ldpc_rate_dematcher_factory =
      create_ldpc_rate_dematcher_factory_sw("auto");
  report_fatal_error_if_not(ldpc_rate_dematcher_factory, "Failed to create factory.");

  std::shared_ptr<ldpc_segmenter_rx_factory> segmenter_rx_factory = create_ldpc_segmenter_rx_factory_sw();
  report_fatal_error_if_not(segmenter_rx_factory, "Failed to create factory.");

  pusch_decoder_factory_sw_configuration pusch_decoder_factory_sw_config;
  pusch_decoder_factory_sw_config.crc_factory               = crc_calc_factory;
  pusch_decoder_factory_sw_config.decoder_factory           = ldpc_decoder_factory;
  pusch_decoder_factory_sw_config.dematcher_factory         = ldpc_rate_dematcher_factory;
  pusch_decoder_factory_sw_config.segmenter_factory         = segmenter_rx_factory;
  pusch_decoder_factory_sw_config.nof_pusch_decoder_threads = max_nof_threads;
  pusch_decoder_factory_sw_config.executor                  = &executor;
  pusch_decoder_factory_sw_config.nof_prb                   = MAX_RB;
  pusch_decoder_factory_sw_config.nof_layers                = pusch_constants::MAX_NOF_LAYERS;
  std::shared_ptr<pusch_decoder_factory> pusch_dec_factory =
      create_pusch_decoder_factory_sw(pusch_decoder_factory_sw_config);
  report_fatal_error_if_not(pusch_dec_factory, "Failed to create factory.");

  std::shared_ptr<pseudo_random_generator_factory> pseudo_random_gen_factory =
      create_pseudo_random_generator_sw_factory();
  report_fatal_error_if_not(pseudo_random_gen_factory, "Failed to create factory.");

  std::shared_ptr<time_alignment_estimator_factory> ta_est_factory =
      create_time_alignment_estimator_dft_factory(dft_proc_factory);
  report_fatal_error_if_not(ta_est_factory, "Failed to create factory.");

  std::shared_ptr<port_channel_estimator_factory> chan_estimator_factory =
      create_port_channel_estimator_factory_sw(ta_est_factory);
  report_fatal_error_if_not(chan_estimator_factory, "Failed to create factory.");

  std::shared_ptr<dmrs_pusch_estimator_factory> chan_est_factory =
      create_dmrs_pusch_estimator_factory_sw(pseudo_random_gen_factory, chan_estimator_factory);
  report_fatal_error_if_not(chan_est_factory, "Failed to create factory.");

  std::shared_ptr<channel_equalizer_factory> eq_factory =
      create_channel_equalizer_generic_factory(channel_equalizer_algorithm_type::zf);
  report_fatal_error_if_not(eq_factory, "Failed to create factory.");

  std::shared_ptr<channel_modulation_factory> chan_mod_factory = create_channel_modulation_sw_factory();
  report_fatal_error_if_not(chan_mod_factory, "Failed to create factory.");

  std::shared_ptr<transform_precoder_factory> precoding_factory =
      create_dft_transform_precoder_factory(dft_proc_factory, MAX_RB);
  report_fatal_error_if_not(precoding_factory, "Invalid transform precoding factory.");

  std::shared_ptr<pusch_demodulator_factory> pusch_demod_factory = create_pusch_demodulator_factory_sw(
      eq_factory, precoding_factory, chan_mod_factory, pseudo_random_gen_factory, MAX_RB, true, true);
  report_fatal_error_if_not(pusch_demod_factory, "Failed to create factory.");

  std::shared_ptr<ulsch_demultiplex_factory> demux_factory = create_ulsch_demultiplex_factory_sw();
  report_fatal_error_if_not(demux_factory, "Failed to create factory.");

  std::shared_ptr<short_block_detector_factory> short_block_det_factory = create_short_block_detector_factory_sw();
  report_fatal_error_if_not(short_block_det_factory, "Failed to create factory.");

  std::shared_ptr<polar_factory> polar_dec_factory = create_polar_factory_sw();
  report_fatal_error_if_not(polar_dec_factory, "Failed to create factory.");

  std::shared_ptr<uci_decoder_factory> uci_dec_factory =
      create_uci_decoder_factory_generic(short_block_det_factory, polar_dec_factory, crc_calc_factory);
  report_fatal_error_if_not(uci_dec_factory, "Failed to create factory.");

  pusch_processor_factory_sw_configuration pusch_proc_factory_config;
  pusch_proc_factory_config.estimator_factory      = chan_est_factory;
  pusch_proc_factory_config.demodulator_factory    = pusch_demod_factory;
  pusch_proc_factory_config.demux_factory          = demux_factory;
  pusch_proc_factory_config.decoder_factory        = pusch_dec_factory;
  pusch_proc_factory_config.uci_dec_factory        = uci_dec_factory;
  pusch_proc_factory_config.ch_estimate_dimensions = {
      MAX_NOF_PRBS, MAX_NSYMB_PER_SLOT, pusch_constants::MAX_NOF_RX_PORTS, pusch_constants::MAX_NOF_LAYERS};
  pusch_proc_factory_config.dec_nof_iterations         = nof_ldpc_iterations;
  pusch_proc_factory_config.dec_enable_early_stop      = dec_enable_early_stop;
  pusch_proc_factory_config.max_nof_concurrent_threads = max_nof_threads;
  pusch_proc_factory_config.csi_sinr_calc_method       = channel_state_information::sinr_type::post_equalization;

  std::shared_ptr<pusch_processor_factory> pusch_proc_factory =
      create_pusch_processor_factory_sw(pusch_proc_factory_config);
  report_fatal_error_if_not(pusch_proc_factory, "Failed to create factory.");

  return pusch_proc_factory;
}