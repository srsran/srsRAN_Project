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

#include "du_connection_manager.h"
#include "../du_processor/du_processor_repository.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/support/executors/sync_task_executor.h"
#include <thread>

using namespace srsran;
using namespace srs_cu_cp;

/// Context of a DU connection which is shared between the du_connection_manager and the f1ap_message_notifier.
class du_connection_manager::shared_du_connection_context
{
public:
  shared_du_connection_context(du_connection_manager& parent_) : parent(parent_) {}
  shared_du_connection_context(const shared_du_connection_context&)            = delete;
  shared_du_connection_context(shared_du_connection_context&&)                 = delete;
  shared_du_connection_context& operator=(const shared_du_connection_context&) = delete;
  shared_du_connection_context& operator=(shared_du_connection_context&&)      = delete;
  ~shared_du_connection_context() { disconnect(); }

  /// Assign a DU repository index to the context. This is called when the DU repository is actually created.
  void connect_du(du_index_t du_idx_)
  {
    du_idx      = du_idx_;
    msg_handler = &parent.dus.get_du_processor(du_idx).get_f1ap_handler();
  }

  /// Determines whether a DU repository has been created for this connection.
  bool connected() const { return msg_handler != nullptr; }

  /// Deletes the associated DU repository, if it exists.
  void disconnect()
  {
    if (not connected()) {
      // DU was never allocated or was already removed.
      return;
    }

    // Notify DU that the connection is closed.
    parent.handle_f1c_gw_connection_closed(du_idx);

    du_idx      = du_index_t::invalid;
    msg_handler = nullptr;
  }

  /// Handle F1AP message coming from the DU.
  void handle_message(const f1ap_message& msg)
  {
    if (not connected()) {
      parent.logger.warning("Discarding DU F1AP message. Cause: DU connection has been closed.");
    }

    // Forward message.
    msg_handler->handle_message(msg);
  }

private:
  du_connection_manager& parent;
  du_index_t             du_idx      = du_index_t::invalid;
  f1ap_message_handler*  msg_handler = nullptr;
};

/// Notifier used to forward Rx F1AP messages from the F1-C GW to CU-CP in a thread safe manner.
class du_connection_manager::f1_gw_to_cu_cp_pdu_adapter final : public f1ap_message_notifier
{
public:
  f1_gw_to_cu_cp_pdu_adapter(du_connection_manager& parent_, std::shared_ptr<shared_du_connection_context> ctxt_) :
    parent(parent_), ctxt(std::move(ctxt_))
  {
    // Increment number of DU connections.
    parent.du_count.fetch_add(1, std::memory_order_release);
  }

  ~f1_gw_to_cu_cp_pdu_adapter() override
  {
    // Decrement the number of active DU connections.
    parent.du_count.fetch_sub(1, std::memory_order_release);

    // Defer destruction of context to CU-CP execution context.
    // Note: We make a copy of the shared_ptr of the context to extend its lifetime to when the defer callback actually
    // gets executed.
    // Note: We don't use move because the defer may fail.
    while (not parent.cu_cp_exec.defer([ctxt_cpy = ctxt]() { ctxt_cpy->disconnect(); })) {
      parent.logger.error("Failed to schedule DU removal task. Retrying...");
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  void on_new_message(const f1ap_message& msg) override
  {
    // Dispatch the F1AP Rx message handling to the CU-CP executor.
    while (not parent.cu_cp_exec.execute([this, msg]() { ctxt->handle_message(msg); })) {
      parent.logger.error("Failed to dispatch F1AP message to CU-CP. Retrying...");
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

private:
  du_connection_manager&                        parent;
  std::shared_ptr<shared_du_connection_context> ctxt;
};

du_connection_manager::du_connection_manager(unsigned                 max_nof_dus_,
                                             du_processor_repository& dus_,
                                             task_executor&           cu_cp_exec_,
                                             common_task_scheduler&   common_task_sched_) :
  max_nof_dus(max_nof_dus_),
  dus(dus_),
  cu_cp_exec(cu_cp_exec_),
  common_task_sched(common_task_sched_),
  logger(srslog::fetch_basic_logger("CU-CP"))
{
}

std::unique_ptr<f1ap_message_notifier>
du_connection_manager::handle_new_du_connection(std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier)
{
  // Note: This function may be called from a different execution context than the CU-CP.

  if (stopped.load(std::memory_order_acquire)) {
    // CU-CP is in the process of being stopped.
    return nullptr;
  }

  // Verify that there is space for new DU connection.
  if (du_count.load(std::memory_order_acquire) >= max_nof_dus) {
    logger.warning("Rejecting new DU connection. Cause: Maximum number of DUs {} reached.", max_nof_dus);
    return nullptr;
  }

  // We create a "detached" notifier, that has no associated DU processor yet.
  auto shared_ctxt     = std::make_shared<shared_du_connection_context>(*this);
  auto rx_pdu_notifier = std::make_unique<f1_gw_to_cu_cp_pdu_adapter>(*this, shared_ctxt);

  // We dispatch the task to allocate a DU processor and "attach" it to the notifier
  while (not cu_cp_exec.execute([this, shared_ctxt, sender_notifier = std::move(f1ap_tx_pdu_notifier)]() mutable {
    // Create a new DU processor.
    du_index_t du_index = dus.add_du(std::move(sender_notifier));
    if (du_index == du_index_t::invalid) {
      logger.warning("Rejecting new DU connection. Cause: Failed to create a new DU.");
      return;
    }

    // Register the allocated DU processor index in the DU connection context.
    shared_ctxt->connect_du(du_index);

    if (not du_connections.insert(std::make_pair(du_index, std::move(shared_ctxt))).second) {
      logger.error("Failed to store new DU connection {}", du_index);
      return;
    }

    logger.info("Added TNL connection to DU {}", du_index);
  })) {
    logger.debug("Failed to dispatch CU-CP DU connection task. Retrying...");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return rx_pdu_notifier;
}

void du_connection_manager::handle_f1c_gw_connection_closed(du_index_t du_idx)
{
  // Note: Called from within CU-CP execution context.

  common_task_sched.schedule_async_task(launch_async([this, du_idx](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    if (du_connections.find(du_idx) == du_connections.end()) {
      // DU was already removed.
      CORO_EARLY_RETURN();
    }

    // Await for clean removal of the DU from the DU repository.
    CORO_AWAIT(dus.remove_du(du_idx));

    // Mark the connection as closed.
    du_connections.erase(du_idx);

    // Flag that all DUs got removed.
    if (stopped and du_connections.empty()) {
      std::unique_lock<std::mutex> lock(stop_mutex);
      stop_completed = true;
      stop_cvar.notify_one();
    }

    CORO_RETURN();
  }));
}

void du_connection_manager::stop()
{
  // Note: Called from outside of the CU-CP execution context.
  stop_completed = false;
  stopped        = true;

  while (not cu_cp_exec.execute([this]() mutable {
    if (du_connections.empty()) {
      // No DUs connected. Notify completion.
      std::unique_lock<std::mutex> lock(stop_mutex);
      stop_completed = true;
      stop_cvar.notify_one();
      return;
    }

    // For each created DU connection context, launch the deletion routine.
    std::vector<du_index_t> du_idxs;
    du_idxs.reserve(du_connections.size());
    for (const auto& [du_idx, ctxt] : du_connections) {
      du_idxs.push_back(du_idx);
    }
    for (du_index_t du_idx : du_idxs) {
      // Disconnect DU notifier.
      du_connections[du_idx]->disconnect();
    }
  })) {
    logger.debug("Failed to dispatch CU-CP DU disconnection task. Retrying...");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Wait for DU stop to complete.
  {
    std::unique_lock<std::mutex> lock(stop_mutex);
    stop_cvar.wait(lock, [this] { return stop_completed; });
  }
}
