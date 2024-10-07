/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/cu_cp/cu_cp.h"
#include "srsran/ngap/gateways/n2_connection_client.h"

namespace srsran {

/// \brief CU-CP wrapper implementation.
///
/// The purpose of this wrapper is to keep the life cycle of the objects related only to the CU-CP.
class cu_cp_wrapper : public srs_cu_cp::cu_cp
{
public:
  cu_cp_wrapper(std::vector<std::unique_ptr<srs_cu_cp::n2_connection_client>> n2_clients_,
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
  std::unique_ptr<srs_cu_cp::cu_cp>                             cu_cp;
};

} // namespace srsran
