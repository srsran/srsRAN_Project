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
#include "srsran/cu_cp/o_cu_cp.h"
#include "srsran/e2/e2_cu_metrics_connector.h"
#include "srsran/ngap/gateways/n2_connection_client.h"

namespace srsran {

using e2_cu_metrics_connector_manager =
    e2_metric_connector_manager<e2_cu_metrics_connector, e2_cu_metrics_notifier, e2_cu_metrics_interface>;

/// \brief O-RAN CU-CP unit implementation.
///
/// The purpose of this unit is to keep the life cycle of the objects related only to the O-RAN CU-CP.
class o_cu_cp_unit_impl : public srs_cu_cp::o_cu_cp
{
public:
  o_cu_cp_unit_impl(std::vector<std::unique_ptr<srs_cu_cp::n2_connection_client>> n2_clients_,
                    std::unique_ptr<e2_cu_metrics_connector_manager>              e2_metric_connector_,
                    std::unique_ptr<srs_cu_cp::o_cu_cp>                           cu_cp_);

  // See interface for documentation.
  srs_cu_cp::cu_cp& get_cu_cp() override;

  // See interface for documentation.
  srs_cu_cp::cu_cp_operation_controller& get_operation_controller() override;

private:
  std::vector<std::unique_ptr<srs_cu_cp::n2_connection_client>> n2_clients;
  std::unique_ptr<e2_cu_metrics_connector_manager>              e2_metric_connector;
  std::unique_ptr<srs_cu_cp::o_cu_cp>                           o_cu;
};

} // namespace srsran
