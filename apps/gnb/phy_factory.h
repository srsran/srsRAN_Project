/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/lower/lower_phy.h"
#include "srsgnb/phy/upper/upper_phy.h"

namespace srsgnb {

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

} // namespace srsgnb
