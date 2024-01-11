/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/span.h"
#include "srsran/phy/lower/lower_phy.h"
#include "srsran/phy/upper/upper_phy.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/subcarrier_spacing.h"

namespace srsran {

class task_executor;
class upper_phy_rg_gateway;
class upper_phy_rx_symbol_request_notifier;

/// Upper PHY parameters to configure the upper PHY factory.
struct upper_phy_params {
  srslog::basic_levels     log_level;
  bs_channel_bandwidth_fr1 channel_bw_mhz;
  subcarrier_spacing       scs;
};

std::unique_ptr<upper_phy> create_upper_phy(const upper_phy_params&               params,
                                            upper_phy_rg_gateway*                 rg_gateway,
                                            span<task_executor*>                  dl_executors,
                                            task_executor*                        ul_executor,
                                            upper_phy_rx_symbol_request_notifier* rx_symbol_request_notifier);

} // namespace srsran
