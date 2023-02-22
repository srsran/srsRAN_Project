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

namespace srsran {

struct gnb_appconfig;
struct lower_phy_configuration;
class task_executor;
class upper_phy_rg_gateway;
class upper_phy_rx_symbol_request_notifier;

std::unique_ptr<lower_phy> create_lower_phy(lower_phy_configuration& config,
                                            unsigned                 max_nof_prach_concurrent_requests);

std::unique_ptr<upper_phy> create_upper_phy(const gnb_appconfig&                  params,
                                            upper_phy_rg_gateway*                 rg_gateway,
                                            task_executor*                        dl_executor,
                                            task_executor*                        pucch_executor,
                                            task_executor*                        pusch_executor,
                                            task_executor*                        prach_executor,
                                            upper_phy_rx_symbol_request_notifier* rx_symbol_request_notifier);

} // namespace srsran
