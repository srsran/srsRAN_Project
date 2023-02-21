/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "phy_factory.h"
#include "srsran/phy/lower/lower_phy_factory.h"
#include "srsran/phy/upper/upper_phy_factories.h"
#include "srsran/ran/band_helper.h"

using namespace srsran;

std::unique_ptr<lower_phy> srsran::create_lower_phy(lower_phy_configuration& config,
                                                    unsigned                 max_nof_prach_concurrent_requests)
{
  // Create DFT factory. It tries to create a FFTW based factory. If FFTW library is not available, it creates a generic
  // DFT factory.
  std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw();
  if (dft_factory == nullptr) {
    dft_factory = create_dft_processor_factory_generic();
  }
  report_fatal_error_if_not(dft_factory, "Failed to create DFT factory.");

  // Create OFDM modulator factory.
  ofdm_factory_generic_configuration ofdm_common_config;
  ofdm_common_config.dft_factory                            = dft_factory;
  std::shared_ptr<ofdm_modulator_factory> modulator_factory = create_ofdm_modulator_factory_generic(ofdm_common_config);
  report_fatal_error_if_not(modulator_factory, "Failed to create OFDM modulator factory.");

  // Create OFDM demodulator factory.
  std::shared_ptr<ofdm_demodulator_factory> demodulator_factory =
      create_ofdm_demodulator_factory_generic(ofdm_common_config);
  report_fatal_error_if_not(demodulator_factory, "Failed to create OFDM demodulator factory.");

  // Create OFDM PRACH demodulator factory.
  std::shared_ptr<ofdm_prach_demodulator_factory> prach_demodulator_factory =
      create_ofdm_prach_demodulator_factory_sw(dft_factory, config.srate.get_dft_size(15e3));

  // Create PRACH processor factory.
  std::shared_ptr<prach_processor_factory> prach_processor_factory = create_prach_processor_factory_sw(
      prach_demodulator_factory, config.srate.get_dft_size(15e3), max_nof_prach_concurrent_requests);
  report_fatal_error_if_not(prach_processor_factory, "Failed to create PRACH processor factory.");

  // Create amplitude control factory.
  std::shared_ptr<amplitude_controller_factory> amplitude_control_factory =
      create_amplitude_controller_clipping_factory(config.amplitude_config);
  report_fatal_error_if_not(amplitude_control_factory, "Failed to create amplitude controller factory.");

  // Create Lower PHY factory.
  std::shared_ptr<lower_phy_factory> lphy_factory = create_lower_phy_factory_sw(
      modulator_factory, demodulator_factory, prach_processor_factory, amplitude_control_factory);
  report_fatal_error_if_not(lphy_factory, "Failed to create lower PHY factory.");

  return lphy_factory->create(config);
}

std::unique_ptr<upper_phy> srsran::create_upper_phy(const upper_phy_params&               params,
                                                    upper_phy_rg_gateway*                 rg_gateway,
                                                    task_executor*                        dl_executor,
                                                    task_executor*                        ul_executor,
                                                    upper_phy_rx_symbol_request_notifier* rx_symbol_request_notifier)
{
  downlink_processor_factory_sw_config dl_proc_config;
  dl_proc_config.ldpc_encoder_type   = "auto";
  dl_proc_config.crc_calculator_type = "auto";

  // Create downlink processor factory.
  std::shared_ptr<downlink_processor_factory> dl_proc_factory = create_downlink_processor_factory_sw(dl_proc_config);
  report_fatal_error_if_not(dl_proc_factory, "Invalid DL processor factory.");

  // Create upper PHY factory.
  std::unique_ptr<upper_phy_factory> upper_phy_factory = create_upper_phy_factory(dl_proc_factory);
  report_fatal_error_if_not(upper_phy_factory, "Invalid upper PHY factory.");

  static constexpr unsigned dl_pipeline_depth    = 8;
  static constexpr unsigned ul_pipeline_depth    = 8;
  static constexpr unsigned prach_pipeline_depth = 1;

  upper_phy_config upper_config;
  upper_config.log_level                = params.log_level;
  upper_config.enable_logging_broadcast = (params.log_level != srslog::basic_levels::none);
  upper_config.rx_symbol_printer_filename.clear();
  upper_config.logger_max_hex_size     = 64;
  upper_config.enable_evm              = true;
  upper_config.sector_id               = 0;
  upper_config.nof_ports               = 1;
  upper_config.ldpc_decoder_iterations = 6;
  upper_config.ldpc_decoder_early_stop = true;

  unsigned nof_slots_per_subframe        = get_nof_slots_per_subframe(params.scs);
  upper_config.nof_slots_dl_rg           = dl_pipeline_depth * nof_slots_per_subframe;
  upper_config.nof_dl_processors         = upper_config.nof_slots_dl_rg;
  upper_config.nof_slots_ul_rg           = ul_pipeline_depth * nof_slots_per_subframe;
  upper_config.nof_ul_processors         = upper_config.nof_slots_ul_rg;
  upper_config.max_ul_thread_concurrency = 4;
  upper_config.nof_prach_buffer          = prach_pipeline_depth * nof_slots_per_subframe;

  upper_config.active_scs                                  = {};
  upper_config.active_scs[to_numerology_value(params.scs)] = true;

  unsigned bw_rb        = band_helper::get_n_rbs_from_bw(params.channel_bw_mhz, params.scs, frequency_range::FR1);
  upper_config.dl_bw_rb = bw_rb;
  upper_config.ul_bw_rb = bw_rb;

  upper_config.softbuffer_config.max_softbuffers      = 4 * upper_config.nof_ul_processors;
  upper_config.softbuffer_config.max_nof_codeblocks   = 128;
  upper_config.softbuffer_config.max_codeblock_size   = ldpc::MAX_CODEBLOCK_SIZE;
  upper_config.softbuffer_config.expire_timeout_slots = 100 * nof_slots_per_subframe;

  upper_config.rg_gateway                 = rg_gateway;
  upper_config.dl_executor                = dl_executor;
  upper_config.pucch_executor             = ul_executor;
  upper_config.pusch_executor             = ul_executor;
  upper_config.prach_executor             = ul_executor;
  upper_config.rx_symbol_request_notifier = rx_symbol_request_notifier;
  upper_config.crc_calculator_type        = "auto";
  upper_config.ldpc_rate_dematcher_type   = "auto";
  upper_config.ldpc_decoder_type          = "auto";

  return upper_phy_factory->create(upper_config);
}
