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
#include "gnb_appconfig.h"
#include "gnb_appconfig_translators.h"
#include "srsran/phy/lower/lower_phy_factory.h"
#include "srsran/phy/upper/upper_phy_factories.h"

using namespace srsran;

std::unique_ptr<lower_phy> srsran::create_lower_phy(lower_phy_configuration& config,
                                                    unsigned                 max_nof_prach_concurrent_requests)
{
  // Create DFT factory. It tries to create a FFTW based factory. If FFTW library is not available, it creates a FFTX
  // based factory.
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

std::unique_ptr<upper_phy> srsran::create_upper_phy(const gnb_appconfig&                  params,
                                                    upper_phy_rg_gateway*                 rg_gateway,
                                                    task_executor*                        dl_executor,
                                                    task_executor*                        pucch_executor,
                                                    task_executor*                        pusch_executor,
                                                    task_executor*                        prach_executor,
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

  std::vector<upper_phy_config> config = generate_du_low_config(params);

  // Fill the rest with the parameters.
  upper_phy_config& cfg          = config.front();
  cfg.rg_gateway                 = rg_gateway;
  cfg.dl_executor                = dl_executor;
  cfg.pucch_executor             = pucch_executor;
  cfg.pusch_executor             = pusch_executor;
  cfg.prach_executor             = prach_executor;
  cfg.rx_symbol_request_notifier = rx_symbol_request_notifier;
  cfg.crc_calculator_type        = "auto";
  cfg.ldpc_rate_dematcher_type   = "auto";
  cfg.ldpc_decoder_type          = "auto";

  return upper_phy_factory->create(cfg);
}
