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

#include "srsran/ru/ru.h"
#include <memory>

namespace srsran {

struct ru_ofh_configuration;
struct ru_ofh_dependencies;

/// Returns an Open FrontHaul Radio Unit using the given configuration and dependencies.
std::unique_ptr<radio_unit> create_ofh_ru(const ru_ofh_configuration& config, ru_ofh_dependencies&& dependencies);

} // namespace srsran
