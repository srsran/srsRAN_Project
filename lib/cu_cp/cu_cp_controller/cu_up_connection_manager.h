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

#include "srsran/cu_cp/cu_cp_e1_handler.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/executors/task_executor.h"
#include <condition_variable>

namespace srsran {
namespace srs_cu_cp {

class cu_up_processor_repository;
class common_task_scheduler;

/// \brief This class is responsible for allocating the resources in the CU-CP required to handle the establishment
/// or drop of E1 GW connections.
///
/// This class acts as a facade, hiding the details associated with the dispatching of E1 GW events to the
/// the CU-CP through the appropriate task executors.
class cu_up_connection_manager : public cu_cp_e1_handler
{
public:
  cu_up_connection_manager(unsigned                    max_nof_cu_ups,
                           cu_up_processor_repository& cus_up_,
                           task_executor&              cu_cp_exec_,
                           common_task_scheduler&      common_task_sched_);

  std::unique_ptr<e1ap_message_notifier>
  handle_new_cu_up_connection(std::unique_ptr<e1ap_message_notifier> e1ap_tx_pdu_notifier) override;

  void stop();

  size_t nof_cu_ups() const { return cu_up_count.load(std::memory_order_relaxed); }

private:
  class shared_cu_up_connection_context;
  class e1_gw_to_cu_cp_pdu_adapter;

  // Called by the E1 GW when it disconnects its PDU notifier endpoint.
  void handle_e1_gw_connection_closed(cu_up_index_t cu_up_index);

  const unsigned              max_nof_cu_ups;
  cu_up_processor_repository& cu_ups;
  task_executor&              cu_cp_exec;
  common_task_scheduler&      common_task_sched;
  srslog::basic_logger&       logger;

  std::map<cu_up_index_t, std::shared_ptr<shared_cu_up_connection_context>> cu_up_connections;
  std::atomic<unsigned>                                                     cu_up_count{0};

  std::atomic<bool>       stopped{false};
  std::mutex              stop_mutex;
  std::condition_variable stop_cvar;
  bool                    stop_completed = false;
};

} // namespace srs_cu_cp
} // namespace srsran
