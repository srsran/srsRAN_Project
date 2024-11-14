/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "apps/services/e2/e2_metric_connector_manager.h"
#include "srsran/cu_cp/cu_cp.h"
#include "srsran/e2/e2_cu_metrics_connector.h"
#include "srsran/ngap/gateways/n2_connection_client.h"

namespace srsran {

using e2_cu_metrics_connector_manager =
    e2_metric_connector_manager<e2_cu_metrics_connector, e2_cu_metrics_notifier, e2_cu_metrics_interface>;

/// \brief CU-CP wrapper implementation.
///
/// The purpose of this wrapper is to keep the life cycle of the objects related only to the CU-CP.
class cu_cp_wrapper : public srs_cu_cp::cu_cp
{
public:
  cu_cp_wrapper(std::vector<std::unique_ptr<srs_cu_cp::n2_connection_client>> n2_clients_,
                std::unique_ptr<e2_cu_metrics_connector_manager>              e2_metric_connector_,
                std::unique_ptr<srs_cu_cp::cu_cp>                             cu_cp_);

  // See interface for documentation.
  srs_cu_cp::cu_cp_f1c_handler& get_f1c_handler() override;
  // See interface for documentation.
  srs_cu_cp::cu_cp_e1_handler& get_e1_handler() override;
  // See interface for documentation.
  srs_cu_cp::cu_cp_ng_handler& get_ng_handler() override;
  // See interface for documentation.
  srs_cu_cp::cu_cp_command_handler& get_command_handler() override;
  // See interface for documentation.
  srs_cu_cp::metrics_handler& get_metrics_handler() override;
  // See interface for documentation.
  bool start() override;
  // See interface for documentation.
  void stop() override;

private:
  std::vector<std::unique_ptr<srs_cu_cp::n2_connection_client>> n2_clients;
  std::unique_ptr<e2_cu_metrics_connector_manager>              e2_metric_connector;
  std::unique_ptr<srs_cu_cp::cu_cp>                             cu_cp;
};

} // namespace srsran
