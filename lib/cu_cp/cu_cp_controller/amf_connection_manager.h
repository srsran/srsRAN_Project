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

#include "../ngap_repository.h"
#include "common_task_scheduler.h"
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
  amf_connection_manager(ngap_repository&       ngaps_,
                         task_executor&         cu_cp_exec_,
                         common_task_scheduler& common_task_sched_);

  /// \brief Initiates the connection to the AMF.
  /// A promise is passed as a parameter to enable blocking synchronization between the completion of the scheduled
  /// async task and the caller side.
  void connect_to_amf(std::promise<bool>* completion_signal = nullptr);

  /// \brief Initiate procedure to disconnect from the N2 interface.
  async_task<void> disconnect_amf();

  void stop();

  /// Checks whether the CU-CP is connected to the AMF.
  bool is_amf_connected(plmn_identity plmn) const;
  bool is_amf_connected(amf_index_t amf_index) const;

private:
  void        handle_connection_setup_result(amf_index_t amf_index, bool success);
  amf_index_t plmn_to_amf_index(plmn_identity plmn) const;

  ngap_repository&       ngaps;
  task_executor&         cu_cp_exec;
  common_task_scheduler& common_task_sched;
  srslog::basic_logger&  logger;

  std::unordered_map<amf_index_t, std::atomic<bool>> amfs_connected;

  std::atomic<bool>       stopped{false};
  std::mutex              stop_mutex;
  std::condition_variable stop_cvar;
  bool                    stop_completed = false;
};

} // namespace srs_cu_cp
} // namespace srsran