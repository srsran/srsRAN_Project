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

#include "srsran/ru/ru.h"
#include <memory>

namespace srsran {

struct ru_dummy_configuration;
struct ru_dummy_dependencies;

/// Returns a generic Radio Unit using the given configuration.
std::unique_ptr<radio_unit> create_dummy_ru(const ru_dummy_configuration& config, ru_dummy_dependencies& dependencies);

} // namespace srsran
