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
#include "srsran/du_low/du_low_wrapper_config.h"

namespace srsran {

struct du_cell_config;
struct du_low_config;
struct du_low_unit_config;

void generate_du_low_wrapper_config(du_low_wrapper_config&              out_config,
                                    const du_low_unit_config&           du_low_unit_cfg,
                                    std::vector<cell_prach_ports_entry> prach_ports,
                                    span<const du_cell_config>          du_cells,
                                    span<const unsigned>                max_puschs_per_slot,
                                    unsigned                            du_id);

} // namespace srsran
