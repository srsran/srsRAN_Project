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

#include "pxsch_bler_test_factories.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor_phy_capabilities.h"
#if defined(HWACC_PDSCH_ENABLED) && defined(HWACC_PUSCH_ENABLED)
#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"
#include "srsran/hal/dpdk/bbdev/bbdev_acc_factory.h"
#include "srsran/hal/dpdk/dpdk_eal_factory.h"
#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_factories.h"
#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc_factory.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/ext_harq_buffer_context_repository_factory.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_factories.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec_factory.h"
#endif // HWACC_PDSCH_ENABLED && HWACC_PUSCH_ENABLED

using namespace srsran;

#if defined(HWACC_PDSCH_ENABLED) && defined(HWACC_PUSCH_ENABLED)
static bool                                                   hwacc_pxsch_init_done   = false;
static std::unique_ptr<dpdk::dpdk_eal>                        dpdk_interface          = nullptr;
static std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory> hwacc_pdsch_enc_factory = nullptr;
static std::shared_ptr<hal::hw_accelerator_pusch_dec_factory> hwacc_pusch_dec_factory = nullptr;

static void create_hwacc_pxsch_factories(const std::string& eal_arguments)
{
  if (!hwacc_pxsch_init_done) {
    // Hardcoded stdout and error logging.
    srslog::sink* log_sink = srslog::create_stdout_sink();
    srslog::set_default_sink(*log_sink);
    srslog::init();
    srslog::basic_logger& logger = srslog::fetch_basic_logger("HAL", false);
    logger.set_level(srslog::basic_levels::error);

    // Pointer to a dpdk-based hardware-accelerator interface.
    if (!dpdk_interface) {
      // :TODO: Enable passing EAL arguments.
      dpdk_interface = dpdk::create_dpdk_eal(eal_arguments, logger);
      if (!dpdk_interface) {
        return;
      }
    }

    // Intefacing to the bbdev-based hardware-accelerator.
    dpdk::bbdev_acc_configuration bbdev_config;
    bbdev_config.id                                    = 0;
    bbdev_config.nof_ldpc_enc_lcores                   = dpdk::MAX_NOF_BBDEV_VF_INSTANCES;
    bbdev_config.nof_ldpc_dec_lcores                   = dpdk::MAX_NOF_BBDEV_VF_INSTANCES;
    bbdev_config.nof_fft_lcores                        = 0;
    bbdev_config.nof_mbuf                              = static_cast<unsigned>(pow2(log2_ceil(MAX_NOF_SEGMENTS)));
    std::shared_ptr<dpdk::bbdev_acc> bbdev_accelerator = create_bbdev_acc(bbdev_config, logger);
    if (!bbdev_accelerator) {
      return;
    }

    // Set the PDSCH encoder hardware-accelerator factory configuration for the ACC100.
    hal::bbdev_hwacc_pdsch_enc_factory_configuration hw_encoder_config;
    hw_encoder_config.acc_type          = "acc100";
    hw_encoder_config.bbdev_accelerator = bbdev_accelerator;
    hw_encoder_config.cb_mode           = false;
    hw_encoder_config.max_tb_size       = RTE_BBDEV_LDPC_E_MAX_MBUF;
    hw_encoder_config.dedicated_queue   = true;
    // ACC100 hardware-accelerator implementation.
    hwacc_pdsch_enc_factory = srsran::hal::create_bbdev_pdsch_enc_acc_factory(hw_encoder_config);
    if (!hwacc_pdsch_enc_factory) {
      return;
    }

    // Interfacing to a shared external HARQ buffer context repository.
    unsigned nof_cbs                   = MAX_NOF_SEGMENTS;
    uint64_t acc100_ext_harq_buff_size = bbdev_accelerator->get_harq_buff_size_bytes();
    std::shared_ptr<hal::ext_harq_buffer_context_repository> harq_buffer_context =
        hal::create_ext_harq_buffer_context_repository(nof_cbs, acc100_ext_harq_buff_size, false);
    if (!harq_buffer_context) {
      return;
    }

    // Set the PUSCH decoder hardware-accelerator factory configuration for the ACC100.
    hal::bbdev_hwacc_pusch_dec_factory_configuration hw_decoder_config;
    hw_decoder_config.acc_type            = "acc100";
    hw_decoder_config.bbdev_accelerator   = bbdev_accelerator;
    hw_decoder_config.force_local_harq    = false;
    hw_decoder_config.harq_buffer_context = harq_buffer_context;
    hw_decoder_config.dedicated_queue     = true;
    // ACC100 hardware-accelerator implementation.
    hwacc_pusch_dec_factory = hal::create_bbdev_pusch_dec_acc_factory(hw_decoder_config);
    if (!hwacc_pusch_dec_factory) {
      return;
    }

    hwacc_pxsch_init_done = true;
  }
}
#endif // HWACC_PDSCH_ENABLED && HWACC_PUSCH_ENABLED

std::shared_ptr<pdsch_processor_factory> srsran::create_sw_pdsch_processor_factory(task_executor&     executor,
                                                                                   unsigned           max_nof_threads,
                                                                                   const std::string& eal_arguments,
                                                                                   const std::string& pxsch_type)
{
  std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
  report_fatal_error_if_not(crc_calc_factory, "Failed to create CRC calculator factory.");

  std::shared_ptr<ldpc_encoder_factory> ldpc_encoder_factory = create_ldpc_encoder_factory_sw("auto");
  report_fatal_error_if_not(ldpc_encoder_factory, "Failed to create LDPC encoder factory.");

  std::shared_ptr<ldpc_rate_matcher_factory> ldpc_rate_matcher_factory = create_ldpc_rate_matcher_factory_sw();
  report_fatal_error_if_not(ldpc_rate_matcher_factory, "Failed to create LDPC RM factory.");

  std::shared_ptr<ldpc_segmenter_tx_factory> segmenter_factory = create_ldpc_segmenter_tx_factory_sw(crc_calc_factory);
  report_fatal_error_if_not(segmenter_factory, "Failed to create LDPC segmenter factory.");

  std::shared_ptr<pseudo_random_generator_factory> pseudo_random_gen_factory =
      create_pseudo_random_generator_sw_factory();
  report_fatal_error_if_not(pseudo_random_gen_factory, "Failed to create pseudo-random generator factory.");

  std::shared_ptr<modulation_mapper_factory> channel_mod_factory = create_modulation_mapper_factory();
  report_fatal_error_if_not(channel_mod_factory, "Failed to create modulation factory.");

  std::shared_ptr<channel_precoder_factory> precoding_factory = create_channel_precoder_factory("auto");
  report_fatal_error_if_not(precoding_factory, "Failed to create precoding factory.");

  std::shared_ptr<resource_grid_mapper_factory> rg_mapper_factory =
      create_resource_grid_mapper_factory(precoding_factory);
  report_fatal_error_if_not(rg_mapper_factory, "Failed to create RG mapper factory.");

  std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_pdsch_proc_factory =
      create_dmrs_pdsch_processor_factory_sw(pseudo_random_gen_factory, rg_mapper_factory);
  report_fatal_error_if_not(dmrs_pdsch_proc_factory, "Failed to create DM-RS generator factory.");

  std::shared_ptr<ptrs_pdsch_generator_factory> ptrs_pdsch_gen_factory =
      create_ptrs_pdsch_generator_generic_factory(pseudo_random_gen_factory, rg_mapper_factory);
  report_fatal_error_if_not(ptrs_pdsch_gen_factory, "Failed to create PT-RS generator factory.");

#if defined(HWACC_PDSCH_ENABLED) && defined(HWACC_PUSCH_ENABLED)
  if (pxsch_type.find("acc100") != std::string::npos) {
    // Initialize the hardware-accelerator.
    create_hwacc_pxsch_factories(eal_arguments);
    if (!hwacc_pxsch_init_done) {
      return nullptr;
    }

    // Set the hardware-accelerated PDSCH encoder configuration.
    pdsch_encoder_factory_hw_configuration encoder_hw_factory_config;
    encoder_hw_factory_config.crc_factory        = crc_calc_factory;
    encoder_hw_factory_config.segmenter_factory  = segmenter_factory;
    encoder_hw_factory_config.hw_encoder_factory = hwacc_pdsch_enc_factory;
    std::shared_ptr<pdsch_encoder_factory> pdsch_encoder_factory =
        create_pdsch_encoder_factory_hw(encoder_hw_factory_config);
    report_fatal_error_if_not(pdsch_encoder_factory, "Failed to create factory.");

    std::shared_ptr<pdsch_modulator_factory> pdsch_modulator_factory =
        create_pdsch_modulator_factory_sw(channel_mod_factory, pseudo_random_gen_factory, rg_mapper_factory);
    report_fatal_error_if_not(pdsch_modulator_factory, "Failed to create factory.");

    return create_pdsch_processor_factory_sw(
        pdsch_encoder_factory, pdsch_modulator_factory, dmrs_pdsch_proc_factory, ptrs_pdsch_gen_factory);
  }
#endif // HWACC_PDSCH_ENABLED && HWACC_PUSCH_ENABLED

  // Create PDSCH block processor factory.
  std::shared_ptr<pdsch_block_processor_factory> block_processor_factory = create_pdsch_block_processor_factory_sw(
      ldpc_encoder_factory, ldpc_rate_matcher_factory, pseudo_random_gen_factory, channel_mod_factory);
  report_fatal_error_if_not(block_processor_factory, "Failed to create factory.");

  return create_pdsch_flexible_processor_factory_sw(segmenter_factory,
                                                    block_processor_factory,
                                                    rg_mapper_factory,
                                                    dmrs_pdsch_proc_factory,
                                                    ptrs_pdsch_gen_factory,
                                                    executor,
                                                    max_nof_threads);
}

std::shared_ptr<pusch_processor_factory>
srsran::create_sw_pusch_processor_factory(task_executor&                                   executor,
                                          unsigned                                         max_nof_threads,
                                          unsigned                                         nof_ldpc_iterations,
                                          bool                                             dec_enable_early_stop,
                                          const std::string&                               pxsch_type,
                                          port_channel_estimator_td_interpolation_strategy td_interpolation_strategy,
                                          channel_equalizer_algorithm_type                 equalizer_algorithm_type)
{
  pusch_processor_phy_capabilities pusch_processor_phy_cap = get_pusch_processor_phy_capabilities();

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

  std::shared_ptr<pusch_decoder_factory> pusch_dec_factory = nullptr;
#if defined(HWACC_PDSCH_ENABLED) && defined(HWACC_PUSCH_ENABLED)
  if (pxsch_type.find("acc100") != std::string::npos) {
    // Set the hardware-accelerated PUSCH decoder configuration.
    pusch_decoder_factory_hw_configuration decoder_hw_factory_config;
    decoder_hw_factory_config.segmenter_factory  = segmenter_rx_factory;
    decoder_hw_factory_config.crc_factory        = crc_calc_factory;
    decoder_hw_factory_config.hw_decoder_factory = hwacc_pusch_dec_factory;
    decoder_hw_factory_config.executor           = &executor;
    pusch_dec_factory                            = create_pusch_decoder_factory_hw(decoder_hw_factory_config);
  } else {
#endif // HWACC_PDSCH_ENABLED && HWACC_PUSCH_ENABLED
    pusch_decoder_factory_sw_configuration pusch_decoder_factory_sw_config;
    pusch_decoder_factory_sw_config.crc_factory               = crc_calc_factory;
    pusch_decoder_factory_sw_config.decoder_factory           = ldpc_decoder_factory;
    pusch_decoder_factory_sw_config.dematcher_factory         = ldpc_rate_dematcher_factory;
    pusch_decoder_factory_sw_config.segmenter_factory         = segmenter_rx_factory;
    pusch_decoder_factory_sw_config.nof_pusch_decoder_threads = max_nof_threads;
    pusch_decoder_factory_sw_config.executor                  = &executor;
    pusch_decoder_factory_sw_config.nof_prb                   = MAX_RB;
    pusch_decoder_factory_sw_config.nof_layers                = pusch_constants::MAX_NOF_LAYERS;
    pusch_dec_factory = create_pusch_decoder_factory_sw(pusch_decoder_factory_sw_config);
#if defined(HWACC_PDSCH_ENABLED) && defined(HWACC_PUSCH_ENABLED)
  }
#endif // HWACC_PDSCH_ENABLED && HWACC_PUSCH_ENABLED
  report_fatal_error_if_not(pusch_dec_factory, "Failed to create factory.");

  std::shared_ptr<pseudo_random_generator_factory> pseudo_random_gen_factory =
      create_pseudo_random_generator_sw_factory();
  report_fatal_error_if_not(pseudo_random_gen_factory, "Failed to create factory.");

  std::shared_ptr<low_papr_sequence_generator_factory> low_papr_sequence_gen_factory =
      create_low_papr_sequence_generator_sw_factory();
  report_fatal_error_if_not(low_papr_sequence_gen_factory, "Failed to create factory.");

  std::shared_ptr<time_alignment_estimator_factory> ta_est_factory =
      create_time_alignment_estimator_dft_factory(dft_proc_factory);
  report_fatal_error_if_not(ta_est_factory, "Failed to create factory.");

  std::shared_ptr<port_channel_estimator_factory> chan_estimator_factory =
      create_port_channel_estimator_factory_sw(ta_est_factory);
  report_fatal_error_if_not(chan_estimator_factory, "Failed to create factory.");

  if (max_nof_threads > 1) {
    chan_estimator_factory = create_port_channel_estimator_pool_factory(chan_estimator_factory, max_nof_threads);
    report_fatal_error_if_not(chan_estimator_factory, "Failed to create factory.");
  }

  // CFO compensation is not necessary if time domain interpolation is enabled.
  bool compensate_cfo = td_interpolation_strategy != port_channel_estimator_td_interpolation_strategy::interpolate;

  std::shared_ptr<dmrs_pusch_estimator_factory> chan_est_factory =
      create_dmrs_pusch_estimator_factory_sw(pseudo_random_gen_factory,
                                             low_papr_sequence_gen_factory,
                                             chan_estimator_factory,
                                             executor,
                                             port_channel_estimator_fd_smoothing_strategy::filter,
                                             td_interpolation_strategy,
                                             compensate_cfo);
  report_fatal_error_if_not(chan_est_factory, "Failed to create factory.");

  std::shared_ptr<channel_equalizer_factory> eq_factory =
      create_channel_equalizer_generic_factory(equalizer_algorithm_type);
  report_fatal_error_if_not(eq_factory, "Failed to create factory.");

  std::shared_ptr<demodulation_mapper_factory> chan_demod_factory = create_demodulation_mapper_factory();
  report_fatal_error_if_not(chan_demod_factory, "Failed to create factory.");

  std::shared_ptr<evm_calculator_factory> evm_calc_factory = create_evm_calculator_factory();
  report_fatal_error_if_not(evm_calc_factory, "Failed to create factory.");

  std::shared_ptr<transform_precoder_factory> precoding_factory =
      create_dft_transform_precoder_factory(dft_proc_factory, MAX_RB);
  report_fatal_error_if_not(precoding_factory, "Invalid transform precoding factory.");

  std::shared_ptr<pusch_demodulator_factory> pusch_demod_factory = create_pusch_demodulator_factory_sw(
      eq_factory, precoding_factory, chan_demod_factory, evm_calc_factory, pseudo_random_gen_factory, MAX_RB, true);
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
  pusch_proc_factory_config.estimator_factory          = chan_est_factory;
  pusch_proc_factory_config.demodulator_factory        = pusch_demod_factory;
  pusch_proc_factory_config.demux_factory              = demux_factory;
  pusch_proc_factory_config.decoder_factory            = pusch_dec_factory;
  pusch_proc_factory_config.uci_dec_factory            = uci_dec_factory;
  pusch_proc_factory_config.ch_estimate_dimensions     = {.nof_prb       = MAX_NOF_PRBS,
                                                          .nof_symbols   = MAX_NSYMB_PER_SLOT,
                                                          .nof_rx_ports  = pusch_constants::MAX_NOF_RX_PORTS,
                                                          .nof_tx_layers = pusch_processor_phy_cap.max_nof_layers};
  pusch_proc_factory_config.dec_nof_iterations         = nof_ldpc_iterations;
  pusch_proc_factory_config.dec_enable_early_stop      = dec_enable_early_stop;
  pusch_proc_factory_config.max_nof_concurrent_threads = max_nof_threads;
  pusch_proc_factory_config.csi_sinr_calc_method       = channel_state_information::sinr_type::post_equalization;

  std::shared_ptr<pusch_processor_factory> pusch_proc_factory =
      create_pusch_processor_factory_sw(pusch_proc_factory_config);
  report_fatal_error_if_not(pusch_proc_factory, "Failed to create factory.");

  return pusch_proc_factory;
}
