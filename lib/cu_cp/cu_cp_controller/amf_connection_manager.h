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

#include "../ngap_repository.h"
#include "../ue_manager/ue_manager_impl.h"
#include "srsran/cu_cp/common_task_scheduler.h"
#include "srsran/cu_cp/cu_cp.h"
#include "srsran/ran/plmn_identity.h"
#include <future>

namespace srsran {
namespace srs_cu_cp {

class cu_cp_routine_manager;
struct cu_cp_configuration;

class amf_connection_manager
{
public:
  amf_connection_manager(ngap_repository&                ngaps_,
                         cu_cp_amf_reconnection_handler& cu_cp_notifier_,
                         timer_manager&                  timers_,
                         task_executor&                  cu_cp_exec_,
                         common_task_scheduler&          common_task_sched_);

  /// \brief Initiates the connection to the AMF.
  /// A promise is passed as a parameter to enable blocking synchronization between the completion of the scheduled
  /// async task and the caller side.
  void connect_to_amf(std::promise<bool>* completion_signal = nullptr);

  /// \brief Initiate procedure to disconnect from the N2 interface.
  async_task<void> disconnect_amf();

  /// \brief Handles the loss of connection to the AMF.
  /// \param[in] amf_index The index of the AMF that has been disconnected.
  void handle_amf_connection_loss(amf_index_t amf_index);

  /// \brief Initiates the reconnection to the AMF.
  /// \param[in] amf_index The index of the AMF to reconnect to.
  /// \param[in] ue_mng The UE manager to re-enable UE connections in case the reconnection was successful.
  /// \param[in] amf_reconnection_retry_time The time to wait before retrying the reconnection.
  void
  reconnect_to_amf(amf_index_t amf_index, ue_manager* ue_mng, std::chrono::milliseconds amf_reconnection_retry_time);

  void stop();

  /// Checks whether the CU-CP is connected to the AMF.
  bool is_amf_connected(plmn_identity plmn) const;
  bool is_amf_connected(amf_index_t amf_index) const;

private:
  void        handle_connection_setup_result(amf_index_t amf_index, bool success);
  amf_index_t plmn_to_amf_index(plmn_identity plmn) const;

  ngap_repository&                ngaps;
  cu_cp_amf_reconnection_handler& cu_cp_notifier;
  timer_manager&                  timers;
  task_executor&                  cu_cp_exec;
  common_task_scheduler&          common_task_sched;
  srslog::basic_logger&           logger;

  std::unordered_map<amf_index_t, std::atomic<bool>> amfs_connected;

  std::atomic<bool> stopped{false};
};

} // namespace srs_cu_cp
} // namespace srsran
