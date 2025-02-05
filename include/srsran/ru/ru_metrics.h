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

#include "srsran/ru/dummy/ru_dummy_metrics.h"
#include "srsran/ru/generic/ru_generic_metrics.h"
#include "srsran/ru/ofh/ru_ofh_metrics.h"
#include <variant>

namespace srsran {

/// Radio Unit metrics.
struct ru_metrics {
  std::variant<ru_dummy_metrics, ru_generic_metrics, ru_ofh_metrics> metrics;
};

} // namespace srsran
