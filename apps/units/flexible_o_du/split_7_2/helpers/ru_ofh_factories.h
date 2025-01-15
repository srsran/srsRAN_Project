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

#include "ru_ofh_config.h"
#include "srsran/ru/ru.h"
#include <memory>

namespace srsran {
struct flexible_o_du_ru_config;
struct flexible_o_du_ru_dependencies;

/// Creates an Open Fronthaul Radio Unit using the given configuration.
std::unique_ptr<radio_unit> create_ofh_radio_unit(const ru_ofh_unit_config&            ru_cfg,
                                                  const flexible_o_du_ru_config&       ru_config,
                                                  const flexible_o_du_ru_dependencies& ru_dependencies);

} // namespace srsran
