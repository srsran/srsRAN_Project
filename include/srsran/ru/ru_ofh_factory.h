/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ru/ru.h"
#include "srsran/ru/ru_ofh_configuration.h"

namespace srsran {

/// Returns an Open FrontHaul Radio Unit using the given configuration.
std::unique_ptr<radio_unit> create_ofh_ru(const ru_ofh_configuration& config);

} // namespace srsran
