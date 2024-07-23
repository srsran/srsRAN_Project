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
#include "cu_cp_ue_admission_controller.h"
#include "cu_up_connection_manager.h"
#include "du_connection_manager.h"
#include "node_connection_notifier.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/cu_cp/cu_cp_e1_handler.h"

namespace srsran {
namespace srs_cu_cp {

class cu_up_processor_repository;
class ue_manager;

/// \brief Entity responsible for managing the CU-CP connections to remote nodes and determining whether the CU-CP
/// is in a state to accept new connections.
///
/// In particular, this class is responsible for:
/// - triggering an AMF connection and tracking the status of the connection;
/// - determining whether a new DU setup request should be accepted based on the status of other remote node
/// connections;
/// - determining whether new UEs should be accepted depending on the status of the CU-CP remote connections.
class cu_cp_controller : public cu_cp_ue_admission_controller
{
public:
  cu_cp_controller(const cu_cp_configuration&  config_,
                   cu_cp_routine_manager&      routine_manager_,
                   ue_manager&                 ue_mng_,
                   ngap_connection_manager&    ngap_conn_mng_,
                   cu_up_processor_repository& cu_ups_,
                   du_processor_repository&    dus_,
                   task_executor&              ctrl_exec);

  void stop();

  amf_connection_manager& amf_connection_handler() { return amf_mng; }

  bool handle_du_setup_request(du_index_t du_idx, const du_setup_request& req);

  /// \brief Determines whether the CU-CP should accept a new UE connection.
  bool request_ue_setup() const override;

  cu_cp_f1c_handler& get_f1c_handler() { return du_mng; }
  cu_cp_e1_handler&  get_e1_handler() { return cu_up_mng; }

private:
  void stop_impl();

  const cu_cp_configuration& cfg;
  ue_manager&                ue_mng;
  cu_cp_routine_manager&     routine_mng;
  task_executor&             ctrl_exec;
  srslog::basic_logger&      logger;

  amf_connection_manager   amf_mng;
  du_connection_manager    du_mng;
  cu_up_connection_manager cu_up_mng;

  std::mutex              mutex;
  std::condition_variable cvar;
  bool                    running = true;
};

} // namespace srs_cu_cp
} // namespace srsran
