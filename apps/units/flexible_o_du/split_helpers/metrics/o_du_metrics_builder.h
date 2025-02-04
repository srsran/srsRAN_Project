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

#include "apps/services/metrics/metrics_config.h"

namespace srsran {

namespace app_services {
class metrics_notifier;
}

namespace srs_du {
class o_du_metrics_notifier;
}

/// Generates the O-DU metrics configuration.
srs_du::o_du_metrics_notifier* generate_o_du_metrics(std::vector<app_services::metrics_config>& metrics,
                                                     app_services::metrics_notifier&            notifier);

} // namespace srsran
