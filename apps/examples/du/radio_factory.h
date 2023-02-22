/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#pragma once

#include "srsran/phy/lower/sampling_rate.h"
#include "srsran/radio/radio_session.h"

namespace srsran {

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
                                            const radio_params&         params,
                                            task_executor&              executor,
                                            radio_notification_handler& radio_handler);

} // namespace srsran
