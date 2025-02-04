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

#include "srsran/du/du_high/o_du_high_metrics.h"
#include "srsran/du/du_low/o_du_low_metrics.h"

namespace srsran {
namespace srs_du {

/// O-RAN DU metrics.
struct o_du_metrics {
  o_du_high_metrics high;
  o_du_low_metrics  low;
};

} // namespace srs_du
} // namespace srsran
