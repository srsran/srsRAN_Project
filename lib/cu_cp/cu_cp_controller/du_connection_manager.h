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

#include "srsran/cu_cp/cu_cp_f1c_handler.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/executors/task_executor.h"
#include <condition_variable>

namespace srsran {
namespace srs_cu_cp {

class du_processor_repository;
class common_task_scheduler;

/// \brief This class is responsible for allocating the resources in the CU-CP required to handle the establishment
/// or drop of F1-C GW connections.
///
/// This class acts as a facade, hiding the details associated with the dispatching of F1-C GW events to the
/// the CU-CP through the appropriate task executors.
class du_connection_manager : public cu_cp_f1c_handler
{
public:
  du_connection_manager(unsigned                 max_nof_dus,
                        du_processor_repository& dus_,
                        task_executor&           cu_cp_exec_,
                        common_task_scheduler&   common_task_sched_);

  std::unique_ptr<f1ap_message_notifier>
  handle_new_du_connection(std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier) override;

  void stop();

private:
  class shared_du_connection_context;
  class f1_gw_to_cu_cp_pdu_adapter;

  // Called by the F1-C GW when it disconnects its PDU notifier endpoint.
  void handle_f1c_gw_connection_closed(du_index_t du_idx);

  const unsigned           max_nof_dus;
  du_processor_repository& dus;
  task_executor&           cu_cp_exec;
  common_task_scheduler&   common_task_sched;
  srslog::basic_logger&    logger;

  std::map<du_index_t, std::shared_ptr<shared_du_connection_context>> du_connections;
  std::atomic<unsigned>                                               du_count{0};

  std::atomic<bool>       stopped{false};
  std::mutex              stop_mutex;
  std::condition_variable stop_cvar;
  bool                    stop_completed = false;
};

} // namespace srs_cu_cp
} // namespace srsran