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

#include "e1ap_cu_up_connection_handler.h"
#include <utility>

using namespace srsran;
using namespace srs_cu_up;

namespace {

/// Adapter used to convert E1AP Rx PDUs coming from the CU-CP into E1AP messages.
class e1ap_rx_pdu_adapter final : public e1ap_message_notifier
{
public:
  e1ap_rx_pdu_adapter(e1ap_message_handler& msg_handler_, std::function<void()> on_connection_loss_) :
    msg_handler(msg_handler_), on_connection_loss(std::move(on_connection_loss_))
  {
  }

  ~e1ap_rx_pdu_adapter() override { on_connection_loss(); }
  void on_new_message(const e1ap_message& msg) override { msg_handler.handle_message(msg); }

private:
  e1ap_message_handler& msg_handler;
  std::function<void()> on_connection_loss;
};

} // namespace

e1ap_cu_up_connection_handler::e1ap_cu_up_connection_handler(e1_connection_client&                   e1_client_handler_,
                                                             e1ap_message_handler&                   e1ap_pdu_handler_,
                                                             e1ap_cu_up_manager_connection_notifier& cu_up_manager_,
                                                             task_executor&                          cu_up_executor_) :
  e1_client_handler(e1_client_handler_),
  e1ap_pdu_handler(e1ap_pdu_handler_),
  cu_up_manager(cu_up_manager_),
  cu_up_executor(cu_up_executor_),
  logger(srslog::fetch_basic_logger("CU-UP-E1"))
{
}

e1ap_cu_up_connection_handler::~e1ap_cu_up_connection_handler()
{
  // Check whether the N2 TNL association was previously shutdown as part of the E1 Removal procedure.
  if (is_connected()) {
    logger.warning("E1 TNL association was not properly shutdown before E1AP shutdown. Forcing it...");
  }

  // Tear down Tx PDU notifier, which will trigger the shutdown of the Rx path as well.
  handle_connection_loss_impl();
}

e1ap_message_notifier* e1ap_cu_up_connection_handler::connect_to_cu_cp()
{
  rx_path_disconnected.reset();

  e1ap_notifier = e1_client_handler.handle_cu_up_connection_request(
      std::make_unique<e1ap_rx_pdu_adapter>(e1ap_pdu_handler, [this]() { handle_connection_loss(); }));
  if (e1ap_notifier == nullptr) {
    return nullptr;
  }

  // Connection successful.
  connected_flag = true;

  return e1ap_notifier.get();
}

void e1ap_cu_up_connection_handler::handle_connection_loss()
{
  // Signal back (via CU-UP control executor) that the Rx channel has been shut down.
  // Note: The caller might be in a different thread than the E1AP, so we dispatch the continuation to the E1AP
  // executor.
  // Note: We use defer, because we want to handle all the already enqueued E1AP events before the association
  // shutdown. This way no pending task is left pointing to an inexistent E1AP context.
  while (not cu_up_executor.defer([this]() { handle_connection_loss_impl(); })) {
    // Note: This defer cannot fail. Keep trying.
    logger.warning("Failed to dispatch handling of E1 Rx path disconnection. Cause: Task queue is full. Retrying...");
    std::this_thread::sleep_for(std::chrono::microseconds{10});
  }
}

void e1ap_cu_up_connection_handler::handle_connection_loss_impl()
{
  connected_flag = false;

  // In case of unexpected E1 connection loss, the Tx path is still up.
  if (e1ap_notifier != nullptr) {
    // Disconnect Tx path.
    e1ap_notifier.reset();

    // Signal to DU that the E1 connection is lost.
    cu_up_manager.on_connection_loss();
  }

  // Signal back that the E1 Rx path has been successfully shutdown to any awaiting coroutine.
  rx_path_disconnected.set();
}

async_task<void> e1ap_cu_up_connection_handler::handle_tnl_association_removal()
{
  return launch_async([this](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    if (not is_connected()) {
      // No need to wait for any event if the E1 TNL association is already down.
      CORO_EARLY_RETURN();
    }

    // Stop Tx PDU path.
    // Note: This should notify the E1 GW that the Rx path should be disconnected as well.
    e1ap_notifier.reset();

    // Wait for Rx PDU disconnection notification.
    CORO_AWAIT(rx_path_disconnected);

    CORO_RETURN();
  });
}
