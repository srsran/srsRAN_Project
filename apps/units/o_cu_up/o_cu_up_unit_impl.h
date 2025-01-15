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

#include "apps/services/e2/e2_metric_connector_manager.h"
#include "srsran/cu_up/cu_up_operation_controller.h"
#include "srsran/cu_up/o_cu_up.h"
#include "srsran/e2/e2_cu_metrics_connector.h"
#include "srsran/gtpu/gtpu_gateway.h"

namespace srsran {

using e2_cu_metrics_connector_manager =
    e2_metric_connector_manager<e2_cu_metrics_connector, e2_cu_metrics_notifier, e2_cu_metrics_interface>;

/// \brief O-RAN CU-UP unit implementation.
///
/// This implementation purpose is to manage the life of the given NGU gateway and the CU-UP interface.
class o_cu_up_unit_impl : public srs_cu_up::o_cu_up
{
public:
  o_cu_up_unit_impl(std::vector<std::unique_ptr<gtpu_gateway>>       gateways_,
                    std::unique_ptr<e2_cu_metrics_connector_manager> e2_metric_connector_,
                    std::unique_ptr<srs_cu_up::o_cu_up>              cu_up_);

  // See interface for documentation.
  srs_cu_up::cu_up_interface& get_cu_up() override;

  // See interface for documentation.
  srs_cu_up::cu_up_operation_controller& get_operation_controller() override;

private:
  std::vector<std::unique_ptr<gtpu_gateway>>       gateways;
  std::unique_ptr<e2_cu_metrics_connector_manager> e2_metric_connector;
  std::unique_ptr<srs_cu_up::o_cu_up>              cu_up;
};

} // namespace srsran
