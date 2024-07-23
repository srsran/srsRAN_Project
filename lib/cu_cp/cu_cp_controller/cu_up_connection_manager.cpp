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

#include "cu_up_connection_manager.h"
#include "../cu_up_processor/cu_up_processor_repository.h"
#include "common_task_scheduler.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/support/executors/sync_task_executor.h"
#include <thread>

using namespace srsran;
using namespace srs_cu_cp;

/// Context of a CU-UP connection which is shared between the cu_up_connection_manager and the e1ap_message_notifier.
class cu_up_connection_manager::shared_cu_up_connection_context
{
public:
  shared_cu_up_connection_context(cu_up_connection_manager& parent_) : parent(parent_) {}
  shared_cu_up_connection_context(const shared_cu_up_connection_context&)            = delete;
  shared_cu_up_connection_context(shared_cu_up_connection_context&&)                 = delete;
  shared_cu_up_connection_context& operator=(const shared_cu_up_connection_context&) = delete;
  shared_cu_up_connection_context& operator=(shared_cu_up_connection_context&&)      = delete;
  ~shared_cu_up_connection_context() { disconnect(); }

  /// Assign a CU-UP repository index to the context. This is called when the CU-UP repository is actually created.
  void connect_cu_up(cu_up_index_t cu_up_idx_)
  {
    cu_up_idx   = cu_up_idx_;
    msg_handler = &parent.cu_ups.get_cu_up(cu_up_idx).get_e1ap_message_handler();
  }

  /// Determines whether a CU-UP repository has been created for this connection.
  bool connected() const { return msg_handler != nullptr; }

  /// Deletes the associated CU-UP repository, if it exists.
  void disconnect()
  {
    if (not connected()) {
      // CU-UP was never allocated or was already removed.
      return;
    }

    // Notify CU-UP that the connection is closed.
    parent.handle_e1_gw_connection_closed(cu_up_idx);

    cu_up_idx   = cu_up_index_t::invalid;
    msg_handler = nullptr;
  }

  /// Handle E1AP message coming from the CU-UP.
  void handle_message(const e1ap_message& msg)
  {
    if (not connected()) {
      parent.logger.warning("Discarding CU-UP E1AP message. Cause: CU-UP connection has been closed.");
    }

    // Forward message.
    msg_handler->handle_message(msg);
  }

private:
  cu_up_connection_manager& parent;
  cu_up_index_t             cu_up_idx   = cu_up_index_t::invalid;
  e1ap_message_handler*     msg_handler = nullptr;
};

/// Notifier used to forward Rx E1AP messages from the E1 GW to CU-CP in a thread safe manner.
class cu_up_connection_manager::e1_gw_to_cu_cp_pdu_adapter final : public e1ap_message_notifier
{
public:
  e1_gw_to_cu_cp_pdu_adapter(cu_up_connection_manager&                        parent_,
                             std::shared_ptr<shared_cu_up_connection_context> ctxt_) :
    parent(parent_), ctxt(std::move(ctxt_))
  {
    // Increment number of CU-UP connections.
    parent.cu_up_count.fetch_add(1, std::memory_order_release);
  }

  ~e1_gw_to_cu_cp_pdu_adapter() override
  {
    // Decrement the number of active CU-UP connections.
    parent.cu_up_count.fetch_sub(1, std::memory_order_release);

    // Defer destruction of context to CU-CP execution context.
    // Note: We make a copy of the shared_ptr of the context to extend its lifetime to when the defer callback actually
    // gets executed.
    // Note: We don't use move because the defer may fail.
    while (not parent.cu_cp_exec.defer([ctxt_cpy = ctxt]() { ctxt_cpy->disconnect(); })) {
      parent.logger.error("Failed to schedule CU-UP removal task. Retrying...");
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  void on_new_message(const e1ap_message& msg) override
  {
    // Dispatch the E1AP Rx message handling to the CU-CP executor.
    while (not parent.cu_cp_exec.execute([this, msg]() { ctxt->handle_message(msg); })) {
      parent.logger.error("Failed to dispatch E1AP message to CU-CP. Retrying...");
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

private:
  cu_up_connection_manager&                        parent;
  std::shared_ptr<shared_cu_up_connection_context> ctxt;
};

cu_up_connection_manager::cu_up_connection_manager(unsigned                    max_nof_cu_ups_,
                                                   cu_up_processor_repository& cu_ups_,
                                                   task_executor&              cu_cp_exec_,
                                                   common_task_scheduler&      common_task_sched_) :
  max_nof_cu_ups(max_nof_cu_ups_),
  cu_ups(cu_ups_),
  cu_cp_exec(cu_cp_exec_),
  common_task_sched(common_task_sched_),
  logger(srslog::fetch_basic_logger("CU-CP"))
{
}

std::unique_ptr<e1ap_message_notifier>
cu_up_connection_manager::handle_new_cu_up_connection(std::unique_ptr<e1ap_message_notifier> e1ap_tx_pdu_notifier)
{
  // Note: This function may be called from a different execution context than the CU-CP.

  if (stopped.load(std::memory_order_acquire)) {
    // CU-CP is in the process of being stopped.
    return nullptr;
  }

  // Verify that there is space for new CU-UP connection.
  if (cu_up_count.load(std::memory_order_acquire) >= max_nof_cu_ups) {
    logger.warning("Rejecting new CU-UP connection. Cause: Maximum number of CU-UPs {} reached.", max_nof_cu_ups);
    return nullptr;
  }

  // We create a "detached" notifier, that has no associated CU-UP processor yet.
  auto shared_ctxt     = std::make_shared<shared_cu_up_connection_context>(*this);
  auto rx_pdu_notifier = std::make_unique<e1_gw_to_cu_cp_pdu_adapter>(*this, shared_ctxt);

  // We dispatch the task to allocate a CU-UP processor and "attach" it to the notifier
  while (not cu_cp_exec.execute([this, shared_ctxt, sender_notifier = std::move(e1ap_tx_pdu_notifier)]() mutable {
    // Create a new CU-UP processor.
    cu_up_index_t cu_up_index = cu_ups.add_cu_up(std::move(sender_notifier));
    if (cu_up_index == cu_up_index_t::invalid) {
      logger.warning("Rejecting new CU-UP TNL connection. Cause: Failed to create a new CU-UP.");
      return;
    }

    // Register the allocated CU-UP processor index in the CU-UP connection context.
    shared_ctxt->connect_cu_up(cu_up_index);

    if (not cu_up_connections.insert(std::make_pair(cu_up_index, std::move(shared_ctxt))).second) {
      logger.error("Failed to store new CU-UP connection {}", cu_up_index);
      return;
    }

    logger.info("Added TNL connection to CU-UP {}", cu_up_index);
  })) {
    logger.debug("Failed to dispatch CU-CP CU-UP connection task. Retrying...");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return rx_pdu_notifier;
}

void cu_up_connection_manager::handle_e1_gw_connection_closed(cu_up_index_t cu_up_idx)
{
  // Note: Called from within CU-CP execution context.

  common_task_sched.schedule_async_task(launch_async([this, cu_up_idx](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    if (cu_up_connections.find(cu_up_idx) == cu_up_connections.end()) {
      // CU-UP was already removed.
      CORO_EARLY_RETURN();
    }

    // Await for clean removal of the CU-UP from the CU-UP repository.
    CORO_AWAIT(cu_ups.remove_cu_up(cu_up_idx));

    // Mark the connection as closed.
    cu_up_connections.erase(cu_up_idx);

    // Flag that all CU-UPs got removed.
    if (stopped and cu_up_connections.empty()) {
      std::unique_lock<std::mutex> lock(stop_mutex);
      stop_completed = true;
      stop_cvar.notify_one();
    }

    CORO_RETURN();
  }));
}

void cu_up_connection_manager::stop()
{
  // Note: Called from outside of the CU-CP execution context.
  stop_completed = false;
  stopped        = true;

  while (not cu_cp_exec.execute([this]() mutable {
    if (cu_up_connections.empty()) {
      // No CU-UPs connected. Notify completion.
      std::unique_lock<std::mutex> lock(stop_mutex);
      stop_completed = true;
      stop_cvar.notify_one();
      return;
    }

    // For each created CU-UP connection context, launch the deletion routine.
    std::vector<cu_up_index_t> cu_up_idxs;
    cu_up_idxs.reserve(cu_up_connections.size());
    for (const auto& [cu_up_idx, ctxt] : cu_up_connections) {
      cu_up_idxs.push_back(cu_up_idx);
    }
    for (cu_up_index_t cu_up_idx : cu_up_idxs) {
      // Disconnect CU-UP notifier.
      cu_up_connections[cu_up_idx]->disconnect();
    }
  })) {
    logger.debug("Failed to dispatch CU-CP CU-UP disconnection task. Retrying...");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Wait for CU-UP stop to complete.
  {
    std::unique_lock<std::mutex> lock(stop_mutex);
    stop_cvar.wait(lock, [this] { return stop_completed; });
  }
}
