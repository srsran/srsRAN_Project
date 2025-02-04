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

#include "srsran/du/du_high/o_du_high.h"
#include "srsran/du/du_low/o_du_low.h"
#include <memory>

namespace srsran {
namespace srs_du {

class o_du_metrics_notifier;

/// O-RAN Distributed Unit dependencies.
struct o_du_dependencies {
  o_du_metrics_notifier*     metrics_notifier = nullptr;
  std::unique_ptr<o_du_high> odu_hi;
  std::unique_ptr<o_du_low>  odu_lo;
};

} // namespace srs_du
} // namespace srsran
