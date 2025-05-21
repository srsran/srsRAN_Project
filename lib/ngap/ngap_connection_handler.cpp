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

#include "ngap_connection_handler.h"
#include "srsran/ngap/gateways/n2_connection_client.h"
#include "srsran/support/executors/task_executor.h"
#include <thread>

using namespace srsran;
using namespace srs_cu_cp;

namespace {

/// Adapter used to forward NGAP Rx PDUs coming from the N2 GW to the CU-CP NGAP.
///
/// On destruction, this class will signal to the CU-CP NGAP that the N2 Rx path has been shut down (e.g. due to
/// peer connection drop).
class n2_rx_channel final : public ngap_rx_message_notifier
{
public:
  n2_rx_channel(ngap_message_handler& msg_handler_, std::function<void()> on_connection_loss_) :
    msg_handler(msg_handler_), on_connection_loss(on_connection_loss_)
  {
  }

  ~n2_rx_channel() override
  {
    // Signal back that the Rx path was taken down.
    on_connection_loss();
  }

  void on_new_message(const ngap_message& msg) override { msg_handler.handle_message(msg); }

private:
  ngap_message_handler& msg_handler;
  std::function<void()> on_connection_loss;
};

/// Adapter to forward NGAP PDUs to the N2 GW that also manages the lifetime of the TNL connection.
class n2_tx_channel final : public ngap_message_notifier
{
public:
  n2_tx_channel(std::unique_ptr<ngap_message_notifier>& n2_notifier_, srslog::basic_logger& logger_) :
    n2_notifier(n2_notifier_), logger(logger_)
  {
  }
  ~n2_tx_channel() override
  {
    // Resetting the N2 Tx notifier should trigger the shutdown of the Rx path in the N2 client.
    n2_notifier.reset();
  }

  [[nodiscard]] bool on_new_message(const ngap_message& msg) override
  {
    if (n2_notifier == nullptr) {
      logger.warning("Dropping NGAP Tx PDU. Cause: The N2 TNL association is down");
      return false;
    }
    return n2_notifier->on_new_message(msg);
  }

private:
  std::unique_ptr<ngap_message_notifier>& n2_notifier;
  srslog::basic_logger&                   logger;
};

} // namespace

ngap_connection_handler::ngap_connection_handler(amf_index_t           amf_index_,
                                                 n2_connection_client& client_handler_,
                                                 ngap_message_handler& rx_pdu_handler_,
                                                 ngap_cu_cp_notifier&  cu_cp_notifier_,
                                                 task_executor&        ctrl_exec_) :
  amf_index(amf_index_),
  client_handler(client_handler_),
  rx_pdu_handler(rx_pdu_handler_),
  cu_cp_notifier(cu_cp_notifier_),
  ctrl_exec(ctrl_exec_),
  logger(srslog::fetch_basic_logger("NGAP"))
{
}

ngap_connection_handler::~ngap_connection_handler()
{
  // Check whether the N2 TNL association was previously shutdown as part of the NG Removal procedure.
  if (is_connected()) {
    logger.warning("N2 TNL association was not properly shutdown before NGAP shutdown. Forcing it...");
  }

  // Tear down Tx PDU notifier, which will trigger the shutdown of the Rx path as well.
  handle_connection_loss_impl();
}

std::unique_ptr<ngap_message_notifier> ngap_connection_handler::connect_to_amf()
{
  if (is_connected()) {
    logger.warning("Reconnections to AMF not supported");
    return nullptr;
  }

  // Create NGAP Rx PDU notifier.
  rx_path_disconnected.reset();
  std::unique_ptr<n2_rx_channel> rx_notifier =
      std::make_unique<n2_rx_channel>(rx_pdu_handler, [this]() { handle_connection_loss(); });

  // Start N2 TNL association and get NGAP Tx PDU notifier.
  tx_pdu_notifier = client_handler.handle_cu_cp_connection_request(std::move(rx_notifier));
  if (tx_pdu_notifier == nullptr) {
    return nullptr;
  }

  // Connection successful.
  connected_flag = true;

  return std::make_unique<n2_tx_channel>(tx_pdu_notifier, logger);
}

void ngap_connection_handler::handle_connection_loss()
{
  // Signal back (via CU-CP NGAP thread) that the Rx channel has been shut down.
  // Note: The caller might be in a different thread than the NGAP, so we dispatch the continuation to the NGAP
  // executor.
  // Note: We use defer, because we want to handle all the already enqueued NGAP events before the association
  // shutdown. This way no pending task is left pointing to an inexistent NGAP context.
  while (not ctrl_exec.defer([this]() { handle_connection_loss_impl(); })) {
    // Note: This defer cannot fail. Keep trying.
    logger.warning("Failed to dispatch handling of N2 Rx path disconnection. Cause: Task queue is full. Retrying...");
    std::this_thread::sleep_for(std::chrono::microseconds{10});
  }
}

void ngap_connection_handler::handle_connection_loss_impl()
{
  // Mark the N2 TNL as disconnected.
  connected_flag = false;

  // In case of unexpected N2 connection loss, the Tx path is still up.
  if (tx_pdu_notifier != nullptr) {
    // Disconnect Tx path.
    tx_pdu_notifier.reset();

    // Signal to DU that the N2 connection is lost.
    cu_cp_notifier.on_n2_disconnection(amf_index);
  }

  // Signal back that the N2 Rx path has been successfully shutdown to any awaiting coroutine.
  rx_path_disconnected.set();
}

async_task<void> ngap_connection_handler::handle_tnl_association_removal()
{
  return launch_async([this](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    if (not is_connected()) {
      // No need to wait for any event if the N2 TNL association is already down.
      CORO_EARLY_RETURN();
    }

    // Stop Tx PDU path.
    // Note: This should notify the N2 GW that the Rx path should be disconnected as well.
    tx_pdu_notifier.reset();

    // Wait for Rx PDU disconnection notification.
    CORO_AWAIT(rx_path_disconnected);

    CORO_RETURN();
  });
}
