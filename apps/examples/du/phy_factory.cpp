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
#include "srsgnb/phy/lower/lower_phy_factory.h"
#include "srsgnb/phy/upper/upper_phy_factories.h"

using namespace srsgnb;

std::unique_ptr<lower_phy> srsgnb::create_lower_phy(lower_phy_configuration& config,
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

std::unique_ptr<upper_phy> srsgnb::create_upper_phy(const upper_phy_params&               params,
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

  upper_phy_config upper_config;
  upper_config.log_level                                   = params.log_level;
  upper_config.sector_id                                   = 0;
  upper_config.nof_ports                                   = 1;
  upper_config.nof_slots_dl_rg                             = 2 * params.dl_pipeline_depth;
  upper_config.nof_slots_ul_rg                             = upper_config.nof_slots_dl_rg;
  upper_config.nof_prach_buffer                            = 1;
  upper_config.nof_dl_processors                           = 2 * params.dl_pipeline_depth;
  upper_config.dl_bw_rb                                    = params.dl_bw_rb;
  upper_config.ul_bw_rb                                    = params.ul_bw_rb;
  upper_config.rg_gateway                                  = rg_gateway;
  upper_config.dl_executor                                 = dl_executor;
  upper_config.nof_ul_processors                           = 8 * params.dl_pipeline_depth;
  upper_config.pucch_executor                              = ul_executor;
  upper_config.pusch_executor                              = ul_executor;
  upper_config.prach_executor                              = ul_executor;
  upper_config.crc_calculator_type                         = "auto";
  upper_config.ldpc_rate_dematcher_type                    = "auto";
  upper_config.ldpc_decoder_type                           = "auto";
  upper_config.rx_symbol_request_notifier                  = rx_symbol_request_notifier;
  upper_config.active_scs                                  = {};
  upper_config.active_scs[to_numerology_value(params.scs)] = true;

  upper_config.softbuffer_config.max_softbuffers      = 4 * upper_config.nof_ul_processors;
  upper_config.softbuffer_config.max_nof_codeblocks   = 128;
  upper_config.softbuffer_config.max_codeblock_size   = (1U << 18);
  upper_config.softbuffer_config.expire_timeout_slots = 100 * get_nof_slots_per_subframe(params.scs);

  return upper_phy_factory->create(upper_config);
}
