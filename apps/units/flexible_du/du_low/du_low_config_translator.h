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
struct du_low_config;
} // namespace srs_du

struct du_low_unit_config;

void generate_du_low_wrapper_config(srs_du::du_low_wrapper_config&              out_config,
                                    const du_low_unit_config&                   du_low_unit_cfg,
                                    const hal_upper_phy_config&                 hal_config,
                                    std::vector<srs_du::cell_prach_ports_entry> prach_ports,
                                    span<const srs_du::du_cell_config>          du_cells,
                                    span<const unsigned>                        max_puschs_per_slot,
                                    unsigned                                    du_id);

} // namespace srsran
