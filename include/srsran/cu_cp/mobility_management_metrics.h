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

namespace srsran {

// Mobility Management metrics, see TS 28.552 section 5.1.1.6.
struct mobility_management_metrics {
  unsigned nof_handover_preparations_requested  = 0;
  unsigned nof_successful_handover_preparations = 0;
};

} // namespace srsran
