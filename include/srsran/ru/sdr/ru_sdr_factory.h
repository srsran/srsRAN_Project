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

struct ru_sdr_configuration;
struct ru_sdr_dependencies;

/// Returns an SDR Radio Unit using the given configuration.
std::unique_ptr<radio_unit> create_sdr_ru(const ru_sdr_configuration& config, const ru_sdr_dependencies& dependencies);

} // namespace srsran
