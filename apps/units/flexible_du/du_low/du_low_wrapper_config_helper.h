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
#include "srsran/du/du_low/du_low_wrapper_config.h"

namespace srsran {

namespace srs_du {
struct du_cell_config;
}

struct du_low_unit_config;
struct worker_manager;

void make_du_low_wrapper_config_and_dependencies(srs_du::du_low_wrapper_config&              out_cfg,
                                                 const du_low_unit_config&                   du_low_unit_cfg,
                                                 std::vector<srs_du::cell_prach_ports_entry> prach_ports,
                                                 span<const srs_du::du_cell_config>          du_cells,
                                                 span<const unsigned>                        max_puschs_per_slot,
                                                 upper_phy_rg_gateway&                       rg_gateway,
                                                 upper_phy_rx_symbol_request_notifier&       rx_symbol_request_notifier,
                                                 worker_manager&                             workers,
                                                 unsigned                                    du_id);

} // namespace srsran
