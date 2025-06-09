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

#include "srsran/du/du_low/o_du_low_metrics.h"
#include "srsran/ru/ru_metrics.h"

namespace srsran {

/// Flexible O-RAN DU metrics.
struct split6_flexible_o_du_low_metrics {
  srs_du::o_du_low_metrics du_low;
  ru_metrics               ru;
};

} // namespace srsran
