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

#include "srsgnb/phy/lower/lower_phy.h"
#include "srsgnb/phy/upper/upper_phy.h"

namespace srsgnb {

struct lower_phy_configuration;
class task_executor;
class upper_phy_rg_gateway;
class upper_phy_rx_symbol_request_notifier;

std::unique_ptr<lower_phy> create_lower_phy(lower_phy_configuration& config,
                                            unsigned                 max_nof_prach_concurrent_requests);

/// Upper PHY parameters to configure the upper PHY factory.
struct upper_phy_params {
  srslog::basic_levels log_level;
  unsigned             nof_ports;
  unsigned             dl_pipeline_depth;
  unsigned             dl_bw_rb;
  unsigned             ul_bw_rb;
};

std::unique_ptr<upper_phy> create_upper_phy(const upper_phy_params&               params,
                                            upper_phy_rg_gateway*                 rg_gateway,
                                            task_executor*                        dl_executor,
                                            task_executor*                        ul_executor,
                                            upper_phy_rx_symbol_request_notifier* rx_symbol_request_notifier);

} // namespace srsgnb
