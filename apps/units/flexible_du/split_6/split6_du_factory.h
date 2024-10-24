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

#include "apps/units/flexible_du/o_du_unit.h"
#include <memory>
#include <vector>

namespace srsran {

namespace fapi {
class fapi_adaptor;
} // namespace fapi

struct split6_du_unit_config;

/// Helper function to build the DU Split 6 with the given arguments.
o_du_unit create_du_split6(const split6_du_unit_config&                     du_unit_cfg,
                           const du_unit_dependencies&                      du_dependencies,
                           std::vector<std::unique_ptr<fapi::fapi_adaptor>> fapi_adaptors);

} // namespace srsran
