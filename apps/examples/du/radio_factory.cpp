/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "radio_factory.h"
#include "srsran/radio/radio_configuration.h"
#include "srsran/radio/radio_factory.h"

using namespace srsran;

static radio_configuration::radio create_radio_configuration(const radio_params& params)
{
  radio_configuration::radio radio_config = {};
  radio_config.args                       = params.device_args;
  radio_config.sampling_rate_hz           = params.srate.to_Hz<double>();
  radio_config.otw_format                 = params.otw_format;
  radio_config.log_level                  = params.log_level;
  for (unsigned sector_id = 0; sector_id != params.nof_sectors; ++sector_id) {
    // For each channel in the streams...
    radio_configuration::stream tx_stream_config;
    radio_configuration::stream rx_stream_config;
    for (unsigned port_id = 0; port_id != params.nof_ports; ++port_id) {
      // Create channel configuration and append it to the previous ones.
      radio_configuration::channel tx_ch_config;
      tx_ch_config.freq.center_frequency_hz = params.dl_frequency_hz;
      tx_ch_config.freq.lo_frequency_hz     = 0.0;
      tx_ch_config.gain_dB                  = params.tx_gain;
      if (!params.tx_channel_args.empty()) {
        tx_ch_config.args = params.tx_channel_args[sector_id * params.nof_ports + port_id];
      }
      tx_stream_config.channels.emplace_back(tx_ch_config);

      radio_configuration::channel rx_ch_config;
      rx_ch_config.freq.center_frequency_hz = params.ul_frequency_hz;
      rx_ch_config.freq.lo_frequency_hz     = 0.0;
      rx_ch_config.gain_dB                  = params.rx_gain;
      if (!params.rx_channel_args.empty()) {
        rx_ch_config.args = params.rx_channel_args[sector_id * params.nof_ports + port_id];
      }
      rx_stream_config.channels.emplace_back(rx_ch_config);
    }
    radio_config.tx_streams.emplace_back(tx_stream_config);
    radio_config.rx_streams.emplace_back(rx_stream_config);
  }
  return radio_config;
}

std::unique_ptr<radio_session> srsran::create_radio(const std::string&          radio_driver,
                                                    const radio_params&         params,
                                                    task_executor&              executor,
                                                    radio_notification_handler& radio_handler)
{
  std::unique_ptr<radio_factory> factory = create_radio_factory(radio_driver);
  report_fatal_error_if_not(factory, "Invalid radio factory {}.", radio_driver);

  // Create radio configuration. Assume 1 sector per stream.
  radio_configuration::radio radio_config = create_radio_configuration(params);

  // Validate radio configuration.
  report_fatal_error_if_not(factory->get_configuration_validator().is_configuration_valid(radio_config),
                            "Invalid radio configuration.");

  // Create actual radio.
  return factory->create(radio_config, executor, radio_handler);
}
