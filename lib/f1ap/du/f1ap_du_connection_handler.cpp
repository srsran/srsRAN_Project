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

#include "f1ap_du_connection_handler.h"
#include "f1ap_du_context.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/srslog/srslog.h"
#include <thread>

using namespace srsran;
using namespace srs_du;

namespace {

/// Adapter used to forward F1AP Rx PDUs coming from the F1-C GW to the F1AP-DU.
///
/// On destruction, this class will signal to the F1AP-DU that the F1-C Rx path has been shut down (e.g. due to
/// peer connection drop).
class f1c_rx_channel final : public f1ap_message_notifier
{
public:
  f1c_rx_channel(f1ap_message_handler& msg_handler_, std::function<void()> on_connection_loss_) :
    msg_handler(msg_handler_), on_connection_loss(on_connection_loss_)
  {
  }

  ~f1c_rx_channel() override
  {
    // Signal back that the Rx path was taken down.
    on_connection_loss();
  }

  void on_new_message(const f1ap_message& msg) override { msg_handler.handle_message(msg); }

private:
  f1ap_message_handler& msg_handler;
  std::function<void()> on_connection_loss;
};

/// Adapter to forward F1AP PDUs to the F1-C GW that also manages the lifetime of the TNL connection.
class f1c_tx_channel final : public f1ap_message_notifier
{
public:
  f1c_tx_channel(std::unique_ptr<f1ap_message_notifier>& f1c_notifier_,
                 unsigned&                               epoch_,
                 srslog::basic_logger&                   logger_) :
    f1c_notifier(f1c_notifier_), epoch_ref(epoch_), logger(logger_), epoch(epoch_ref)
  {
  }
  ~f1c_tx_channel() override
  {
    if (epoch_ref == epoch) {
      // Resetting the F1-C Tx notifier should trigger the shutdown of the Rx path in the F1-C client.
      f1c_notifier.reset();
    }
  }

  void on_new_message(const f1ap_message& msg) override
  {
    if (f1c_notifier == nullptr) {
      logger.warning("Dropping F1AP Tx PDU. Cause: The F1-C TNL association is down");
      return;
    }
    f1c_notifier->on_new_message(msg);
  }

private:
  std::unique_ptr<f1ap_message_notifier>& f1c_notifier;
  unsigned&                               epoch_ref;
  srslog::basic_logger&                   logger;

  unsigned epoch;
};

} // namespace

f1ap_du_connection_handler::f1ap_du_connection_handler(f1c_connection_client& f1c_client_handler_,
                                                       f1ap_message_handler&  f1ap_pdu_handler_,
                                                       f1ap_du_configurator&  du_mng_,
                                                       f1ap_du_context&       du_ctxt_,
                                                       task_executor&         ctrl_exec_) :
  f1c_client_handler(f1c_client_handler_),
  f1ap_pdu_handler(f1ap_pdu_handler_),
  du_mng(du_mng_),
  du_ctxt(du_ctxt_),
  ctrl_exec(ctrl_exec_),
  logger(srslog::fetch_basic_logger("DU-F1"))
{
}

f1ap_du_connection_handler::~f1ap_du_connection_handler()
{
  // Check whether the F1-C TNL association was previously shutdown as part of the F1 Removal procedure.
  if (is_connected()) {
    logger.warning("F1-C TNL association was not properly shutdown before F1AP shutdown. Forcing it...");
  }

  // Tear down Tx PDU notifier, which will trigger the shutdown of the Rx path as well.
  handle_connection_loss_impl();
}

std::unique_ptr<f1ap_message_notifier> f1ap_du_connection_handler::connect_to_cu_cp()
{
  if (is_connected()) {
    logger.warning("Reconnections to CU-CP not supported");
    return nullptr;
  }

  // Create F1AP Rx PDU notifier.
  rx_path_disconnected.reset();
  auto rx_notifier = std::make_unique<f1c_rx_channel>(f1ap_pdu_handler, [this]() { handle_connection_loss(); });

  // Start F1-C TNL association and get F1AP Tx PDU notifier.
  tx_pdu_notifier = f1c_client_handler.handle_du_connection_request(std::move(rx_notifier));
  if (tx_pdu_notifier == nullptr) {
    return nullptr;
  }
  f1c_session_epoch++;

  // Connection successful.
  connected_flag = true;

  return std::make_unique<f1c_tx_channel>(tx_pdu_notifier, f1c_session_epoch, logger);
}

void f1ap_du_connection_handler::handle_connection_loss()
{
  // Signal back (via F1AP thread) that the Rx channel has been shut down.
  // Note: The caller might be in a different thread than the F1AP, so we dispatch the continuation to the F1AP
  // executor.
  // Note: We use defer, because we want to handle all the already enqueued F1AP events before the association
  // shutdown. This way no pending task is left pointing to an inexistent F1AP context.
  while (not ctrl_exec.defer([this]() { handle_connection_loss_impl(); })) {
    // Note: This set cannot fail. Keep trying.
    logger.warning("Failed to dispatch handling of F1-C Rx path disconnection. Cause: Task queue is full. Retrying...");
    std::this_thread::sleep_for(std::chrono::microseconds{10});
  }
}

void f1ap_du_connection_handler::handle_connection_loss_impl()
{
  // Mark the F1-C TNL as disconnected.
  connected_flag = false;

  // F1 is implicitly not setup anymore when the TNL association is lost.
  du_ctxt.f1c_setup = false;

  // In case of unexpected F1-C connection loss, the Tx path is still up.
  if (tx_pdu_notifier != nullptr) {
    // Disconnect Tx path.
    tx_pdu_notifier.reset();

    // Signal to DU that the F1-C connection is lost.
    du_mng.on_f1c_disconnection();
  }

  // Signal back that the F1-C Rx path has been successfully shutdown to any awaiting coroutine.
  rx_path_disconnected.set();
}

async_task<void> f1ap_du_connection_handler::handle_tnl_association_removal()
{
  return launch_async([this](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    if (not is_connected()) {
      // No need to wait for any event if the F1-C TNL association is already down.
      CORO_EARLY_RETURN();
    }

    // Stop Tx PDU path.
    // Note: This should notify the F1-C GW that the Rx path should be disconnected as well.
    tx_pdu_notifier.reset();

    // Wait for Rx PDU disconnection notification.
    CORO_AWAIT(rx_path_disconnected);

    CORO_RETURN();
  });
}
