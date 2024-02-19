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

#include "amf_connection_manager.h"
#include "node_connection_notifier.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/cu_cp/cu_cp_e1_handler.h"

namespace srsran {
namespace srs_cu_cp {

class cu_up_processor_repository;

/// \brief Entity responsible for managing the CU-CP connections to remote nodes and determining whether the CU-CP
/// is in a state to accept new connections.
///
/// In particular, this class is responsible for:
/// - triggering an AMF connection and tracking the status of the connection;
/// - determining whether a new DU setup request should be accepted based on the status of other remote node
/// connections;
/// - determining whether new UEs should be accepted depending on the status of the CU-CP remote connections.
class cu_cp_controller
{
public:
  cu_cp_controller(cu_cp_routine_manager&            routine_manager_,
                   const ngap_configuration&         ngap_cfg_,
                   cu_cp_ngap_control_notifier&      ngap_ctrl_notif_,
                   const cu_up_processor_repository& cu_ups_);

  amf_connection_manager& amf_connection_handler() { return amf_mng; }

  bool handle_du_setup_request(const du_setup_request& req);

  /// \brief Determines whether the CU-CP should accept a new UE connection.
  bool request_ue_setup() const;

private:
  amf_connection_manager            amf_mng;
  const cu_up_processor_repository& cu_ups;
};

} // namespace srs_cu_cp
} // namespace srsran
