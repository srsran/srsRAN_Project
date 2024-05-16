/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_connection_manager.h"
#include "../du_processor/du_processor_repository.h"
#include <thread>

using namespace srsran;
using namespace srs_cu_cp;

/// Notifier used to forward Rx F1AP messages from the F1-C GW to CU-CP.
class du_connection_manager::f1_gw_to_cu_cp_pdu_adapter final : public f1ap_message_notifier
{
public:
  f1_gw_to_cu_cp_pdu_adapter(du_connection_manager& parent_, du_index_t du_idx_, f1ap_message_handler& msg_handler_) :
    parent(parent_), du_idx(du_idx_), msg_handler(msg_handler_)
  {
  }

  ~f1_gw_to_cu_cp_pdu_adapter() override
  {
    // Notify DU that the connection is closed.
    parent.handle_f1c_gw_connection_closed(du_idx);
  }

  void on_new_message(const f1ap_message& msg) override { msg_handler.handle_message(msg); }

private:
  du_connection_manager& parent;
  du_index_t             du_idx;
  f1ap_message_handler&  msg_handler;
};

du_connection_manager::du_connection_manager(du_processor_repository& dus_, task_executor& cu_cp_exec_) :
  dus(dus_), cu_cp_exec(cu_cp_exec_), logger(srslog::fetch_basic_logger("CU-CP"))
{
}

std::unique_ptr<f1ap_message_notifier>
du_connection_manager::handle_new_du_connection(std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier)
{
  du_index_t du_index = dus.add_du(std::move(f1ap_tx_pdu_notifier));
  if (du_index == du_index_t::invalid) {
    logger.warning("Rejecting new DU connection. Cause: Failed to create a new DU.");
    return nullptr;
  }

  logger.info("Added TNL connection to DU {}", du_index);

  return std::make_unique<f1_gw_to_cu_cp_pdu_adapter>(
      *this,
      du_index,
      dus.get_du_processor(du_index).get_f1ap_interface().get_f1ap_handler().get_f1ap_message_handler());
}

void du_connection_manager::handle_f1c_gw_connection_closed(du_index_t du_idx)
{
  // Note: The caller may be in a different thread than the CU-CP, thus, we need to dispatch it to the CU-CP executor.
  // Note: The dispatch call cannot fail.
  while (not cu_cp_exec.defer([this, du_idx]() { dus.remove_du(du_idx); })) {
    logger.error("Failed to schedule DU removal task. Retrying...");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
