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
#include "srsran/du/du_low/o_du_low.h"
#include <memory>

namespace srsran {
namespace srs_du {

struct du_cell_config;
struct o_du_low_config;

/// Creates and returns an ORAN Distributed Unit (O-DU) low.
std::unique_ptr<o_du_low> make_o_du_low(const o_du_low_config& config, span<const du_cell_config> du_cells);

} // namespace srs_du
} // namespace srsran
