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

#include "srsran/phy/lower/lower_phy.h"
#include "srsran/phy/upper/upper_phy.h"
#include "srsran/ran/bs_channel_bandwidth.h"

namespace srsran {

struct lower_phy_configuration;
class task_executor;
class upper_phy_rg_gateway;
class upper_phy_rx_symbol_request_notifier;

std::unique_ptr<lower_phy> create_lower_phy(lower_phy_configuration& config,
                                            unsigned                 max_nof_prach_concurrent_requests);

/// Upper PHY parameters to configure the upper PHY factory.
struct upper_phy_params {
  srslog::basic_levels     log_level;
  bs_channel_bandwidth_fr1 channel_bw_mhz;
  subcarrier_spacing       scs;
};

std::unique_ptr<upper_phy> create_upper_phy(const upper_phy_params&               params,
                                            upper_phy_rg_gateway*                 rg_gateway,
                                            task_executor*                        dl_executor,
                                            task_executor*                        ul_executor,
                                            upper_phy_rx_symbol_request_notifier* rx_symbol_request_notifier);

} // namespace srsran
