/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/lower/sampling_rate.h"
#include "srsgnb/radio/radio_session.h"

namespace srsgnb {

class task_executor;

/// Radio parameters to configure the radio factory.
struct radio_params {
  /// Basic parameters.
  std::string                               device_args;
  std::string                               log_level;
  sampling_rate                             srate;
  radio_configuration::over_the_wire_format otw_format;
  unsigned                                  nof_sectors;
  unsigned                                  nof_ports;
  /// Transmission parameters.
  double                   dl_frequency_hz;
  double                   tx_gain;
  std::vector<std::string> tx_channel_args;
  /// Reception parameters.
  double                   ul_frequency_hz;
  double                   rx_gain;
  std::vector<std::string> rx_channel_args;
};

std::unique_ptr<radio_session> create_radio(const std::string&          radio_driver,
                                            const std::string&          device_address,
                                            const radio_params&         params,
                                            task_executor&              executor,
                                            radio_notification_handler& radio_handler);

} // namespace srsgnb
