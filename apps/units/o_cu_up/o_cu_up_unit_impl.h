/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "apps/helpers/e2/e2_metric_connector_manager.h"
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
