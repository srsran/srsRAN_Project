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

struct ru_generic_configuration;

/// Returns a generic Radio Unit using the given configuration.
std::unique_ptr<radio_unit> create_generic_ru(ru_generic_configuration& config);

} // namespace srsran
