/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "apps/units/flexible_o_du/o_du_unit.h"
#include <memory>
#include <vector>

namespace srsran {

namespace fapi {
class fapi_adaptor;
} // namespace fapi

struct split6_o_du_unit_config;

/// Helper function to build the O-RAN DU Split 6 with the given arguments.
o_du_unit create_o_du_split6(const split6_o_du_unit_config&                   du_unit_cfg,
                             const o_du_unit_dependencies&                    du_dependencies,
                             std::vector<std::unique_ptr<fapi::fapi_adaptor>> fapi_adaptors);

} // namespace srsran
