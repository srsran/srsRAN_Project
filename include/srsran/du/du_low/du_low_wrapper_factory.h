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
#include "srsran/du/du_low/du_low_wrapper.h"

#include <memory>

namespace srsran {

struct du_cell_config;
struct du_low_wrapper_config;

/// Creates and returns a DU low wrapper.
std::unique_ptr<du_low_wrapper> make_du_low_wrapper(const du_low_wrapper_config& config,
                                                    span<const du_cell_config>   du_cells);

} // namespace srsran
