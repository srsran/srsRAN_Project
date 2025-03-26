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

/// \brief RRC metrics for all UEs connected to a DU.
struct rrc_du_metrics {
  unsigned mean_nof_rrc_connections;
  unsigned max_nof_rrc_connections;
};

} // namespace srsran
