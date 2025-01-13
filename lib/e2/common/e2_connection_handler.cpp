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

#include "e2_connection_handler.h"
#include "srsran/e2/gateways/e2_connection_client.h"
#include "srsran/support/executors/task_executor.h"
#include <thread>

using namespace srsran;

namespace {

/// Adapter used to forward E2AP Rx PDUs coming from the E2 GW to the E2 Agent.
///
/// On destruction, this class will signal to the E2 Agent that the E2 Rx path has been shut down (e.g. due to
/// peer connection drop).
class e2_rx_channel final : public e2_message_notifier
{
public:
  e2_rx_channel(e2_message_handler& msg_handler_, std::function<void()> on_connection_loss_) :
    msg_handler(msg_handler_), on_connection_loss(on_connection_loss_)
  {
  }

  ~e2_rx_channel() override
  {
    // Signal back that the Rx path was taken down.
    on_connection_loss();
  }

  void on_new_message(const e2_message& msg) override { msg_handler.handle_message(msg); }

private:
  e2_message_handler&   msg_handler;
  std::function<void()> on_connection_loss;
};

/// Adapter to forward E2AP PDUs to the E2 GW that also manages the lifetime of the TNL connection.
class e2_tx_channel final : public e2_message_notifier
{
public:
  e2_tx_channel(std::unique_ptr<e2_message_notifier>& e2_notifier_, srslog::basic_logger& logger_) :
    e2_notifier(e2_notifier_), logger(logger_)
  {
  }
  ~e2_tx_channel() override
  {
    // Resetting the E2 Tx notifier should trigger the shutdown of the Rx path in the E2 client.
    e2_notifier.reset();
  }

  void on_new_message(const e2_message& msg) override
  {
    if (e2_notifier == nullptr) {
      logger.warning("Dropping E2AP Tx PDU. Cause: The E2 TNL association is down");
      return;
    }
    e2_notifier->on_new_message(msg);
  }

private:
  std::unique_ptr<e2_message_notifier>& e2_notifier;
  srslog::basic_logger&                 logger;
};

} // namespace

e2_connection_handler::e2_connection_handler(e2_connection_client&  client_handler_,
                                             e2_message_handler&    rx_pdu_handler_,
                                             e2ap_e2agent_notifier& agent_notifier_,
                                             task_executor&         ctrl_exec_) :
  client_handler(client_handler_),
  rx_pdu_handler(rx_pdu_handler_),
  agent_notifier(agent_notifier_),
  ctrl_exec(ctrl_exec_),
  logger(srslog::fetch_basic_logger("E2AP"))
{
}

e2_connection_handler::~e2_connection_handler()
{
  // Check whether the E2 TNL association was previously shutdown as part of the NG Removal procedure.
  if (is_connected()) {
    logger.warning("E2 TNL association was not properly shutdown before E2AP shutdown. Forcing it...");
  }

  // Tear down Tx PDU notifier, which will trigger the shutdown of the Rx path as well.
  handle_connection_loss_impl();
}

std::unique_ptr<e2_message_notifier> e2_connection_handler::connect_to_ric()
{
  if (is_connected()) {
    logger.warning("Reconnections to RIC not supported");
    return nullptr;
  }

  // Create E2AP Rx PDU notifier.
  rx_path_disconnected.reset();
  auto rx_notifier = std::make_unique<e2_rx_channel>(rx_pdu_handler, [this]() { handle_connection_loss(); });

  // Start E2 TNL association and get E2AP Tx PDU notifier.
  tx_pdu_notifier = client_handler.handle_e2_connection_request(std::move(rx_notifier));
  if (tx_pdu_notifier == nullptr) {
    return nullptr;
  }

  // Connection successful.
  connected_flag = true;

  return std::make_unique<e2_tx_channel>(tx_pdu_notifier, logger);
}

void e2_connection_handler::handle_connection_loss()
{
  // Signal back (via E2 E2AP thread) that the Rx channel has been shut down.
  // Note: The caller might be in a different thread than the E2AP, so we dispatch the continuation to the E2AP
  // executor.
  // Note: We use defer, because we want to handle all the already enqueued E2AP events before the association
  // shutdown. This way no pending task is left pointing to an inexistent E2AP context.
  while (not ctrl_exec.defer([this]() { handle_connection_loss_impl(); })) {
    // Note: This defer cannot fail. Keep trying.
    logger.warning("Failed to dispatch handling of E2 Rx path disconnection. Cause: Task queue is full. Retrying...");
    std::this_thread::sleep_for(std::chrono::microseconds{10});
  }
}

void e2_connection_handler::handle_connection_loss_impl()
{
  // Mark the E2 TNL as disconnected.
  connected_flag = false;

  // In case of unexpected E2 connection loss, the Tx path is still up.
  if (tx_pdu_notifier != nullptr) {
    // Disconnect Tx path.
    tx_pdu_notifier.reset();

    // Signal to Agent that the E2 connection is lost.
    agent_notifier.on_e2_disconnection();
  }

  // Signal back that the E2 Rx path has been successfully shutdown to any awaiting coroutine.
  rx_path_disconnected.set();
}

async_task<void> e2_connection_handler::handle_tnl_association_removal()
{
  return launch_async([this](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    if (not is_connected()) {
      // No need to wait for any event if the E2 TNL association is already down.
      CORO_EARLY_RETURN();
    }

    // Stop Tx PDU path.
    // Note: This should notify the E2 GW that the Rx path should be disconnected as well.
    tx_pdu_notifier.reset();

    // Wait for Rx PDU disconnection notification.
    CORO_AWAIT(rx_path_disconnected);

    CORO_RETURN();
  });
}
